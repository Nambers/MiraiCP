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

#ifndef MIRAICP_PRO_LOADERAPI_H
#define MIRAICP_PRO_LOADERAPI_H

#include "MiraiCPMacros.h"
// -----------------------
#include <string>
#include <vector>


// for plugin usage
namespace MiraiCP::LoaderApi {
    /// @brief 返回所有plugin的id
    MIRAICP_EXPORT std::vector<std::string> showAllPluginId();

    /// @brief 启用一个已经加载的插件，仅限有管理权限的插件使用，否则没有任何效果
    /// @param id 插件id
    MIRAICP_EXPORT void enablePluginById(const std::string &id);

    /// @brief 禁用一个已经启用的插件，仅限有管理权限的插件使用，否则没有任何效果
    /// @param id 插件id
    MIRAICP_EXPORT void disablePluginById(const std::string &);

    /// @brief 启用全部已加载的插件，仅限有管理权限的插件使用，否则没有任何效果
    MIRAICP_EXPORT void enableAllPlugins();

    /// @brief 禁用全部已启用的插件，仅限有管理权限的插件使用，否则没有任何效果
    MIRAICP_EXPORT void disableAllPlugins();

    /// @brief 加载新的插件，仅限有管理权限的插件使用，否则没有任何效果
    /// @param path 插件路径
    /// @param enableNow 是否立即启用
    MIRAICP_EXPORT void loadNewPlugin(const std::string &path, bool enableNow);

    /// @brief 卸载一个插件，仅限有管理权限的插件使用，否则没有任何效果
    /// @param id 插件id
    MIRAICP_EXPORT void unloadPluginById(const std::string &id);

    /// @brief 重载一个插件，仅限有管理权限的插件使用，否则没有任何效果
    /// @param id 插件id
    MIRAICP_EXPORT void reloadPluginById(const std::string &id);
} // namespace MiraiCP::LoaderApi

#endif //MIRAICP_PRO_LOADERAPI_H
