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

#ifndef MIRAICP_PRO_PLUGINLISTIMPLEMENTS_H
#define MIRAICP_PRO_PLUGINLISTIMPLEMENTS_H


#include "commonTypes.h"
#include <string>


namespace LibLoader {
    /// 激活目前所有存储的插件。在Verify步骤中被kt（主）线程调用一次
    /// 实际的入口，plugin_list 必须在这里初始化，该函数只会被调用一次
    void registerAllPlugin(jstring _cfgPath);

    /// 激活目前所有存储的插件。被loader线程调用
    /// 该函数在插件被 registerAllPlugin 全部注册完成后，调用一次
    void activateAllPlugins();

    void loadNewPluginByPath(const std::string &_path, bool activateNow);

    void enable_plugin(MiraiCPPluginConfig &plugin);

    void unload_plugin(MiraiCPPluginConfig &plugin);
} // namespace LibLoader
#endif //MIRAICP_PRO_PLUGINLISTIMPLEMENTS_H
