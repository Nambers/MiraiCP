//
// Created by antares on 5/25/22.
//
#include "loaderTools.h"
#include "JNIEnvs.h"
#include "LoaderLogger.h"
#include <iostream>
#include <json.hpp>
#include <utf8.h>

namespace LibLoader {
    std::vector<std::string> collect_plugins(const std::string &cfgPath, nlohmann::json j) {
        std::vector<std::string> paths;
        try {
            auto &_paths = j["pluginpaths"];
            if (!_paths.is_array()) {
                throw std::exception();
            }
            for (auto &&v: _paths) {
                paths.emplace_back(v);
            }
        } catch (...) {
            std::cerr << "failed to load json: " << cfgPath << std::endl; // todo(antares): change this to logger.error()
            exit(1);
        }
    }

    std::string jstring2str(jstring jStr) {
        if (JNIEnvs::libLoaderEnv == nullptr) {
            std::cerr << "Env pointer not set" << std::endl;
        }
        if (!jStr) {
            return "";
        }
        std::u16string s = reinterpret_cast<const char16_t *>(JNIEnvs::libLoaderEnv->GetStringChars(jStr, nullptr));
        if (s.length() == 0) {
            return "";
        }
        std::string x;
        utf8::utf16to8(s.begin(), s.end(), std::back_inserter(x));
        return x;
    }



    void MiraiCPThreadsController::end_all_thread() {
        // to be implemented
    }
} // namespace LibLoader
