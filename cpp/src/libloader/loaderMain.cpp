//
// Created by antares on 5/20/22.
//

#include "loaderMain.h"
#include "JNIEnvManager.h"
#include "JNIEnvs.h"
#include "LoaderLogger.h"
#include "ThreadController.h"
#include "commonTypes.h"
#include "json.hpp"
#include "libOpen.h"
#include "loaderApi.h"
#include "loaderTools.h"
#include <future>

#define FUNC_ENTRANCE "enrollPlugin"
#define FUNC_EVENT "eventHandle"
#define FUNC_EXIT "onDisable"


namespace LibLoader {

    namespace LoaderGlobals {
        static std::recursive_mutex pluginlist_mtx;
        static PluginList plugin_list;
        constexpr static interface_funcs interfaces = collect_interface_functions();

        enum LOADER_TASKS {
            ADD_THREAD,
            END_THREAD,
            ENABLE_ALL,
            DISABLE_ALL,
            LOAD_NEW_ACTIVATENOW,
            LOAD_NEW_DONTACTIVATE,
            TASK_TYPES_COUNT,
        };

        typedef std::pair<LOADER_TASKS, std::string> loadertask;
        static std::queue<loadertask> loader_thread_task_queue;
        static std::recursive_mutex task_mtx;

    } // namespace LoaderGlobals

    inline void callEntranceFunc(plugin_entrance_func_ptr func) {
        func(LoaderGlobals::interfaces, JNIEnvManager::getGvm());
    }

    ////////////////////////////////////
    // api
    void disable_plugin(MiraiCPPluginConfig &plugin) {
        if (!plugin.handle) {
            JNIEnvs::logger.warning("plugin " + plugin.path + " is already disabled");
            return;
        }

        auto disable_func = (plugin_func_ptr) libSymbolLookup(plugin.handle, FUNC_EXIT);
        ThreadController::getController().callThreadEnd(plugin.path, disable_func);
        libClose(plugin.handle);
        plugin.handle = nullptr;
    }

    void enable_plugin(MiraiCPPluginConfig &plugin) {
        if (plugin.handle) {
            JNIEnvs::logger.warning("plugin " + plugin.path + " is already disabled");
            return;
        }
        plugin.handle = libOpen(plugin.path);
        auto func = (plugin_entrance_func_ptr) libSymbolLookup(plugin.handle, FUNC_ENTRANCE);
        ThreadController::getController().addThread(plugin.path, [&]() {
            callEntranceFunc(func); // discard return value
        });
    }

    // test symbol existance (not calling them)
    bool testSymbolExistance(void *handle, const std::string &path) {
        // using static keyword; don't capture any data
        static auto errorMsg = [](const std::string &path) {
            JNIEnvs::logger.error("failed to find symbol in plugin " + path);
            return false;
        };

        auto func_addr = libSymbolLookup(handle, FUNC_ENTRANCE);
        if (!func_addr) return errorMsg(path);

        func_addr = libSymbolLookup(handle, FUNC_EVENT);
        if (!func_addr) return errorMsg(path);

        func_addr = libSymbolLookup(handle, FUNC_EXIT);
        if (!func_addr) return errorMsg(path);

        return true;
    }

