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

#include "LoaderLogger.h"
#include "JNIEnvs.h"
#include "json.hpp"
#include "loaderTools.h"
#include <sstream>


namespace LibLoader {
    LibLoader::LoaderLogger logger;

    std::string LibLoader::LoaderLogger::vector2string(const std::vector<std::string> &v) {
        // from https://stackoverflow.com/a/5689061/14646226
        const char *const delim = "\n";
        std::ostringstream imploded;
        std::copy(v.begin(), v.end(),
                  std::ostream_iterator<std::string>(imploded, delim));
        return imploded.str();
    }

    void LoaderLogger::init() {
        logMethod = JNIEnvs::getEnv()->GetStaticMethodID(JNIEnvs::Class_cpplib, "KSendLog", "(Ljava/lang/String;I)V");
    }

    void LoaderLogger::info(const string &msg) {
        call_logger(msg, "", -2, 0);
    }

    void LoaderLogger::warning(const string &msg) {
        call_logger(msg, "", -2, 1);
    }

    void LoaderLogger::error(const string &msg) {
        call_logger(msg, "", -2, 2);
    }

    void LoaderLogger::call_logger(const string &content, string name, int id, int level) {
        nlohmann::json j = {
                {"id", id},
                {"log", content}};
        if (!name.empty()) j["name"] = std::move(name);
        log0(content, level, j, JNIEnvs::getEnv());
    }
    void LoaderLogger::log0(const string &content, int level, nlohmann::json j, JNIEnv *env) {
        env->CallStaticVoidMethod(JNIEnvs::Class_cpplib, logMethod, str2jstring(j.dump().c_str()), (jint) level);
    }
} // namespace LibLoader
