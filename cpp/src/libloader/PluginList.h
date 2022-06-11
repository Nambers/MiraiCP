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
#ifndef __STRINGIFY
#define __STRINGIFY(A) #A
#endif


namespace LibLoader {
    // todo(Antares): 文件过于复杂，需要重构

    // 暴露的接口，必须开放
    void loader_enablePluginById(const std::string &);
    void loader_disablePluginById(const std::string &);
    void loader_enableAllPlugins();
    void loader_disableAllPlugins();
    void loader_loadNewPlugin(const std::string &path, bool activateNow);
    void loader_unloadPluginById(const std::string &);

    // loaderMain调用
    void activateAllPlugins();

    // 插件端调用
    std::vector<std::string> getAllPluginName();

    // todo(Antares): 重构下列接口，可能的话尽量避免调用
    void reloadAllPlugin(jstring _cfgPath);
    void enableAll();
    void disableAll();
    void unloadAll();
} // namespace LibLoader


#endif //MIRAICP_PRO_PLUGINLIST_H
