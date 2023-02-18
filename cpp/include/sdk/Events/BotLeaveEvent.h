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

#ifndef MIRAICP_PRO_BOTLEAVEEVENT_H
#define MIRAICP_PRO_BOTLEAVEEVENT_H


#include "../Group.h"
#include "../Member.h"
#include "BotEvent.h"


namespace MiraiCP {
    /// 机器人退群事件
    /// 可能有3种类型, 主动退/被踢/解散
    /// 目前mirai的botLeave事件还不稳定暂时不支持类型
    class BotLeaveEvent : public BotEvent<BotLeaveEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::BotLeaveEvent;
        }

        /// 事件类型
        enum class EventType {
            /// 主动退出
            Active = 0,
            /// 被踢出
            Kick,
            /// 群被解散
            Disband
        };

    public:
        /// 退出的群
        /// @attension 收到这个事件时已经退出该群, 可能取不到相关信息
        Group group;
        EventType type;
        Member operater;

    public:
        explicit BotLeaveEvent(BaseEventData j);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_BOTLEAVEEVENT_H
