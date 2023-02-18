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

#ifndef MIRAICP_PRO_GROUPINVITEEVENT_H
#define MIRAICP_PRO_GROUPINVITEEVENT_H


#include "../Friend.h"
#include "../Group.h"
#include "BotEvent.h"


namespace MiraiCP {
    /// 群聊邀请事件类声明
    class GroupInviteEvent : public BotEvent<GroupInviteEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::GroupInviteEvent;
        }

    public:
        /// 事件序列化文本
        std::string source;
        /// 发起人昵称
        std::string inviterNick;
        /// 被邀请进的组
        std::string groupName;
        /// 群号
        Group group;
        /// 邀请的好友, 如果在邀请后删除好友则为空
        Friend inviter;
        /// 本次申请 id
        size_t requestEventId = 0;

        void reject();

        void accept();

        explicit GroupInviteEvent(BaseEventData j);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_GROUPINVITEEVENT_H
