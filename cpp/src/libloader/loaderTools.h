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

#ifndef MIRAICP_PRO_LOADERTOOLS_H
#define MIRAICP_PRO_LOADERTOOLS_H


#include "LoaderPluginConfig.h"
#include <fstream>
#include <string>
#include <thread>


namespace LibLoader {
    inline nlohmann::json readJsonFile(const std::string &path) {
        nlohmann::json j;
        {
            std::ifstream i;
            i.open(path.c_str(), std::ios::in);
            i >> j;
            i.close();
        }
        return j;
    }

    template<typename Val1, typename Val2>
    inline void CASStrong(Val1 &a, Val2 b) {
        if (a < b) a = b;
    }

    void FormatPluginListInfo(const MiraiCP::PluginConfig &plugin_config, int *charNum, std::vector<std::string> &out);

    std::string PluginInfoStream(const std::vector<std::string> &plugin_info, int *charNum);

    std::pair<std::vector<std::string>, std::vector<PluginAuthority>> collect_plugins(const std::string &cfgPath, nlohmann::json j);

    std::string jstring2str(jstring jStr);

    jstring str2jstring(const char *cstr);
} // namespace LibLoader

#endif //MIRAICP_PRO_LOADERTOOLS_H