    // 仅加载并测试所有plugin的符号是否存在
    void loadsAll(const std::vector<std::string> &paths) {
        for (auto &&path: paths) {
            auto timestamp = std::chrono::system_clock::now();

            void *handle = libOpen(path);
            if (!handle) {
                JNIEnvs::logger.error("failed to load plugin at " + path);
                continue;
            }

            // test symbol existance
            bool normal = testSymbolExistance(handle, path);
            if (!normal) {
                libClose(handle);
                continue;
            }

            auto timestamp2 = std::chrono::system_clock::now();

            auto timedelta = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp2 - timestamp).count();
            JNIEnvs::logger.info("loaded plugin " + path + " in " + std::to_string(timedelta) + "ms");

            MiraiCPPluginConfig cfg{handle, path};
            std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
            LoaderGlobals::plugin_list[path] = std::move(cfg);
        }
    }

    void loadNewPluginByPath(const std::string &_path, bool activateNow) {
        void *handle = libOpen(_path);
        if (handle != nullptr) {
            JNIEnvs::logger.error("failed to load new plugin: " + _path);
            return;
        }

        if (!testSymbolExistance(handle, _path)) {
            libClose(handle);
            return;
        }

        std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
        LoaderGlobals::plugin_list[_path] = {handle, _path};
    }

    void enableAll() {
        std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
        for (auto &&[k, v]: LoaderGlobals::plugin_list) {
            enable_plugin(v);
        }
    }

    void disableAll() {
        std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
        for (auto &&[k, v]: LoaderGlobals::plugin_list) {
            disable_plugin(v);
        }
    }

    void reloadAllPlugin(jstring _cfgPath) {
        std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
        disableAll();
        LoaderGlobals::plugin_list.clear();

        std::string cfgPath = jstring2str(_cfgPath);
    }

    /// the entrance to load all plugins (not activate)
    void registerAllPlugin(jstring _cfgPath) {
        if (!LoaderGlobals::plugin_list.empty()) {
            JNIEnvs::logger.warning("Plugin is already loaded");
            return;
        }

        std::string cfgPath = jstring2str(_cfgPath);

        nlohmann::json j = readJsonFile(cfgPath);

        auto paths = collect_plugins(cfgPath, std::move(j));

        if (paths.empty()) {
            JNIEnvs::logger.warning("no plugin to load");
            return;
        }

        loadsAll(paths);
    }

    /// activate all loaded plugins
    // todo(antares): logger指针也要传递给插件
    void activateAllPlugins() {
        std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
        auto len = LoaderGlobals::plugin_list.size();
        std::unique_ptr<std::promise<void>[]> promiselist(new std::promise<void>[len]);
        std::unique_ptr<std::future<void>[]> futurelist(new std::future<void>[len]);

        int i = 0;
        for (auto &&[k, v]: LoaderGlobals::plugin_list) {
            auto &pr = promiselist[i];
            auto &fu = futurelist[i];
            fu = pr.get_future();

            auto entrance = (plugin_entrance_func_ptr) libSymbolLookup(v.handle, FUNC_ENTRANCE);

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

    /// interfaces for plugins
    std::vector<std::string> _showAllPluginName() {
        std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
        std::vector<std::string> ans;
        for (auto &&[k, v]: LoaderGlobals::plugin_list) {
            ans.emplace_back(k);
        }
        return ans;
    }

    void _enablePluginByName(const std::string &name) {
        std::lock_guard lk(LoaderGlobals::task_mtx);
        LoaderGlobals::loader_thread_task_queue.push(std::make_pair(LoaderGlobals::ADD_THREAD, name));
    }

    void _disablePluginByName(const std::string &name) {
        std::lock_guard lk(LoaderGlobals::task_mtx);
        LoaderGlobals::loader_thread_task_queue.push(std::make_pair(LoaderGlobals::END_THREAD, name));
    }

    void _enableAllPlugins() {
        std::lock_guard lk(LoaderGlobals::task_mtx);
        LoaderGlobals::loader_thread_task_queue.push(std::make_pair(LoaderGlobals::ENABLE_ALL, ""));
    }

    void _disableAllPlugins() {
        std::lock_guard lk(LoaderGlobals::task_mtx);
        LoaderGlobals::loader_thread_task_queue.push(std::make_pair(LoaderGlobals::DISABLE_ALL, ""));
    }

    void _loadNewPlugin(const std::string &path, bool activateNow) {
        std::lock_guard lk(LoaderGlobals::task_mtx);
        if (activateNow)
            LoaderGlobals::loader_thread_task_queue.push(std::make_pair(LoaderGlobals::LOAD_NEW_ACTIVATENOW, path));
        else
            LoaderGlobals::loader_thread_task_queue.push(std::make_pair(LoaderGlobals::LOAD_NEW_DONTACTIVATE, path));
    }

    JNIEnv *_getEnv() {
        return JNIEnvManager::getEnv();
    }

    ////////////////////////////////////
    void loader_enablePluginByName(const std::string &name) {
        std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
        auto it = LoaderGlobals::plugin_list.find(name);
        if (it == LoaderGlobals::plugin_list.end()) {
            JNIEnvs::logger.error(name + "尚未加载");
            return;
        }
        enable_plugin(it->second);
    }

    void loader_disablePluginByName(const std::string &name) {
        std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
        auto it = LoaderGlobals::plugin_list.find(name);
        if (it == LoaderGlobals::plugin_list.end()) {
            JNIEnvs::logger.error(name + "尚未加载");
            return;
        }
        disable_plugin(it->second);
    }

    void loader_enableAllPlugins() {
        std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
        enableAll();
    }

    void loader_disableAllPlugins() {
        std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
        disableAll();
    }

    void loader_loadNewPlugin(const std::string &path, bool activateNow) {
        std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
        loadNewPluginByPath(path, activateNow);
    }
    ////////////////////////////////////


    void LoaderMain::loaderMain() {
        activateAllPlugins();

        while (!is_loader_exited()) mainloop();

        shutdownLoader();
    }

    void LoaderMain::mainloop() {
        using loadertask = LoaderGlobals::loadertask;

        if (LoaderGlobals::loader_thread_task_queue.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(70));
        else {
            loadertask task;
            {
                std::lock_guard lk(LoaderGlobals::task_mtx);
                task = std::move(LoaderGlobals::loader_thread_task_queue.front());
                LoaderGlobals::loader_thread_task_queue.pop();
            }

            switch (task.first) {
                case LoaderGlobals::ADD_THREAD:
                    loader_enablePluginByName(task.second);
                    break;
                case LoaderGlobals::END_THREAD:
                    loader_disablePluginByName(task.second);
                    break;
                case LoaderGlobals::ENABLE_ALL:
                    loader_enableAllPlugins();
                    break;
                case LoaderGlobals::DISABLE_ALL:
                    loader_disableAllPlugins();
                    break;
                case LoaderGlobals::LOAD_NEW_ACTIVATENOW:
                    loader_loadNewPlugin(task.second, true);
                    break;
                case LoaderGlobals::LOAD_NEW_DONTACTIVATE:
                    loader_loadNewPlugin(task.second, false);
                    break;
                default:
                    throw std::exception();
            }
        }
    }

    void LoaderMain::shutdownLoader() {
        loader_disableAllPlugins();
    }
} // namespace LibLoader
