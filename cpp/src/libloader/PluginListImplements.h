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


#include "LoaderPluginConfig.h"
#include "commonTypes.h"
#include <string>


namespace LibLoader {
    void callEntranceFuncAdmin(plugin_entrance_func_ptr func);
    void callEntranceFuncNormal(plugin_entrance_func_ptr func);
    /// 激活目前所有存储的插件。在Verify步骤中被kt（主）线程调用一次
    /// 实际的入口，id_plugin_list 必须在这里初始化，该函数只会被调用一次
    void registerAllPlugin(jstring _cfgPath);

    void loadNewPluginByPath(const std::string &_path, bool activateNow);

    void enable_plugin(LoaderPluginConfig &plugin);

    void disable_plugin(LoaderPluginConfig &plugin);

    void load_plugin(LoaderPluginConfig &plugin, bool alsoEnablePlugin);

    void unload_plugin(LoaderPluginConfig &plugin);
} // namespace LibLoader
#endif //MIRAICP_PRO_PLUGINLISTIMPLEMENTS_H
