//
// Created by antares on 5/25/22.
//

#include "interface.h"
#include "JNIEnvs.h"
#include "json.hpp"
#include <exception>

namespace LibLoader {
    jstring enrollAllPlugin(jstring);
}

jstring Verify(JNIEnv *env, jobject, jstring _cfgPath) {
    using json = nlohmann::json;
    JNIEnvs::libLoaderEnv = env;
    JNIEnvs::JNIVersion = env->GetVersion();
    jstring ans;
    try {
        //初始化日志模块
        JNIEnvs::initializeMiraiCPLoader();

        ans = LibLoader::enrollAllPlugin(_cfgPath);
        // plugin == nullptr 无插件实例加载
        //        if (CPPPlugin::plugin != nullptr) {
        //            CPPPlugin::pluginLogger = new PluginLogger(&Logger::logger);
        //            CPPPlugin::plugin->onEnable();
        //        }
    } catch (std::exception &e) {
    }

    // json j = (CPPPlugin::plugin != nullptr) ? CPPPlugin::plugin->config.serialize() : json::parse("{}");
    // j["MiraiCPversion"] = MiraiCPVersion;
    // Config::pluginId = std::stoi(Tools::jstring2str(id));
    return ans;
}
