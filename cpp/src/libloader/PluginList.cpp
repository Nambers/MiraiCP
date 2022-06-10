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

#include "PluginList.h"
#include "JNIEnvs.h"
#include "LoaderLogger.h"
#include "PluginConfig.h"
#include "ThreadController.h"
#include "libOpen.h"
#include "loaderApi.h"
#include "loaderTools.h"
#include <future>
#include <mutex>
#include <string>

namespace LibLoader {
    using LoaderApi::libClose;
    using LoaderApi::libOpen;
    using LoaderApi::libSymbolLookup;
    // todo(Antares): 文件过于复杂，需要重构。需要隔离所有复杂函数中对plugin_list的直接访问，改用一些inline函数获取信息；以及插件使用插件id索引！

    static PluginList plugin_list;
    static std::recursive_mutex pluginlist_mtx;
    constexpr static LoaderApi::interface_funcs interfaces = LoaderApi::collect_interface_functions();

    inline void callEntranceFunc(plugin_entrance_func_ptr func) {
        func(interfaces);
    }

    inline void addPlugin(MiraiCPPluginConfig cfg) {
        std::lock_guard lk(pluginlist_mtx);
        auto cfgptr = (MiraiCP::PluginConfig *) libSymbolLookup(cfg.handle, STRINGIFY(PLUGIN_INFO));
        if (cfgptr == nullptr) {
            logger.error("Runtime Error: plugin config does not exist, did you forget to check symbol existance?");
            return;
        }
        plugin_list.insert(std::make_pair(cfgptr->id, std::move(cfg)));
    }

    // test symbol existance and return the event func addr. return nullptr if fails. (not calling them)
    plugin_func_ptr testSymbolExistance(void *handle, const std::string &path) {
        // using static keyword; don't capture any data
        static auto errorMsg = [](const std::string &path) {
            logger.error("failed to find symbol in plugin " + path);
            return nullptr;
        };

        auto symbol = libSymbolLookup(handle, STRINGIFY(FUNC_ENTRANCE));
        if (!symbol) return errorMsg(path);

        symbol = libSymbolLookup(handle, STRINGIFY(FUNC_EXIT));
        if (!symbol) return errorMsg(path);

        auto event_addr = (plugin_func_ptr) libSymbolLookup(handle, STRINGIFY(FUNC_EVENT));
        if (!event_addr) return errorMsg(path);

        auto pluginInfo = libSymbolLookup(handle, STRINGIFY(PLUGIN_INFO));
        if (!pluginInfo) return errorMsg(path);

        return event_addr;
    }


    ////////////////////////////////////
    // api
    void unload_plugin(MiraiCPPluginConfig &plugin) {
        if (!plugin.handle) {
            logger.warning("plugin " + plugin.path + " is already unloaded");
            return;
        }

        auto disable_func = (plugin_func_ptr) libSymbolLookup(plugin.handle, STRINGIFY(FUNC_EXIT));
        ThreadController::getController().callThreadEnd(plugin.path, disable_func);
        libClose(plugin.handle);
        plugin.handle = nullptr;
    }

    void enable_plugin(MiraiCPPluginConfig &plugin) {
        if (plugin.handle) {
            logger.warning("plugin " + plugin.path + " is already disabled");
            return;
        }

        plugin.handle = libOpen(plugin.path);

        if (nullptr == plugin.handle) {
            logger.error("failed to load plugin " + plugin.path);
            return;
        }

        auto event_func = testSymbolExistance(plugin.handle, plugin.path);
        if (nullptr == event_func) {
            logger.error("failed to read symbol in plugin " + plugin.path);
            libClose(plugin.handle);
            plugin.handle = nullptr;
            return;
        }

        auto func = (plugin_entrance_func_ptr) libSymbolLookup(plugin.handle, STRINGIFY(FUNC_ENTRANCE));
        ThreadController::getController().addThread(plugin.path, [&]() {
            callEntranceFunc(func); // discard return value
        });

        plugin.eventFunc = event_func;
    }

