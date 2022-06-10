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

#ifndef MIRAICP_PRO_LOADERAPI_H
#define MIRAICP_PRO_LOADERAPI_H


#include <jni.h>
#include <string>
#include <vector>


// the API defs to be exposed
namespace LibLoader::LoaderApi {
    std::vector<std::string> _showAllPluginName();

    void _enablePluginById(const std::string &);

    void _disablePluginById(const std::string &);

    void _enableAllPlugins();

    void _disableAllPlugins();

    void _loadNewPlugin(const std::string &, bool);

    void _unloadPluginById(const std::string &);

    JNIEnv *_getEnv();

    struct interface_funcs {
        decltype(&_showAllPluginName) showAllPluginId;
        decltype(&_enablePluginById) enablePluginById;
        decltype(&_disablePluginById) disablePluginById;
        decltype(&_enableAllPlugins) enableAllPlugins;
        decltype(&_disableAllPlugins) disableAllPlugins;
        decltype(&_loadNewPlugin) loadNewPlugin;
        decltype(&_unloadPluginById) unloadPluginById;
        decltype(&_getEnv) getEnv;
    };
    // DON'T CALL THIS in MiraiCP plugins!!!
    constexpr inline interface_funcs collect_interface_functions() {
        static_assert(sizeof(interface_funcs) == sizeof(void *) * 8);
        return {
                _showAllPluginName,
                _enablePluginById,
                _disablePluginById,
                _enableAllPlugins,
                _disableAllPlugins,
                _loadNewPlugin,
                _unloadPluginById,
                _getEnv,
        };
    }
} // namespace LibLoader::LoaderApi


#endif //MIRAICP_PRO_LOADERAPI_H
