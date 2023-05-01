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

#ifndef MIRAICP_PRO_UTILS_H
#define MIRAICP_PRO_UTILS_H

#include "CPPPlugin.h"
#include "PluginConfig.h"


namespace MiraiCP {
    /// 注册插件函数, 需要被插件实现, 类似onStart();
    MIRAICP_EXPORT void enrollPlugin();

    /// 用指针绑定插件
    [[deprecated("use enrollPlugin<T> instead")]] inline void enrollPlugin(CPPPlugin *p) {
        CPPPlugin::plugin.reset(p);
    }

    /// @note dev: 为防止用户插件crash造成内存泄漏等问题，改为用安全一些的make_unique
    template<typename T>
    inline void enrollPlugin() {
        static_assert(std::is_base_of_v<CPPPlugin, T>, "Enrolling plugin type T should be inherited from CPPPlugin");
        CPPPlugin::plugin = std::make_unique<T>();
    }
} // namespace MiraiCP

#endif //MIRAICP_PRO_UTILS_H
