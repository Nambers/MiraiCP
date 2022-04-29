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
#include "MiraiCP.hpp"
#include <jni.h>
#include <string>
namespace MiraiCP::Core {
    using QQID = unsigned long long;
    extern jclass coreClaz;
    extern void *jvmLib;
    /*!
     * @brief 加载MiraiCP核心
     * @param corePath MiraiCP-core jar 地址
     * @return 如果返回 > 0 就是出现错误, 错误原因到实现里定位
     */
    int loadCore(const std::string &corePath);
    /*!
     * @brief 结束时释放占用的空间
     */
    void exitCore();
    /*!
     * @brief 登录一个机器人账号
     * @param id QQID
     * @param password 密码, 可为明文/md5
     * @param md5 密码是否为md5
     * @param protocol 登录协议, PAD/IPAD/MACOS/WATCH/PHONE, 默认为 `PAD`
     * @param heartBeat 心跳协议, STAT_HB/REGISTER/NONE, 默认为 `STAT_HB`
     * @throw BotException 登录出现异常时抛出
     * @return 登录后的bot
     */
    Bot login(QQID id, std::string_view password, bool md5 = false, std::string_view protocol = "PAD", std::string_view heartBeat = "STAT_HB", JNIEnv * = nullptr);
} // namespace MiraiCP::Core
#endif //MIRAICP_PRO_CORE_H
