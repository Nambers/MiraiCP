// Copyright (c) 2022. Eritque arcus and contributors.
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

//
// Created by antares on 5/25/22.
//

#ifndef MIRAICP_PRO_COMMONTYPES_H
#define MIRAICP_PRO_COMMONTYPES_H
// don't create cpp for this header


#include "loaderApi.h"
#include <jni.h>
#include <unordered_map>

#define FUNC_ENTRANCE Init
#define FUNC_EVENT Event
#define FUNC_EXIT PluginDisable
#define PLUGIN_INFO MiraiCPPluginInfo

namespace MiraiCP {
    class PluginConfig;
}


namespace LibLoader {
    struct MiraiCPPluginConfig;
    typedef std::unordered_map<std::string, MiraiCPPluginConfig> PluginList;
    typedef void *plugin_handle;
    typedef void (*plugin_entrance_func_ptr)(const LoaderApi::interface_funcs &);
    typedef void (*plugin_func_ptr)();

    enum PluginAuthority {
        PLUGIN_AUTHORITY_NORMAL = 0,
        PLUGIN_AUTHORITY_ADMIN = 1,
    };

    // implement MiraiCPPluginConfig
    struct MiraiCPPluginConfig {
        std::string path;
        plugin_handle handle;
        plugin_func_ptr eventFunc;
        MiraiCP::PluginConfig *config;
        PluginAuthority authority = PLUGIN_AUTHORITY_NORMAL;
        bool enable = true;
    };
} // namespace LibLoader


#endif //MIRAICP_PRO_COMMONTYPES_H
