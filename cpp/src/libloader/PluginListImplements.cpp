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
    constexpr static LoaderApi::interface_funcs interfaces = LoaderApi::collect_interface_functions();

    inline void callEntranceFunc(plugin_entrance_func_ptr func) {
        func(interfaces);
    }

    /// 测试符号存在性，并返回event func的地址。return nullptr代表符号测试未通过，不是一个可以使用的MiraiCP插件
    /// 不会调用其中任何一个函数
    PluginAddrInfo testSymbolExistance(plugin_handle handle, const std::string &path) {
        // using static keyword; don't capture any data
        static auto errorMsg = [](const std::string &path) -> PluginAddrInfo {
            logger.error("failed to find symbol in plugin " + path);
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

    void unload_plugin(LoaderPluginConfig &plugin) {
        if (!plugin.handle) {
            logger.warning("plugin " + plugin.config->id + " is already unloaded");
            return;
        }

        auto disable_func = (plugin_func_ptr) LoaderApi::libSymbolLookup(plugin.handle, STRINGIFY(FUNC_EXIT));
        ThreadController::getController().callThreadEnd(plugin.config->id, disable_func);
        LoaderApi::libClose(plugin.handle);
        plugin.reset();
    }

    void enable_plugin(LoaderPluginConfig &plugin) {
        if (plugin.handle) {
            logger.warning("plugin " + plugin.path + " is already disabled");
            return;
        }

        plugin.handle = LoaderApi::libOpen(plugin.path);

        if (nullptr == plugin.handle) {
            logger.error("failed to load plugin " + plugin.path);
            return;
        }

        auto addrInfo = testSymbolExistance(plugin.handle, plugin.path);
        if (nullptr == addrInfo.event_func || nullptr == addrInfo.pluginAddr) {
            logger.error("failed to read symbol in plugin " + plugin.path);
            LoaderApi::libClose(plugin.handle);
            plugin.handle = nullptr;
            return;
        }

        auto func = (plugin_entrance_func_ptr) LoaderApi::libSymbolLookup(plugin.handle, STRINGIFY(FUNC_ENTRANCE));
        ThreadController::getController().addThread(plugin.path, [&]() {
            callEntranceFunc(func); // discard return value
        });

        plugin.eventFunc = addrInfo.event_func;
        plugin.config = addrInfo.pluginAddr;
    }

    /// 仅加载并测试所有plugin的符号是否存在
    void loadsAll(const std::vector<std::string> &paths) {
        for (auto &&path: paths) {
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

            LoaderPluginConfig cfg{path, handle, pluginInfo.event_func, pluginInfo.pluginAddr};
            PluginListManager::addPlugin(std::move(cfg));
        }
    }

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

        PluginListManager::addPlugin({_path, handle, pluginInfo.event_func, pluginInfo.pluginAddr});
        // todo 设计上应该是全部都要当场init，不存在activateNow == false的情况?
        // todo(Antares): 这里代码写错了，init的签名见commonTypes.h: plugin_entrance_func_ptr，
        //  之后把这一段重新设计一下
        //  关于activateNow的这部分，最开始的想法是load和enable的区别，
        //  activateNow=true就相当于是load+enable；false的话不enable，不会被event传递
        //  实际考虑的时候确实有欠缺，需要再讨论一下
        // auto init = (init_func_ptr) LoaderApi::libSymbolLookup(handle, STRINGIFY(FUNC_ENTRANCE));
        // init(JNIEnvManager::getGvm(), JNIEnvs::Class_cpplib, logger.logMethod, JNIEnvs::koper);
    }

    ////////////////////////////////////

    /// 激活目前所有存储的插件。在Verify步骤中被kt（主）线程调用且仅调用一次
    /// 实际的入口，plugin_list 必须在这里初始化
    void registerAllPlugin(jstring _cfgPath) {
        // 获取cfg文件路径并读取
        std::string cfgPath = jstring2str(_cfgPath);
        nlohmann::json j = readJsonFile(cfgPath);

        // 读取全部path
        // todo(Antares): 增加插件权限的读取，需要讨论一下json格式
        auto paths = collect_plugins(cfgPath, std::move(j));

        if (paths.empty()) {
            logger.warning("no plugin to load");
            return;
        }

        loadsAll(paths);
    }

    /// 激活目前所有存储的插件。被loader线程调用
    /// 该函数在插件被 registerAllPlugin 全部注册完成后，调用一次
    // todo(Antares): 需要讨论一下是否需要loader线程等待所有线程激活完成
    //  可能是不必要的，如果能保证callEntranceFunc不会使得loader线程出现冲突
    void activateAllPlugins() {
        std::lock_guard lk(PluginListManager::getLock());

        auto len = PluginListManager::count();

        std::unique_ptr<std::promise<void>[]> promiselist(new std::promise<void>[len]);
        std::unique_ptr<std::future<void>[]> futurelist(new std::future<void>[len]);

        int i = 0;

        std::function f = [&](const std::string &k, const LoaderPluginConfig &v) {
            auto &pr = promiselist[i];
            auto &fu = futurelist[i];
            fu = pr.get_future();

            auto entrance = (plugin_entrance_func_ptr) LoaderApi::libSymbolLookup(v.handle, STRINGIFY(FUNC_ENTRANCE));

            /// 创建新线程并指派第一个任务，激活插件并set value给future对象
            ThreadController::getController().addThread(k, [&]() {
                callEntranceFunc(entrance);
                pr.set_value();
            });
            ++i;
        };

        /// 此处开始执行插件的激活
        PluginListManager::run_over_pluginlist(f);

        for (i = 0; i < len; ++i) {
            futurelist[i].get();
        }
    }
} // namespace LibLoader