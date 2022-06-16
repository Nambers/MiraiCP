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


#include "json.hpp"
#include <jni.h>
#include <string>
#include <vector>


constexpr int LOADERAPI_H_COUNTER_BASE = __COUNTER__ + 1;
#define LOADERAPI_H_NOTHING(X)
#define LOADERAPI_H_LOADER_API_INNER(X) LOADERAPI_H_NOTHING(X)
#define LOADER_API_COUNT LOADERAPI_H_LOADER_API_INNER(__COUNTER__)
#define LOADERAPI_H_GET_COUNTER (__COUNTER__ - LOADERAPI_H_COUNTER_BASE)


// the API defs to be exposed
namespace LibLoader::LoaderApi {
    LOADER_API_COUNT
    JNIEnv *getEnv();

    LOADER_API_COUNT
    std::string pluginOperation(const std::string &);

    LOADER_API_COUNT
    void loggerInterface(const std::string &, std::string, int, int);

    LOADER_API_COUNT
    std::vector<std::string> showAllPluginName();

    LOADER_API_COUNT
    void enablePluginById(const std::string &);

    LOADER_API_COUNT
    void disablePluginById(const std::string &);

    LOADER_API_COUNT
    void enableAllPlugins();

    LOADER_API_COUNT
    void disableAllPlugins();

    LOADER_API_COUNT
    void loadNewPlugin(const std::string &, bool);

    LOADER_API_COUNT
    void unloadPluginById(const std::string &);

    LOADER_API_COUNT
    void reloadPluginById(const std::string &);


    struct interface_funcs {
        decltype(&getEnv) _getEnv;
        decltype(&pluginOperation) _pluginOperation;
        decltype(&loggerInterface) _loggerInterface;
        decltype(&showAllPluginName) _showAllPluginId;
        decltype(&enablePluginById) _enablePluginById = nullptr;
        decltype(&disablePluginById) _disablePluginById = nullptr;
        decltype(&enableAllPlugins) _enableAllPlugins = nullptr;
        decltype(&disableAllPlugins) _disableAllPlugins = nullptr;
        decltype(&loadNewPlugin) _loadNewPlugin = nullptr;
        decltype(&unloadPluginById) _unloadPluginById = nullptr;
        decltype(&reloadPluginById) _reloadPluginById = nullptr;
    };

    /// DON'T CALL THIS in MiraiCP plugins!!!
    constexpr inline interface_funcs collect_interface_functions(bool admin) {
        constexpr int counter = LOADERAPI_H_GET_COUNTER;
        static_assert(sizeof(interface_funcs) == sizeof(void *) * counter);

        constexpr int line0 = __LINE__;
        interface_funcs t = {
                getEnv,
                pluginOperation,
                loggerInterface,
                showAllPluginName,
                enablePluginById,
                disablePluginById,
                enableAllPlugins,
                disableAllPlugins,
                loadNewPlugin,
                unloadPluginById,
                reloadPluginById,
        };
        constexpr int line1 = __LINE__;

        static_assert(line1 - line0 == counter + 3);

        if (admin) return t;

        interface_funcs t2 = {
                getEnv,
                pluginOperation,
                loggerInterface,
                showAllPluginName,
        };
        return t2;
    }
} // namespace LibLoader::LoaderApi


#endif //MIRAICP_PRO_LOADERAPI_H
