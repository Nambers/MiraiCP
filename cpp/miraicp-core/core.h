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

#ifndef MIRAICP_PRO_CORE_H
#define MIRAICP_PRO_CORE_H
#include <optional>
#include <string>
// todo 是否多余依赖
#include "Bot.h"
namespace MiraiCP::Core {
    using QQID = unsigned long long;
    extern jclass coreClaz;
    extern void *jvmLib;
    /*!
     * @brief 加载MiraiCP核心
     * @param jvmPath jvm.dll 地址
     * @param corePath MiraiCP-core jar 地址
     * @return 如果返回 > 0 就是出现错误, 错误原因到实现里定位
     */
    int loadCore(const char *jvmPath, const std::string &corePath);
    /*!
     * @brief 结束时释放占用的空间
     */
    void exitCore();
    // todo test login
    std::optional<Bot> login(QQID id, std::string_view password, bool md5 = false, std::string_view protocol = "PAD", std::string_view heartBeat = "STAT_HB", JNIEnv * = nullptr);
} // namespace MiraiCP::Core
#endif //MIRAICP_PRO_CORE_H
