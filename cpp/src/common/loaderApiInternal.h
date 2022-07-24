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

#ifndef MIRAICP_PRO_LOADERAPIINTERNAL_H
#define MIRAICP_PRO_LOADERAPIINTERNAL_H


#include "miraicpString.h"


#ifdef MIRAICP_LIB_LOADER
constexpr int LOADERAPI_H_COUNTER_BASE = __COUNTER__ + 1;
#define LOADERAPI_H_NOTHING(X)
#define LOADERAPI_H_LOADER_API_INNER(X) LOADERAPI_H_NOTHING(X)
#define LOADER_API_COUNT LOADERAPI_H_LOADER_API_INNER(__COUNTER__)
#define LOADERAPI_H_GET_COUNTER (__COUNTER__ - LOADERAPI_H_COUNTER_BASE)
#else
#define LOADER_API_COUNT
#endif


// the API defs to be exposed
namespace LibLoader::LoaderApi {
    using MiraiCP::MiraiCPString;

    LOADER_API_COUNT
    MiraiCPString pluginOperation(const MiraiCPString &);

    LOADER_API_COUNT
    void loggerInterface(const MiraiCPString &content, const MiraiCPString &name, long long id, int level);

    LOADER_API_COUNT
    MiraiCPString showAllPluginId();

    LOADER_API_COUNT
    void enablePluginById(const MiraiCPString &);

    LOADER_API_COUNT
    void disablePluginById(const MiraiCPString &);

    LOADER_API_COUNT
    void enableAllPlugins();

    LOADER_API_COUNT
    void disableAllPlugins();

    LOADER_API_COUNT
    void loadNewPlugin(const MiraiCPString &, bool);

    LOADER_API_COUNT
    void unloadPluginById(const MiraiCPString &);

    LOADER_API_COUNT
    void reloadPluginById(const MiraiCPString &);


    // internal usage. do not call this directly in plugins.
    struct interface_funcs {
        static constexpr int line0 = __LINE__;
        decltype(&pluginOperation) _pluginOperation;
        decltype(&loggerInterface) _loggerInterface;
        decltype(&showAllPluginId) _showAllPluginId;
        static constexpr int line1 = __LINE__;
        // function below can only be called by admin plugins
        static constexpr int adminline0 = __LINE__;
        decltype(&enablePluginById) _enablePluginById = nullptr;
        decltype(&disablePluginById) _disablePluginById = nullptr;
        decltype(&enableAllPlugins) _enableAllPlugins = nullptr;
        decltype(&disableAllPlugins) _disableAllPlugins = nullptr;
        decltype(&loadNewPlugin) _loadNewPlugin = nullptr;
        decltype(&unloadPluginById) _unloadPluginById = nullptr;
        decltype(&reloadPluginById) _reloadPluginById = nullptr;
        static constexpr int adminline1 = __LINE__;
    };


#ifdef MIRAICP_LIB_LOADER
    constexpr inline interface_funcs collect_interface_functions(bool admin) {
        constexpr int counter = LOADERAPI_H_GET_COUNTER;
        static_assert(sizeof(interface_funcs) == sizeof(void *) * counter);
        if (admin) {
            constexpr int line0 = __LINE__;
            interface_funcs t = {
                    pluginOperation,
                    loggerInterface,
                    showAllPluginId,
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
            return t;
        } else {
            constexpr int line0 = __LINE__;
            interface_funcs t2 = {
                    pluginOperation,
                    loggerInterface,
                    showAllPluginId,
            }; // no admin functions
            constexpr int line1 = __LINE__;
            static_assert(line1 - line0 == interface_funcs::line1 - interface_funcs::line0 + 2);
            return t2;
        }
    }
#endif
} // namespace LibLoader::LoaderApi


#endif //MIRAICP_PRO_LOADERAPIINTERNAL_H
