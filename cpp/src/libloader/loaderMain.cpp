//
// Created by antares on 5/20/22.
//

#include "JNIEnvs.h"
#include "LoaderLogger.h"
#include "ThreadController.h"
#include "commonTypes.h"
#include "json.hpp"
#include "ktInterface.h"
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

    const volatile bool loader_exit = false; // use const_cast to modify this
    std::thread loaderThread;

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
            func(LoaderGlobals::interfaces); // discard return value
        });
    }

    // test symbol existance (not calling them)
    bool testSymbolExistance(void *handle, const std::string &path) {
        // using static keyword; don't capture any data
        static auto errorMsg = [](const std::string &path) {
            JNIEnvs::logger.error("failed to find symbol in plugin " + path);
            JNIEnvs::logger.error(dlerror());
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
            void *handle = libOpen(path);
            if (!handle) {
                JNIEnvs::logger.error("failed to load plugin at " + path);
                JNIEnvs::logger.error(dlerror());
                continue;
            }

            // test symbol existance
            bool normal = testSymbolExistance(handle, path);
            if (!normal) {
                libClose(handle);
                continue;
            }

            MiraiCPPluginConfig cfg{handle, path};
            std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
            LoaderGlobals::plugin_list[path] = std::move(cfg);
        }
    }

    void loadNewPluginByPath(const std::string &_path, bool activateNow) {
        void *handle = libOpen(_path);
        if (handle != nullptr) {
            JNIEnvs::logger.error("failed to load new plugin: " + _path);
            JNIEnvs::logger.error(dlerror());
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
    std::string activateAllPlugins() {
        std::lock_guard lk(LoaderGlobals::pluginlist_mtx);
        auto len = LoaderGlobals::plugin_list.size();
        std::unique_ptr<std::promise<std::string>[]> promiselist(new std::promise<std::string>[len]);
        std::unique_ptr<std::future<std::string>[]> futurelist(new std::future<std::string>[len]);

        int i = 0;
        for (auto &&[k, v]: LoaderGlobals::plugin_list) {
            auto &pr = promiselist[i];
            auto &fu = futurelist[i];
            fu = pr.get_future();

            auto entrance = (plugin_entrance_func_ptr) libSymbolLookup(v.handle, FUNC_ENTRANCE);

            ThreadController::getController().addThread(k, [&]() {
                pr.set_value(entrance(LoaderGlobals::interfaces));
            });
            ++i;
        }

        std::string ans;
        for (i = 0; i < len; ++i) {
            ans += futurelist[i].get();
            ans.push_back('\n');
        }

        return ans;
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

    void loaderMain(std::promise<std::string> _promise) {
        using loadertask = LoaderGlobals::loadertask;
        _promise.set_value(activateAllPlugins());

        while (!loader_exit) {
            if (LoaderGlobals::loader_thread_task_queue.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
    }

    void loaderExit() {
        const_cast<bool &>(loader_exit) = true;
    }
} // namespace LibLoader



