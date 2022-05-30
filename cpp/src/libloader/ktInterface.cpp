//
// Created by antares on 5/25/22.
//


#include "ktInterface.h"
#include "JNIEnvManager.h"
#include "JNIEnvs.h"
#include "json.hpp"
#include "loaderMain.h"
#include <exception>
#include <future>


namespace LibLoader {
    void registerAllPlugin(jstring);
    std::string activateAllPlugins();
    std::thread loaderThread;
} // namespace LibLoader

/// 实际初始化函数
/// 1. 设置全局变量
/// 2. 开启loader线程并获取插件入口函数的返回值
jobject Verify(JNIEnv *env, jobject, jstring _version, jstring _cfgPath) {
    using json = nlohmann::json;

    JNIEnvs::setJNIVersion();

    std::string ans;
    try {
        //初始化日志模块
        JNIEnvs::initializeMiraiCPLoader();

        // 测试有效的插件
        LibLoader::registerAllPlugin(_cfgPath);

        // 激活插件。创建loader thread。
        // loader thread中创建多线程加载所有插件，调用入口函数
        LibLoader::loaderThread = std::thread(LibLoader::LoaderMain::loaderMain);
    } catch (std::exception &e) {
    }

    return nullptr;
}

jobject Event(JNIEnv *env, jobject, jstring content) {
    return nullptr; // todo (Antares): to be implemented
}

jobject PluginDisable(JNIEnv *env, jobject) {
    LibLoader::LoaderMain::loaderExit();
    LibLoader::loaderThread.join();
    return nullptr;
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

// register
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    assert(env != nullptr);
    JNIEnvManager::gvm = vm;
    // 注册native方法
    if (!registerMethods(env, "tech/eritquearcus/miraicp/shared/CPPLib", method_table, 3)) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}
