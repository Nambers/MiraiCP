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

#ifndef MIRAICP_PRO_MEMBERLEAVEEVENT_H
#define MIRAICP_PRO_MEMBERLEAVEEVENT_H


#include "../Group.h"
#include "../Member.h"
#include "BotEvent.h"


namespace MiraiCP {
    /// 群成员离开
    class MemberLeaveEvent : public BotEvent<MemberLeaveEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MemberLeaveEvent;
        }

    public:
        /// 退出的成员q号
        Member member;
        /// 目标群
        Group group;
        /// 操作人, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
        std::optional<Member> operater;
        /*!
        * @brief 事件类型
        *           1 - 被踢出
        *           2 - 主动退出
        */
        int type = 0;

    public:
        explicit MemberLeaveEvent(BaseEventData j);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MEMBERLEAVEEVENT_H
