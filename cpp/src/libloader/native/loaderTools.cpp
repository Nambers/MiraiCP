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
#include "LoaderLogger.h"
#include "commonTools.h"
#include "utf8.h"
// for std::setw
#include <iomanip>
// for ostringstream
#include <sstream>


namespace LibLoader {
    std::pair<std::vector<std::string>, std::vector<PluginAuthority>>
    collect_plugins(const std::string &cfgPath, nlohmann::json j) {
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
                        authority = v["authority"].get<unsigned long long>() ? PLUGIN_AUTHORITY_ADMIN
                                                                             : PLUGIN_AUTHORITY_NORMAL;
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

    void FormatPluginListInfo(const MiraiCP::PluginConfig &plugin_config, int *charNum, std::vector<std::string> &out) {
        CASStrong(charNum[0], plugin_config.id.toString().length() + 1);
        CASStrong(charNum[1], plugin_config.name.toString().length() + 1);
        CASStrong(charNum[2], plugin_config.author.toString().length() + 1);
        CASStrong(charNum[3], plugin_config.description.toString().length() + 1);
        out.emplace_back(plugin_config.id);
        out.emplace_back(plugin_config.name);
        out.emplace_back(plugin_config.author);
        out.emplace_back(plugin_config.description);
        out.emplace_back("\n");
    }

    std::string PluginInfoStream(const std::vector<std::string> &plugin_info, int *charNum) {
        std::ostringstream out;
        out << std::setiosflags(std::ios::left) << "\n";
        int index = 0;
        for (const auto &one_plugin_info: plugin_info) {
            if (index == 0) out << '|';
            if (one_plugin_info != "\n") {
                out << std::setfill(' ') << std::setw(charNum[index]) << one_plugin_info;
                out << '|';
            } else {
                out << "\n";
            }
            index++;
            if (index == 5) {
                index = 0;
                out << std::setw(charNum[0] + charNum[1] + charNum[2] + charNum[3] + 4 + 1) << std::setfill('-') << "";
                out << "\n";
            }
        }
        return out.str();
    }
} // namespace LibLoader