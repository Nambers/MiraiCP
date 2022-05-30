//
// Created by antares on 5/25/22.
//


#include "JNIEnvs.h"
#include "JNIEnvManager.h"
#include "LoaderLogger.h"


namespace JNIEnvs {
    // globals
    volatile jclass Class_cpplib = nullptr;

    void setJNIVersion() {
        JNIEnvManager::JNIVersion = getEnv()->GetVersion();
    }

    // functions
    void loadConfigClass() {
        auto env = getEnv();
        Class_cpplib = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("tech/eritquearcus/miraicp/shared/CPPLib")));
    }

    void initializeMiraiCPLoader() {
        loadConfigClass();
        logger.init();
    }

    JNIEnv *getEnv() {
        return JNIEnvManager::getEnv();
    }
} // namespace JNIEnvs
