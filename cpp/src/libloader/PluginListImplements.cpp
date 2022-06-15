// Copyright (c) 2022 - 2022. Antares, Eritque arcus and contributors.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or any later version(in your opinion).
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

/// 本文件的目的是分离出PluginListManager交互的部分函数实现，方便代码的阅读和维护

#include "PluginListImplements.h"
#include "LoaderLogger.h"
#include "PluginConfig.h"
#include "PluginListManager.h"
#include "ThreadController.h"
#include "libOpen.h"
#include "loaderTools.h"
#include <future>
#include <jni.h>


namespace LibLoader {
    struct PluginAddrInfo {
        plugin_func_ptr event_func;
        const MiraiCP::PluginConfig *pluginAddr;
    };

    ////////////////////////////////////
    /// 这部分是一些工具函数、对象
    // todo(Antares): 这部分的path全部换成id

    /// 该对象的地址将被用于传递给MiraiCP插件
    constexpr static LoaderApi::interface_funcs interfaces = LoaderApi::collect_interface_functions(true);

    constexpr static LoaderApi::interface_funcs normal_interfaces = LoaderApi::collect_interface_functions(false);

    void callEntranceFuncAdmin(plugin_entrance_func_ptr func) {
        func(interfaces);
    }

    void callEntranceFuncNormal(plugin_entrance_func_ptr func) {
        func(normal_interfaces);
    }

    /// 测试符号存在性，并返回event func的地址。return {nullptr, nullptr} 代表符号测试未通过，
    /// 不是一个可以使用的MiraiCP插件
    /// 不会调用其中任何一个函数
    PluginAddrInfo testSymbolExistance(plugin_handle handle, const std::string &path) {
        // using static keyword; don't capture any data
        static auto errorMsg = [](const std::string &path) -> PluginAddrInfo {
            logger.error("failed to find symbol in plugin at location: " + path);
            return {nullptr, nullptr};
        };

        auto symbol = LoaderApi::libSymbolLookup(handle, STRINGIFY(FUNC_ENTRANCE));
        if (!symbol) return errorMsg(path);

        symbol = LoaderApi::libSymbolLookup(handle, STRINGIFY(FUNC_EXIT));
        if (!symbol) return errorMsg(path);

        auto event_addr = (plugin_func_ptr) LoaderApi::libSymbolLookup(handle, STRINGIFY(FUNC_EVENT));
        if (!event_addr) return errorMsg(path);

        auto pluginInfo = (const MiraiCP::PluginConfig *) LoaderApi::libSymbolLookup(handle, STRINGIFY(PLUGIN_INFO));
        if (!pluginInfo) return errorMsg(path);

        return {event_addr, pluginInfo};
    }

    ////////////////////////////////////

    void disable_plugin(LoaderPluginConfig &plugin) {
        if (nullptr == plugin.handle) {
            logger.error("plugin " + plugin.getId() + " is not loaded!");
            return;
        }
        if (!plugin.enabled) {
            logger.warning("plugin " + plugin.getId() + " is already disabled");
            return;
        }

        auto disable_func = (plugin_func_ptr) LoaderApi::libSymbolLookup(plugin.handle, STRINGIFY(FUNC_EXIT));
        ThreadController::getController().callThreadEnd(plugin.config->id, disable_func);
        plugin.disable();
    }

    // unload将释放插件的内存
    // 不涉及插件列表的修改；不会修改插件权限
    void unload_plugin(LoaderPluginConfig &plugin) {
        if (nullptr == plugin.handle) {
            logger.warning("plugin " + plugin.config->id + " is already unloaded");
            return;
        }

        // first disable it
        if (plugin.enabled) {
            disable_plugin(plugin);
        }

        // then unload it
        LoaderApi::libClose(plugin.handle);
        plugin.unload();
    }

