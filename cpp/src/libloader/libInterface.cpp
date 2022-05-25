//
// Created by antares on 5/20/22.
//

#include "libInterface.h"
#include "JNIEnvs.h"
#include "LoaderLogger.h"
#include "commonTypes.h"
#include "interface.h"
#include "json.hpp"
#include "loaderTools.h"
#include <fstream>
#include <iostream>

#define FUNC_ENTRANCE "enrollPlugin"
#define FUNC_EXIT "onDisable"

#if _WIN32 || WIN32

#else
#include <dlfcn.h>
#endif

namespace LibLoader {
    static PluginList plugin_list;
    constexpr static interface_funcs interfaces = collect_interface_functions();

#if _WIN32 || WIN32

#else
    /*
    void enroll_plugin(const MiraiCPPluginConfig &plugin) {
        if (!plugin.handle || !plugin.func) {
            JNIEnvs::logger.error("Plugin" + plugin.path + " is disabled");
            return;
        }
        plugin.func(&plugin_list);
    }
    */
    void disable_plugin(MiraiCPPluginConfig &plugin) {
        if (!plugin.handle) {
            JNIEnvs::logger.warning("plugin " + plugin.path + " is already disabled");
            return;
        }
        auto disable_func = (void_func_ptr) dlsym(plugin.handle, FUNC_EXIT);
        disable_func();
        dlclose(plugin.handle);
        plugin.handle = nullptr;
    }

    void enable_plugin(MiraiCPPluginConfig &plugin) {
        if (plugin.handle) {
            JNIEnvs::logger.warning("plugin " + plugin.path + " is already disabled");
            return;
        }
        plugin.handle = dlopen(plugin.path.c_str(), RTLD_LAZY);
        auto func = (plugin_func_ptr) dlsym(plugin.handle, FUNC_ENTRANCE);
        func(&interfaces);
    }

    bool testSymbolExistance(void *handle, const std::string &path) {
        auto func = dlsym(handle, FUNC_ENTRANCE);
        if (!func) {
            JNIEnvs::logger.error("failed to find symbol in plugin " + path);
            JNIEnvs::logger.error(dlerror());
            return false;
        }
        func = dlsym(handle, FUNC_EXIT);
        if (!func) {
            JNIEnvs::logger.error("failed to find symbol in plugin " + path);
            JNIEnvs::logger.error(dlerror());
            return false;
        }
    }

    void loadsAll(const std::vector<std::string> &paths) {
        for (auto &&path: paths) {
            void *handle = dlopen(path.c_str(), RTLD_LAZY);
            if (!handle) {
                JNIEnvs::logger.error("failed to load plugin at " + path);
                JNIEnvs::logger.error(dlerror());
                continue;
            }

            // test symbol existance
            bool normal = testSymbolExistance(handle, path);
            if (!normal) {
                dlclose(handle);
                continue;
            }

            MiraiCPPluginConfig cfg{handle, path};
            plugin_list[path] = std::move(cfg);
        }
    }


#endif

    void enableAll() {
        for (auto &&[k, v]: plugin_list) {
            enable_plugin(v);
        }
    }

    /// the entrance to load all plugins
    void registerAllPlugin(jstring _cfgPath) {
        if (!plugin_list.empty()) {
            JNIEnvs::logger.warning("Plugin is already loaded");
            return;
        }

        std::string cfgPath = jstring2str(_cfgPath);

        nlohmann::json j;
        {
            std::ifstream i;
            i.open(cfgPath.c_str(), std::ios::in);
            i >> j;
            i.close();
        }

        auto paths = collect_plugins(cfgPath, std::move(j));

        if (paths.empty()) {
            JNIEnvs::logger.warning("no plugin to load");
            return;
        }

        loadsAll(paths);
    }

    std::vector<std::string> _showAllPluginName() {
        std::vector<std::string> ans;
        for (auto &&[k, v]: plugin_list) {
            ans.emplace_back(k);
        }
        return ans;
    }

    void _enablePluginByName(const std::string &name) {
        auto it = plugin_list.find(name);
        if (it == plugin_list.end()) {
            JNIEnvs::logger.error(name + "尚未加载");
            return;
        }
        enable_plugin(it->second);
    }

    void _disablePluginByName(const std::string &name) {
        auto it = plugin_list.find(name);
        if (it == plugin_list.end()) {
            JNIEnvs::logger.error(name + "尚未加载");
            return;
        }
        disable_plugin(it->second);
    }

    void _enableAllPlugins() {
        enableAll();
    }
} // namespace LibLoader

// entrance
jstring activateAllPlugins() {
    return 0; // to be implemented
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    assert(env != nullptr);
    JNIEnvs::gvm = vm;
    // 注册native方法
    if (!registerMethods(env, "tech/eritquearcus/miraicp/shared/CPPLib", method_table, 3)) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}
