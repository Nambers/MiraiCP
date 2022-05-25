//
// Created by antares on 5/20/22.
//

#include "JNIEnvs.h"
#include "interface.h"
#include "json.hpp"
#include "loaderTools.h"
#include <fstream>
#include <iostream>


#define FUNC_ENTRANCE "enrollPlugin"


#if _WIN32 || WIN32

#else
#include <dlfcn.h>
#endif

namespace LibLoader {
#if _WIN32 || WIN32

#else
    struct MiraiCPPluginConfig;
    typedef std::vector<MiraiCPPluginConfig> PluginList;

    typedef void (*void_funct_ptr)(PluginList *);
    typedef void *plugin_handle;
    struct MiraiCPPluginConfig {
        plugin_handle handle;
        void_funct_ptr func;
        std::string path;
    };

    static PluginList plugin_list;

    void enroll_plugin(const MiraiCPPluginConfig &plugin) {
        if (!plugin.handle || !plugin.func) {
            std::cerr << "Plugin is disabled" << std::endl; // todo(antares): change this to logger.warning()
            return;
        }
        plugin.func(&plugin_list);
    }

    void disable_plugin(MiraiCPPluginConfig &plugin) {
        if (!plugin.handle) {
            std::cerr << "Plugin is already disabled" << std::endl; // todo(antares): change this to logger.warning()
            return;
        }
        dlclose(plugin.handle);
        plugin.handle = nullptr;
        plugin.func = nullptr;
    }

    void enable_plugin(MiraiCPPluginConfig &plugin) {
        if (plugin.handle) {
            std::cerr << "Plugin is already enabled" << std::endl; // todo(antares): change this to logger.warning()
            return;
        }
        plugin.handle = dlopen(plugin.path.c_str(), RTLD_LAZY);
        plugin.func = (void_funct_ptr) dlsym(plugin.handle, FUNC_ENTRANCE);
    }

    /// the real entrance to load all plugins
    void enrollAllPlugin(jstring _cfgPath) {

        if (!plugin_list.empty()) {
            std::cerr << "Plugin is already loaded" << std::endl;
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

        std::vector<std::string> paths;
        try {
            auto &_paths = j["pluginpaths"];
            if (!_paths.is_array()) {
                throw std::exception();
            }
            for (auto &&v: _paths) {
                paths.emplace_back(v);
            }
        } catch (...) {
            std::cerr << "failed to load json: " << cfgPath << std::endl; // todo(antares): change this to logger.error()
            exit(1);
        }

        if (paths.empty()) {
            std::cerr << "no plugin to load" << std::endl; // todo(antares): change this to logger.warning()
        }

        for (auto &&path: paths) {
            void *handle = dlopen(path.c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "failed to load plugin at " << path << std::endl; // todo(antares): change this to logger.error()
                exit(2);
            }

            auto func = (void_funct_ptr) dlsym(handle, FUNC_ENTRANCE);
            if (!func) {
                std::cerr << "failed to find symbol in plugin " << path << std::endl; // todo(antares): change this to logger.error()
                exit(3);
            }

            MiraiCPPluginConfig cfg{handle, func, path};
            plugin_list.emplace_back(std::move(cfg));
        }
    }
#endif
} // namespace LibLoader

// entrance
int loaderMain() {
    return 0;
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
