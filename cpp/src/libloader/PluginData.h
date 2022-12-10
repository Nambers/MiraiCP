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

#ifndef MIRAICP_PRO_PLUGINDATA_H
#define MIRAICP_PRO_PLUGINDATA_H


#include "LoaderLogger.h"
#include "commonTypes.h"
#include <shared_mutex>


namespace LibLoader {
    enum PluginAuthority {
        PLUGIN_AUTHORITY_NORMAL = 0,
        PLUGIN_AUTHORITY_ADMIN = 1,
    };

    struct PluginFuncAddrData {
        plugin_entrance_func_ptr entrance = nullptr;
        plugin_event_func_ptr eventFunc = nullptr;
        plugin_func_ptr exit = nullptr;
        plugin_info_func_ptr configFunc = nullptr;

        void _resetAddrData() {
            PluginFuncAddrData tmp;
            std::swap(*this, tmp);
        }
    };

    class PluginData : protected PluginFuncAddrData {
    protected:
        const std::string path;
        plugin_handle handle = nullptr;

        PluginAuthority authority = PLUGIN_AUTHORITY_NORMAL;
        bool enabled = false;

        [[nodiscard]] std::string _getId() const {
            if (!handle) {
                logger.error("致命错误：插件未加载或已经被卸载，请联系MiraiCP开发者并提供您的历史日志");
                return "";
            }
            return configFunc()->getId();
        }

        void _load(plugin_handle _handle, const PluginFuncAddrData &funcAddrs) {
            handle = _handle;
            *static_cast<PluginFuncAddrData *>(this) = funcAddrs;
        }

        void _unload() {
            // cannot modify path
            handle = nullptr;
            _resetAddrData();
        }

        void _enable() {
            enabled = true;
        }

        void _disable() {
            enabled = false;
        }
    };
} // namespace LibLoader
#endif //MIRAICP_PRO_PLUGINDATA_H
