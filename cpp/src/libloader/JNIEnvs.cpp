//
// Created by antares on 5/25/22.
//


#include "JNIEnvs.h"
#include "LoaderLogger.h"
#include "JNIEnvManager.h"


namespace JNIEnvs {
    // globals
    JNIEnv *const volatile libLoaderEnv = nullptr;
    volatile const long JNIVersion = 0;
    volatile jclass Class_cpplib = nullptr;
    JavaVM *volatile gvm = nullptr;

    void setJNIVersion() {
        const_cast<long &>(JNIEnvs::JNIVersion) = getEnv()->GetVersion();
    }

    // functions
    void loadConfigClass() {
        auto env = getEnv();
        Class_cpplib = reinterpret_cast<jclass>(env->NewGlobalRef(
                env->FindClass("tech/eritquearcus/miraicp/shared/CPPLib")));
    }

    void loggerInit() {
        logger.init();
    }

    void initializeMiraiCPLoader() {
        loadConfigClass();
        loggerInit();
    }

    JNIEnv *getEnv() {
        return JNIEnvManager::getEnv();
    }
} // namespace JNIEnvs
