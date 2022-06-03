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

#ifndef MIRAICP_PRO_PLUGINLIST_H
#define MIRAICP_PRO_PLUGINLIST_H


#include "commonTypes.h"
#include <jni.h>


// 字符串宏
#define STRINGIFY(A) __STRINGIFY(A)
#define __STRINGIFY(A) #A
// todo(Antares): 将这部分宏放到公共头文件中
#define FUNC_ENTRANCE enrollPlugin
#define FUNC_EVENT eventHandle
#define FUNC_EXIT onDisable
#define PLUGIN_INFO MiraiCPPluginInfo


namespace LibLoader {
    // todo(Antares): 文件过于复杂，需要重构

    // 暴露的接口，必须开放
    void loader_enablePluginByName(const std::string &name);
    void loader_disablePluginByName(const std::string &name);
    void loader_enableAllPlugins();
    void loader_disableAllPlugins();
    void loader_loadNewPlugin(const std::string &path, bool activateNow);

    // loaderMain调用
    void activateAllPlugins();

    // 插件端调用
    std::vector<std::string> getAllPluginName();

    // todo(Antares): 重构下列接口，可能的话尽量避免调用
    void reloadAllPlugin(jstring _cfgPath);
    void enableAll();
    void disableAll();
} // namespace LibLoader


#endif //MIRAICP_PRO_PLUGINLIST_H
