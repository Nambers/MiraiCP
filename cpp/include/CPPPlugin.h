// Copyright (c) 2021-2021. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_CPPPLUGIN_H
#define MIRAICP_PRO_CPPPLUGIN_H

#include "Logger.h"
#include "PluginConfig.h"

namespace MiraiCP {
    /// 插件父类
    class CPPPlugin {
    public:
        /// @brief 插件信息
        PluginConfig config;
        /// @brief 插件级logger
        static PluginLogger *pluginLogger;

        static CPPPlugin *plugin;

        virtual void onEnable() {}

        virtual void onDisable() {}

        explicit CPPPlugin(PluginConfig c) : config(std::move(c)) {}
    };

} // namespace MiraiCP

#endif //MIRAICP_PRO_CPPPLUGIN_H
