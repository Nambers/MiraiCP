// Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_EVENTBASE_H
#define MIRAICP_PRO_EVENTBASE_H


#include "../SdkType.h"
#include <nlohmann/json.hpp>
#include <memory>
#include <optional>


namespace MiraiCP {
    class Contact;
    class MessageChain;

    /// Event 工厂
    namespace eventTypes {
        enum Types {
            BaseEvent [[maybe_unused]] = -1,
            BotOnlineEvent,             // 0
            BotJoinGroupEvent,          // 1
            GroupInviteEvent,           // 2
            BotLeaveEvent,              // 3
            FriendMessageEvent,         // 4
            GroupMessageEvent,          // 5
            GroupTempMessageEvent,      // 6
            FriendRecallEvent,          // 7
            MemberRecallEvent,          // 8
            MessagePreSendEvent,        // 9
            NudgeEvent,                 // 10
            NewFriendRequestEvent,      // 11
            MemberLeaveEvent,           // 12
            MemberJoinEvent,            // 13
            MemberJoinRequestEvent,     // 14
            TimeOutEvent,               // 15
            MiraiCPExceptionEvent = 16, // 16 todo 暂时保持
            Command = 17,               // 17
            count,                      // 事件在此位置前定义，此时count为事件种类数
            error                       // 出现问题时使用此enum
        };
    }

    class BaseEventData {
    public:
        class BuiltInContact {
        public:
            enum ContactType {
                TypeFriend = 1,     // 1
                TypeGroup,          // 2
                TypeMember,         // 3
                TypeBot,            // 4
                TypeStranger,       // 5
                TypeAnonymousMember // 6
            };
            QQID id{};
            QQID botId{};
            QQID groupId{};
            ContactType type{};

            // test helper
            [[nodiscard]] std::string toString() const;
            explicit BuiltInContact(nlohmann::json in_json);

            std::shared_ptr<Contact> toContactPointer();
        };

        std::optional<BuiltInContact> subject = std::nullopt;
        std::optional<BuiltInContact> object = std::nullopt;
        QQID botId;
        nlohmann::json eventData;

        // test helper
        [[nodiscard]] std::string toString() const;
        explicit BaseEventData(nlohmann::json j);
    };

    /// Event抽象父类
    class MiraiCPEvent {
    public:
        MiraiCPEvent() = default;

        virtual ~MiraiCPEvent() = default;

    public:
        static eventTypes::Types get_event_type() { return eventTypes::Types::error; }

        virtual eventTypes::Types getEventType() const = 0;
    };

    /// MessageEvent类型的抽象接口，用于Message类型多态实现
    class IMessageEvent {
    public:
        /// 获取当前聊天，可能是群，私聊，或群临时会话
        virtual Contact *chat() = 0;

        /// 获取当前聊天的发送人，可能是群成员，私聊的好友，或群临时会话对应的群成员
        virtual Contact *from() = 0;

        virtual MessageChain *getMessageChain() = 0;

        virtual const Contact *chat() const = 0;

        virtual const Contact *from() const = 0;

        virtual const MessageChain *getMessageChain() const = 0;
    };

} // namespace MiraiCP

#endif //MIRAICP_PRO_EVENTBASE_H
