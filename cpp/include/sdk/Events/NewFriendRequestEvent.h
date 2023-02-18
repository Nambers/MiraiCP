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

#ifndef MIRAICP_PRO_NEWFRIENDREQUESTEVENT_H
#define MIRAICP_PRO_NEWFRIENDREQUESTEVENT_H


#include "../Friend.h"
#include "../Group.h"
#include "BotEvent.h"


namespace MiraiCP {
    /// 好友申请事件声明
    class NewFriendRequestEvent : public BotEvent<NewFriendRequestEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::NewFriendRequestEvent;
        }

    public:
        /// @brief 序列化的事件信息
        std::string source;
        /// @brief 对方id
        Friend from;
        /// 如果是从群聊申请的则为来源群, 否则为空
        std::optional<Group> fromGroup;
        /// @brief 对方昵称
        std::string nick;
        /// @brief 申请理由
        std::string message;
        /// @brief 事件识别 id
        size_t requestEventId;

        /// @brief 拒绝好友申请
        /// @param ban - 是否加入黑名单
        void reject(bool ban = false);

        /// @brief 接受好友申请
        void accept();

        explicit NewFriendRequestEvent(BaseEventData j);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_NEWFRIENDREQUESTEVENT_H
