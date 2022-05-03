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

#ifndef MIRAICP_PRO_MEMBER_H
#define MIRAICP_PRO_MEMBER_H

#include "Contact.h"

namespace MiraiCP {
    /*!
     * @brief 群成员类声明
     * @doxygenEg{1013, member.cpp, 群成员操作}
     */
    class Member : public Contact, INudgeSupport {
    public:
        /// @brief 权限等级
        ///     - OWNER群主 为 2
        ///     - ADMINISTRATOR管理员 为 1
        ///     - MEMBER群成员 为 0
        /// @note 上面那些变量在constants.h中有定义
        unsigned int permission = 0;

        /// @brief 更改群成员权限
        /// @param admin 如果为true为更改到管理员
        /// @param env
        void modifyAdmin(bool admin, JNIEnv *env = nullptr);

        /// @brief 构建群成员对象
        /// @param qqid 该成员q号
        /// @param groupid 所在群号
        /// @param botid 机器人id
        explicit Member(QQID qqid, QQID groupid, QQID botid,
                        JNIEnv * = nullptr);

        explicit Member(const Contact &c) : Contact(c) {
            if (c.type() != 3)
                throw IllegalArgumentException("无法从 type==" + std::to_string(c.type()) + " 转为 type == 3(member)", MIRAICP_EXCEPTION_WHERE);
            this->isAnonymous = this->_anonymous;
            refreshInfo();
        };

        /// 是否是匿名群成员, 如果是匿名群成员一些功能会受限
        bool isAnonymous = false;

        /// 重新获取(刷新)群成员信息
        void refreshInfo(JNIEnv *env = nullptr);

        /// 发送语音
        MessageSource sendVoice(const std::string &path, JNIEnv *env = nullptr) {
            return Contact::sendVoice0(path, env);
        }

        /// 获取权限，会在构造时调用，请使用permission缓存变量
        /// @see Member::permission
        unsigned int getPermission(JNIEnv * = nullptr) const;

        /*!
         * 禁言当前对象，单位是秒，最少0秒最大30天，如果为0或者为负则unmute
         * @throws BotException, MuteException
        */
        void mute(int time, JNIEnv * = nullptr);

        /// 取消禁言
        /// @throws BotException, MuteException
        void unMute(JNIEnv *env = nullptr) {
            mute(0, env);
        }

        /*! 踢出这个群成员
        * @param reason - 原因
        */
        void kick(const std::string &reason, JNIEnv * = nullptr);

        /// At一个群成员
        At at() {
            return At(this->id());
        }

        /// 更改群名片
        /// @throw MiraiCP::BotException 如果没权限时
        void changeNameCard(std::string_view newName, JNIEnv* = nullptr);

        /*!
         * @brief 发送戳一戳
         * @warning 发送戳一戳的前提是登录该bot的协议是android_phone/ipad, 否则抛出IllegalStateException
         * @throw MiraiCP::BotException, MiraiCP::IllegalStateException
         */
        void sendNudge() override;
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MEMBER_H
