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


#include "PluginData.h"
#include "commonTypes.h"
#include <string>


namespace LibLoader {
    class Plugin : public PluginData {
        std::shared_mutex _mtx;

        plugin_handle loadPluginInternal() noexcept;

        void enablePluginInternal();

        void disablePluginInternal();

    public:
        void load_plugin(bool alsoEnablePlugin);

        void enable_plugin();

        void callEntranceFuncAdmin();

        void callEntranceFuncNormal();

        /// 只应被线程池worker调用
        void pushEvent_worker(const MiraiCP::MiraiCPString& event);

        void unload_plugin();

        void disable_plugin();

    public:
        static PluginFuncAddrData testSymbolExistance(plugin_handle handle, const std::string &path);
    };

    plugin_func_ptr get_plugin_disable_ptr(Plugin &plugin);



    /// 激活目前所有存储的插件。在Verify步骤中被kt（主）线程调用一次
    /// 实际的入口，id_plugin_list 必须在这里初始化，该函数只会被调用一次
    void registerAllPlugin(const std::string &cfgPath) noexcept;

    void loadNewPluginByPath(const std::string &_path, bool activateNow);






    void unload_when_exception(Plugin &plugin);
} // namespace LibLoader
#endif //MIRAICP_PRO_PLUGINLISTIMPLEMENTS_H
