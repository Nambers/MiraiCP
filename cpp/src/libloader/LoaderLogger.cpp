// Copyright (c) 2022 - 2022. Antares, Eritque arcus and contributors.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or any later version(in your opinion).
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "MiraiCPMacros.h"
// -----------------------
#include "JNIEnvs.h"
#include "LoaderLogger.h"
#include "commonTools.h"
#include "loaderTools.h"
#include "utf8.h"
#include <json.hpp>

#ifdef LOADER_NATIVE
namespace LoaderAPIs {
    using LogFunc = void (*)(const char *, int);
    extern LogFunc log;
}; // namespace LoaderAPIs
#endif

namespace LibLoader {
    static const std::string LoaderName = "MiraiCP/LibLoader";
    LibLoader::LoaderLogger logger;

    void LoaderLogger::init() {
#ifndef LOADER_NATIVE
        logMethod = JNIEnvs::getEnv()->GetStaticMethodID(JNIEnvs::Class_cpplib, "KSendLog", "(Ljava/lang/String;I)V");
#endif
    }

    void LoaderLogger::info(const string &msg) const {
        call_logger(msg, LoaderName, -1, 0);
    }

    void LoaderLogger::warning(const string &msg) const {
        call_logger(msg, LoaderName, -1, 1);
    }

    void LoaderLogger::error(const string &msg) const {
        call_logger(msg, LoaderName, -1, 2);
    }

    void LoaderLogger::info(const LoaderLogger::wstring &msg) const {
        call_logger(msg, LoaderName, -1, 0);
    }

    void LoaderLogger::warning(const LoaderLogger::wstring &msg) const {
        call_logger(msg, LoaderName, -1, 1);
    }

    void LoaderLogger::error(const LoaderLogger::wstring &msg) const {
        call_logger(msg, LoaderName, -1, 2);
    }

    void LoaderLogger::call_logger(const string &content, string name, long long id, int level) const {
        nlohmann::json j = {
                {"id", id},
                {"log", content}};
        if (!name.empty()) j["name"] = std::move(name);
#ifdef LOADER_NATIVE
        LoaderAPIs::log(j.dump().c_str(), level);
#else
        auto env = JNIEnvs::getEnv();
        env->CallStaticVoidMethod(JNIEnvs::Class_cpplib, logMethod, LibLoader::str2jstring(j.dump().c_str()), (jint) (level));
#endif
    }

    void LoaderLogger::call_logger(const wstring &content, string name, long long id, int level) const {
        std::string rcontent;

#if WCHAR_MAX <= 0xFFFFu
        utf8::utf16to8(content.begin(), content.end(), std::back_inserter(rcontent)); // Windows
#else
        utf8::utf32to8(content.begin(), content.end(), std::back_inserter(rcontent)); // Linux & others
#endif

        nlohmann::json j = {
                {"id", id},
                {"log", rcontent}};
        if (!name.empty()) j["name"] = std::move(name);
#ifdef LOADER_NATIVE
        LoaderAPIs::log(j.dump().c_str(), level);
#else
        auto env = JNIEnvs::getEnv();
        env->CallStaticVoidMethod(JNIEnvs::Class_cpplib, logMethod, LibLoader::str2jstring(j.dump().c_str()), (jint) (level));
#endif
    }
} // namespace LibLoader
