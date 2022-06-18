// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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

#include <utility>

#include "Logger.h"
#include "PluginConfig.h"

namespace MiraiCP {
    /// 插件父类
    class CPPPlugin {
    public:
        // for api-compatible
        explicit CPPPlugin(PluginConfig c) {
            CPPPlugin::config = std::move(c);
        }
        explicit CPPPlugin() = default;
        virtual ~CPPPlugin() = default;

    public:
        /// @brief 插件信息
        static PluginConfig config;
        /// @brief 插件级logger
        /// @deprecated use Logger::logger instead
        [[deprecated("Use Logger::logger instead")]] static Logger *pluginLogger;

        static std::unique_ptr<CPPPlugin> plugin;

    public:
        /// 插件启用时调用一次
        virtual void onEnable() {}

        virtual void onDisable() {}
    };

} // namespace MiraiCP

#endif //MIRAICP_PRO_CPPPLUGIN_H
