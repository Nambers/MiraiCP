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
     * @brief Member数据声明
     */
    struct MemberData : public GroupRelatedData {
        typedef IContactData Super;

        /// @brief 权限等级
        ///     - OWNER群主 为 2
        ///     - ADMINISTRATOR管理员 为 1
        ///     - MEMBER群成员 为 0
        unsigned int _permission = 0;
        /// 是否是匿名群成员, 如果是匿名群成员一些功能会受限
        bool _anonymous = false;

        explicit MemberData(QQID in_groupid) : GroupRelatedData(in_groupid) {}

        void deserialize(nlohmann::json in_json) override;
        void refreshInfo() override;
    };
    /*!
     * @brief 群成员类声明
     * @doxygenEg{1013, member.cpp, 群成员操作}
     */
    class Member : public Contact, public INudgeSupport, public ContactDataHelper<Member, MemberData> {
        friend class Contact;

    public:
        /// @brief 构建群成员对象，一般为内部调用
        /// @param in_json 内容至少如下：{"id":123, "groupid":456, "botid":789}
        /// @throw IllegalArgumentException
        explicit Member(nlohmann::json in_json);

        /// @brief 构建群成员对象
        /// @param qqid 该成员q号
        /// @param groupid 所在群号
        /// @param botid 机器人id
        explicit Member(QQID qqid, QQID groupid, QQID botid);

        /// @note dev: avoid unintentional implicit conversion to nlohmann::json
        Member(bool) = delete;

        /// 是否是匿名群成员, 如果是匿名群成员一些功能会受限
        DECL_GETTER(anonymous)
        DECL_GETTER(permission)
        INLINE_GETTER(groupid)

    public:
        /// @brief 更改群成员权限
        /// @param admin 如果为true为更改到管理员
        /// @param env
        void modifyAdmin(bool admin);

        /// 发送语音
        MessageSource sendVoice(std::string path) {
            return sendVoiceImpl(std::move(path));
        }

        /*!
         * 禁言当前对象，单位是秒，最少0秒最大30天，如果为0或者为负则unmute
         * @throws BotException, MuteException
        */
        void mute(long long sec) const;

        /*!
         * 使用现代C++时间禁言对象，可以是任意时间，传参时会被自动转换为秒
         * @throws BotException, MuteException
        */
        void mute(std::chrono::seconds time) const {
            mute(time.count());
        }

        /// 取消禁言
        /// @throws BotException, MuteException
        void unMute() const {
            mute(0);
        }

        /*! 踢出这个群成员
        * @param reason - 原因
        */
        void kick(std::string reason);

        /// At一个群成员
        At at() { return At(this->id()); }

        /// 更改群名片
        /// @throw MiraiCP::BotException 如果没权限时
        void changeNameCard(std::string_view newName);

        /*!
         * @brief 发送戳一戳
         * @warning 发送戳一戳的前提是登录该bot的协议是android_phone/ipad, 否则抛出IllegalStateException
         * @throw MiraiCP::BotException, MiraiCP::IllegalStateException
         */
        void sendNudge() override;
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MEMBER_H
