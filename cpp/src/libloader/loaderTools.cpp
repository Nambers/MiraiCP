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

#include "loaderTools.h"
#include "JNIEnvs.h"
#include "LoaderLogger.h"
#include <json.hpp>
#include <utf8.h>


namespace LibLoader {
    std::pair<std::vector<std::string>, std::vector<PluginAuthority>> collect_plugins(const std::string &cfgPath, nlohmann::json j) {
        std::vector<std::string> paths;
        std::vector<PluginAuthority> authorities;
        try {
            auto &_pairs = (j.contains("pluginConfig") ? j["pluginConfig"] : j["cppPaths"]);
            if (!_pairs.is_array()) {
                logger.error("pluginConfig is not an array");
                throw std::exception();
            }
            for (auto &&v: _pairs) {
                // path
                {
                    paths.emplace_back(v["path"].get<std::string>());
                }
                // authority
                {
                    PluginAuthority authority = PLUGIN_AUTHORITY_NORMAL;

                    if (v.contains("authority")) {
                        // change this if you want to support multiple authorities
                        authority = v["authority"].get<unsigned long long>() ? PLUGIN_AUTHORITY_ADMIN : PLUGIN_AUTHORITY_NORMAL;
                    }

                    authorities.emplace_back(authority);
                }
            }
        } catch (...) {
            logger.error("failed to load json: " + cfgPath);
            return {};
        }
        return {std::move(paths), std::move(authorities)};
    }

    std::string jstring2str(jstring jStr) {
        if (!jStr) {
            return "";
        }
        std::u16string s = reinterpret_cast<const char16_t *>(JNIEnvs::getEnv()->GetStringChars(jStr, nullptr));
        if (s.length() == 0) {
            return "";
        }
        std::string x;
        utf8::utf16to8(s.begin(), s.end(), std::back_inserter(x));
        return x;
    }

    jstring str2jstring(const char *cstr) {
        if (!cstr) {
            logger.warning("警告:str2jstring传入空字符串");
        }
        std::string str(cstr);
        std::vector<unsigned short> utf16line;
        utf8::utf8to16(str.begin(), str.end(), std::back_inserter(utf16line));
        auto *c = new jchar[utf16line.size()];
        for (int i = 0; i < utf16line.size(); i++) {
            c[i] = utf16line[i];
        }
        return JNIEnvs::getEnv()->NewString((jchar *) c, (jsize) utf16line.size());
    }
} // namespace LibLoader
