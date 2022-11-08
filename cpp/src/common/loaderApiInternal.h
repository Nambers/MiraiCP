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


#include "MiraiCPStringInternal.h"
#include "commonTypes.h"

#ifdef MIRAICP_LIB_LOADER
constexpr size_t LOADERAPI_H_COUNTER_BASE = __COUNTER__ + 1;
#define LOADERAPI_H_NOTHING(X)
#define LOADERAPI_H_LOADER_API_INNER(X) LOADERAPI_H_NOTHING(X)
#define LOADER_API_COUNT LOADERAPI_H_LOADER_API_INNER(__COUNTER__)
#define LOADERAPI_H_GET_COUNTER (__COUNTER__ - LOADERAPI_H_COUNTER_BASE)
#else
#define LOADER_API_COUNT
#endif


// the API defs to be exposed
namespace LibLoader::LoaderApi {
    typedef void (*task_func)();
    typedef void (*task_func_with_id)(size_t);

    using MiraiCP::MiraiCPString;

    LOADER_API_COUNT
    MiraiCPString pluginOperation(const MiraiCPString &);

    LOADER_API_COUNT
    void loggerInterface(const MiraiCPString &content, const MiraiCPString &name, long long id, int level);

    LOADER_API_COUNT
    MiraiCPString showAllPluginId();

    LOADER_API_COUNT
    void pushTask(task_func);

    LOADER_API_COUNT
    void pushTaskWithId(task_func_with_id, size_t);

    // Admin api

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
        static constexpr size_t line0 = __LINE__;
        DECL_API(pluginOperation);
        DECL_API(loggerInterface);
        DECL_API(showAllPluginId);
        DECL_API(pushTask);
        DECL_API(pushTaskWithId);
        static constexpr size_t line1 = __LINE__;
        static constexpr size_t normal_api_count = line1 - line0 - 1;
        // apis below can only be called by admin plugins
        static constexpr size_t adminline0 = __LINE__;
        DECL_API(enablePluginById) = nullptr;
        DECL_API(disablePluginById) = nullptr;
        DECL_API(enableAllPlugins) = nullptr;
        DECL_API(disableAllPlugins) = nullptr;
        DECL_API(loadNewPlugin) = nullptr;
        DECL_API(unloadPluginById) = nullptr;
        DECL_API(reloadPluginById) = nullptr;
        static constexpr size_t adminline1 = __LINE__;
        static constexpr size_t admin_api_count = adminline1 - adminline0 - 1;
    };


#ifdef MIRAICP_LIB_LOADER
    constexpr inline interface_funcs collect_interface_functions(bool admin) {
        constexpr size_t counter = LOADERAPI_H_GET_COUNTER;
        static_assert(sizeof(interface_funcs) == sizeof(void *) * counter);
        if (admin) {
            constexpr size_t line0 = __LINE__;
            interface_funcs t = {
                    pluginOperation,
                    loggerInterface,
                    showAllPluginId,
                    pushTask,
                    pushTaskWithId, /// end normal apis
                    enablePluginById,
                    disablePluginById,
                    enableAllPlugins,
                    disableAllPlugins,
                    loadNewPlugin,
                    unloadPluginById,
                    reloadPluginById,
            };
            constexpr size_t line1 = __LINE__;
            static_assert(line1 - line0 == counter + 3);
            return t;
        } else {
            constexpr size_t line0 = __LINE__;
            interface_funcs t2 = {
                    pluginOperation,
                    loggerInterface,
                    showAllPluginId,
                    pushTask,
                    pushTaskWithId,
            }; // no admin functions
            constexpr size_t line1 = __LINE__;
            static_assert(line1 - line0 == interface_funcs::line1 - interface_funcs::line0 + 2);
            return t2;
        }
    }
#endif
} // namespace LibLoader::LoaderApi


#endif //MIRAICP_PRO_LOADERAPIINTERNAL_H
