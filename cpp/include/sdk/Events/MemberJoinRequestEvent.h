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

#ifndef MIRAICP_PRO_MEMBERJOINREQUESTEVENT_H
#define MIRAICP_PRO_MEMBERJOINREQUESTEVENT_H


#include "../Group.h"
#include "../Member.h"
#include "BotEvent.h"


namespace MiraiCP {
    /// 申请加群事件, bot需为管理员或者群主
    class MemberJoinRequestEvent : public BotEvent<MemberJoinRequestEvent> {
    private:
        std::string source;

    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MemberJoinRequestEvent;
        }

    public:
        /// 申请的群, 如果不存在就表明广播这个事件的时候机器人已经退出该群
        Group group;
        /// 邀请人, 如果不存在表明这个邀请人退出了群或没有邀请人为主动进群
        std::optional<Member> inviter;
        /// 申请人
        Member from;
        /// 申请人昵称
        std::string fromNick;
        /// 申请信息
        std::string message;

    public:
        explicit MemberJoinRequestEvent(BaseEventData j);

        /// 通过
        void accept();

        /// 拒绝
        void reject(std::string msg);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MEMBERJOINREQUESTEVENT_H
