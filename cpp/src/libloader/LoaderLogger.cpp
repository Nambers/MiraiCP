//
// Created by antares on 5/25/22.
//

#include "LoaderLogger.h"
#include "JNIEnvs.h"

namespace JNIEnvs {
    LoaderLogger logger;
}

void LoaderLogger::init() {
    log = JNIEnvs::libLoaderEnv->GetStaticMethodID(JNIEnvs::Class_cpplib, "KSendLog", "(Ljava/lang/String;I)V");
}

void LoaderLogger::info(string t) {
    // to be implemented
}

void LoaderLogger::warning(string t) {
    // to be implemented
}

void LoaderLogger::error(string t) {
    // to be implemented
}

void LoaderLogger::call_logger(string) {
    // to be implemented
}