    // enable的前提是该插件已经被加载进内存，但尚未执行初始化函数，或者执行了Exit
    // 不涉及插件列表的修改；不会修改插件权限
    void enable_plugin(LoaderPluginConfig &plugin) {
        if (plugin.enabled) {
            logger.warning("plugin " + plugin.getId() + " is already enabled");
            return;
        }
        if (plugin.handle == nullptr) {
            logger.error("plugin " + plugin.getId() + " is not loaded!");
            return;
        }

        auto func = (plugin_entrance_func_ptr) LoaderApi::libSymbolLookup(plugin.handle, STRINGIFY(FUNC_ENTRANCE));
        ThreadController::getController().addThread(plugin.getId(), [&]() {
            callEntranceFuncAdmin(func);
        });
        plugin.enable();
    }

    void load_plugin(LoaderPluginConfig &plugin, bool alsoEnablePlugin) {
        if (plugin.handle != nullptr) {
            logger.error("plugin at location " + plugin.path + " is already loaded!");
            return;
        }

        auto handle = LoaderApi::libOpen(plugin.path);
        if (handle == nullptr) {
            logger.error("failed to load plugin at location " + plugin.path);
            return;
        }

        auto symbols = testSymbolExistance(handle, plugin.path);
        if (symbols.pluginAddr == nullptr || symbols.event_func == nullptr) {
            logger.error("failed to find plugin symbol at location " + plugin.path);
            return;
        }

        plugin.load(handle, symbols.event_func, symbols.pluginAddr);

        plugin.disable();
        if (alsoEnablePlugin) enable_plugin(plugin);
    }

    ////////////////////////////////////

    void loadNewPluginByPath(const std::string &_path, bool activateNow) {
        void *handle = LoaderApi::libOpen(_path);
        if (handle != nullptr) {
            logger.error("failed to load new plugin: " + _path);
            return;
        }
        auto pluginInfo = testSymbolExistance(handle, _path);
        if (nullptr == pluginInfo.pluginAddr || nullptr == pluginInfo.event_func) {
            LoaderApi::libClose(handle);
            return;
        }

        LoaderPluginConfig cfg{_path};

        cfg.load(handle, pluginInfo.event_func, pluginInfo.pluginAddr);
        cfg.disable();
        if (activateNow) enable_plugin(cfg);

        PluginListManager::addNewPlugin(std::move(cfg));
    }

    /// 仅加载并测试所有plugin的符号是否存在
    /// 仅被registerAllPlugin调用，即，在kt（主）线程Verify中会被调用一次
    /// 作用是将所有plugin加入plugin列表
    /// 这一过程必须是原子的
    void loadsAll(const std::vector<std::string> &paths, const std::vector<PluginAuthority> &authorities) {
        std::lock_guard lk(PluginListManager::getLock());

        for (int i = 0; i < paths.size(); ++i) {
            auto &path = paths[i];
            auto authority = authorities[i];

            auto timestamp = std::chrono::system_clock::now();

            plugin_handle handle = LoaderApi::libOpen(path);
            if (!handle) {
                logger.error("failed to load plugin at " + path);
                continue;
            }

            // test symbol existance
            auto pluginInfo = testSymbolExistance(handle, path);
            if (nullptr == pluginInfo.pluginAddr || nullptr == pluginInfo.event_func) {
                LoaderApi::libClose(handle);
                continue;
            }

            auto timestamp2 = std::chrono::system_clock::now();

            auto timedelta = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp2 - timestamp).count();
            logger.info("loaded plugin " + path + " in " + std::to_string(timedelta) + "ms");

            LoaderPluginConfig cfg{path};
            cfg.load(handle, pluginInfo.event_func, pluginInfo.pluginAddr);
            cfg.disable();
            cfg.authority = authority;

            PluginListManager::addNewPlugin(std::move(cfg));
        }
    }

    /// 激活目前所有存储的插件。在Verify步骤中被kt（主）线程调用且仅调用一次
    /// 实际的入口，id_plugin_list 必须在这里初始化
    void registerAllPlugin(jstring _cfgPath) {
        // 获取cfg文件路径并读取
        std::string cfgPath = jstring2str(_cfgPath);
        nlohmann::json j = readJsonFile(cfgPath);

        // 读取全部path
        auto [paths, authorities] = collect_plugins(cfgPath, std::move(j));

        if (paths.empty()) {
            logger.warning("no plugin to load");
            return;
        }

        loadsAll(paths, authorities);
    }
} // namespace LibLoader
