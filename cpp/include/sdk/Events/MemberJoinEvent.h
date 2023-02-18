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

#ifndef MIRAICP_PRO_MEMBERJOINEVENT_H
#define MIRAICP_PRO_MEMBERJOINEVENT_H


#include "../Group.h"
#include "../Member.h"
#include "BotEvent.h"


namespace MiraiCP {
    /// 新群成员加入
    class MemberJoinEvent : public BotEvent<MemberJoinEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MemberJoinEvent;
        }

    public:
        enum joinType {
            error = 0,
            invited = 1,
            applied = 2,
            rehab = 3
        };

        /*!
        * @brief 事件类型
        *   1 - 被邀请进来
        *   2 - 主动加入
        *   3 - 原群主通过 https://huifu.qq.com/ 恢复原来群主身份并入群
        */
        joinType type = joinType::error;
        ///新进入的成员
        Member member;
        ///目标群
        Group group;
        ///邀请人, 当type = 1时存在，否则则和member变量相同
        std::optional<Member> inviter;

        explicit MemberJoinEvent(BaseEventData j);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MEMBERJOINEVENT_H
