//
// Created by antares on 5/25/22.
//

#include "JNIEnvs.h"
#include "LoaderLogger.h"
namespace JNIEnvs {
    // globals
    JNIEnv *const libLoaderEnv = nullptr;
    const long JNIVersion = 0;
    jclass Class_cpplib = nullptr;
    JavaVM *gvm = nullptr;

    void setLoaderEnv(JNIEnv *env) {
        const_cast<JNIEnv *&>(libLoaderEnv) = env;
    }

    void setJNIVersion() {
        const_cast<long &>(JNIEnvs::JNIVersion) = libLoaderEnv->GetVersion();
    }

    // functions
    void loadConfigClass() {
        Class_cpplib = reinterpret_cast<jclass>(libLoaderEnv->NewGlobalRef(
                libLoaderEnv->FindClass("tech/eritquearcus/miraicp/shared/CPPLib")));
    }

    void loggerInit() {
        logger.init();
    }

    void initializeMiraiCPLoader() {
        loadConfigClass();
        loggerInit();
    }
} // namespace JNIEnvs
