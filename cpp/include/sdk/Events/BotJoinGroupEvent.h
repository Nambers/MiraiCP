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

#ifndef MIRAICP_PRO_BOTJOINGROUPEVENT_H
#define MIRAICP_PRO_BOTJOINGROUPEVENT_H


#include "../Group.h"
#include "../Member.h"
#include "BotEvent.h"


namespace MiraiCP {
    /// 机器人进入某群
    class BotJoinGroupEvent : public BotEvent<BotJoinGroupEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::BotJoinGroupEvent;
        }

    public:
        /// 进入的群
        Group group;
        /// 当type=2时存在，为邀请人，否则为空，调用可能会报错
        std::optional<Member> inviter;
        /// 1-主动加入,2-被邀请加入,3-提供恢复群主身份加入
        int type;

    public:
        explicit BotJoinGroupEvent(BaseEventData j);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_BOTJOINGROUPEVENT_H
