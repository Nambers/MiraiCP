//
// Created by antares on 5/25/22.
//

#include "JNIEnvs.h"
#include "LoaderLogger.h"

void JNIEnvs::loadConfigClass() {
    Class_cpplib = reinterpret_cast<jclass>(libLoaderEnv->NewGlobalRef(
            libLoaderEnv->FindClass("tech/eritquearcus/miraicp/shared/CPPLib")));
}

void JNIEnvs::loggerInit() {
    logger.init();
}

void JNIEnvs::initializeMiraiCPLoader() {
    loadConfigClass();
    loggerInit();
}