    // 仅加载并测试所有plugin的符号是否存在
    void loadsAll(const std::vector<std::string> &paths) {
        for (auto &&path: paths) {
            auto timestamp = std::chrono::system_clock::now();

            void *handle = libOpen(path);
            if (!handle) {
                logger.error("failed to load plugin at " + path);
                continue;
            }

            // test symbol existance
            auto eventFuncAddr = testSymbolExistance(handle, path);
            if (nullptr == eventFuncAddr) {
                libClose(handle);
                continue;
            }

            auto timestamp2 = std::chrono::system_clock::now();

            auto timedelta = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp2 - timestamp).count();
            logger.info("loaded plugin " + path + " in " + std::to_string(timedelta) + "ms");

            MiraiCPPluginConfig cfg{path, handle, eventFuncAddr};
            addPlugin(std::move(cfg));
        }
    }

    void loadNewPluginByPath(const std::string &_path, bool activateNow) {
        void *handle = libOpen(_path);
        if (handle != nullptr) {
            logger.error("failed to load new plugin: " + _path);
            return;
        }
        auto eventFuncAddr = testSymbolExistance(handle, _path);
        if (nullptr == eventFuncAddr) {
            libClose(handle);
            return;
        }

        addPlugin({_path, handle, eventFuncAddr});
    }


    void reloadAllPlugin(jstring _cfgPath) {
        std::lock_guard lk(pluginlist_mtx);
        unloadAll();
        plugin_list.clear();

        std::string cfgPath = jstring2str(_cfgPath);
        // todo(antares): this function is not finished
    }

    /// the entrance to load all plugins (not activate)
    void registerAllPlugin(jstring _cfgPath) {
        if (!plugin_list.empty()) {
            logger.warning("Plugin is already loaded");
            return;
        }

        std::string cfgPath = jstring2str(_cfgPath);

        nlohmann::json j = readJsonFile(cfgPath);

        auto paths = collect_plugins(cfgPath, std::move(j));

        if (paths.empty()) {
            logger.warning("no plugin to load");
            return;
        }

        loadsAll(paths);
    }

    /// activate all loaded plugins
    // todo(antares): logger指针也要传递给插件
    void activateAllPlugins() {
        std::lock_guard lk(pluginlist_mtx);
        auto len = plugin_list.size();
        std::unique_ptr<std::promise<void>[]> promiselist(new std::promise<void>[len]);
        std::unique_ptr<std::future<void>[]> futurelist(new std::future<void>[len]);

        int i = 0;
        for (auto &&[k, v]: plugin_list) {
            auto &pr = promiselist[i];
            auto &fu = futurelist[i];
            fu = pr.get_future();

            auto entrance = (plugin_entrance_func_ptr) libSymbolLookup(v.handle, STRINGIFY(FUNC_ENTRANCE));

            ThreadController::getController().addThread(k, [&]() {
                callEntranceFunc(entrance);
                pr.set_value();
            });
            ++i;
        }

        for (i = 0; i < len; ++i) {
            futurelist[i].get();
        }
    }

    ////////////////////////////////////


    void loader_enablePluginById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = plugin_list.find(id);
        if (it == plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        enable_plugin(it->second);
    }

    void loader_disablePluginById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = plugin_list.find(id);
        if (it == plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        // disable todo
    }

    void loader_enableAllPlugins() {
        std::lock_guard lk(pluginlist_mtx);
        enableAll();
    }

    void loader_disableAllPlugins() {
        std::lock_guard lk(pluginlist_mtx);
        disableAll();
    }

    void loader_loadNewPlugin(const std::string &path, bool activateNow) {
        std::lock_guard lk(pluginlist_mtx);
        loadNewPluginByPath(path, activateNow);
    }

    void loader_unloadPluginById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = plugin_list.find(id);
        if (it == plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        unload_plugin(it->second);
    }

    void enableAll() {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: plugin_list) {
            enable_plugin(v);
        }
    }

    void disableAll() {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: plugin_list) {
            // disableAll todo
        }
    }

    void unloadAll() {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: plugin_list) {
            unload_plugin(v);
        }
    }

    std::vector<std::string> getAllPluginName() {
        std::lock_guard lk(pluginlist_mtx);
        std::vector<std::string> ans;
        for (auto &&[k, v]: plugin_list) {
            ans.emplace_back(k);
        }
        return ans;
    }
} // namespace LibLoader
