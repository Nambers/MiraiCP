//
// Created by antares on 5/25/22.
//

#include "interface.h"
#include "JNIEnvs.h"
#include "json.hpp"
#include <exception>


namespace LibLoader {
    void registerAllPlugin(jstring);
    jstring activateAllPlugins();
}


jstring Verify(JNIEnv *env, jobject, jstring _cfgPath) {
    using json = nlohmann::json;
    JNIEnvs::setLoaderEnv(env);
    JNIEnvs::JNIVersion = env->GetVersion();
    jstring ans = nullptr;
    try {
        //初始化日志模块
        JNIEnvs::initializeMiraiCPLoader();

        LibLoader::registerAllPlugin(_cfgPath);

        ans = LibLoader::activateAllPlugins();
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

int registerMethods(JNIEnv *env, const char *className, const JNINativeMethod *gMethods, int numMethods) {
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        return JNI_FALSE;
    }
    //注册native方法
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}
