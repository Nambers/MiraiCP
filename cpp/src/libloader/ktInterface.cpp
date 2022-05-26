//
// Created by antares on 5/25/22.
//

#include "ktInterface.h"
#include "JNIEnvs.h"
#include "json.hpp"
#include "loaderTools.h"
#include <exception>

namespace LibLoader {
    void registerAllPlugin(jstring);
    std::string activateAllPlugins();
} // namespace LibLoader

/// 实际初始化函数
/// 1. 设置全局变量
/// 2. 加载全部插件
jstring Verify(JNIEnv *env, jobject, jstring _cfgPath) {
    using json = nlohmann::json;

    JNIEnvs::setLoaderEnv(env);
    JNIEnvs::setJNIVersion();

    std::string ans;
    try {
        //初始化日志模块
        JNIEnvs::initializeMiraiCPLoader();

        // 测试有效的插件
        LibLoader::registerAllPlugin(_cfgPath);

        // 激活插件。多线程加载所有插件，调用入口函数
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
    return LibLoader::str2jstring(ans.c_str());
}

jstring Event(JNIEnv *env, jobject, jstring content) {
    return nullptr; // to be implemented
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
