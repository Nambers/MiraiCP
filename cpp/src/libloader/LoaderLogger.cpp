//
// Created by antares on 5/25/22.
//

#include "LoaderLogger.h"
#include "JNIEnvs.h"
#include "json.hpp"
#include "loaderTools.h"

namespace JNIEnvs {
    LoaderLogger logger;
}

void LoaderLogger::init() {
    logMethod = JNIEnvs::getEnv()->GetStaticMethodID(JNIEnvs::Class_cpplib, "KSendLog", "(Ljava/lang/String;I)V");
}

// todo (Antares): to be implemented
void LoaderLogger::info(const string &) {
    // to be implemented
}

void LoaderLogger::warning(const string &) {
    // to be implemented
}

void LoaderLogger::error(const string &) {
    // to be implemented
}

void LoaderLogger::call_logger(string content, string name, int id, int level) {
    nlohmann::json j;
    j["log"] = std::move(content);
    j["id"] = id;
    if (!name.empty()) j["name"] = std::move(name);
    auto s = j.dump();

    JNIEnvs::getEnv()->CallStaticVoidMethod(JNIEnvs::Class_cpplib, logMethod, LibLoader::str2jstring(s.c_str()), static_cast<jint>(level));
}
