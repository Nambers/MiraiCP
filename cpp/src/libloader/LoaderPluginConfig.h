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

#ifndef MIRAICP_PRO_LOADERPLUGINCONFIG_H
#define MIRAICP_PRO_LOADERPLUGINCONFIG_H


#include "commonTypes.h"
#include <string>

namespace LibLoader {
    enum PluginAuthority {
        PLUGIN_AUTHORITY_NORMAL = 0,
        PLUGIN_AUTHORITY_ADMIN = 1,
    };

    struct LoaderPluginConfig {
        const std::string path;
        plugin_handle handle;
        plugin_func_ptr eventFunc;
        const MiraiCP::PluginConfig *config;
        PluginAuthority authority = PLUGIN_AUTHORITY_NORMAL;
        bool enabled = false;

        const std::string &getId() const {
            return config->id;
        }

        void reset() {
            // cannot modify path
            handle = nullptr;
            eventFunc = nullptr;
            config = nullptr;
            // do not modify authority
            enabled = false;
        }

        void enable() {
            enabled = true;
        }

        void disable() {
            enabled = false;
        }
    };
} // namespace LibLoader
#endif //MIRAICP_PRO_LOADERPLUGINCONFIG_H
