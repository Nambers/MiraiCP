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

#ifndef MIRAICP_PRO_MEMBERRECALLEVENT_H
#define MIRAICP_PRO_MEMBERRECALLEVENT_H


#include "../Member.h"
#include "BotEvent.h"


namespace MiraiCP {
    class MemberRecallEvent : public BotEvent<MemberRecallEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MemberRecallEvent;
        }

    public:
        /// 时间戳
        int time = 0;
        /// 信息发生着
        Member author;
        /// 信息撤回者
        Member operater;
        /// 信息id
        std::string ids;
        /// 信息内部ids
        std::string internalIds;

    public:
        explicit MemberRecallEvent(BaseEventData j);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MEMBERRECALLEVENT_H
