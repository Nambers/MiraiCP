//
// Created by antares on 5/25/22.
//

#include "JNIEnvs.h"
#include "LoaderLogger.h"
namespace JNIEnvs {
    // globals
    JNIEnv *libLoaderEnv = nullptr;
    long JNIVersion;
    jclass Class_cpplib;
    JavaVM *gvm;

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
