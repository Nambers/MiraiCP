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

#ifndef MIRAICP_PRO_GROUPTEMPMESSAGEEVENT_H
#define MIRAICP_PRO_GROUPTEMPMESSAGEEVENT_H


#include "../Group.h"
#include "../Member.h"
#include "BotEvent.h"


namespace MiraiCP {
    /// 群临时会话
    class GroupTempMessageEvent : public BotEvent<GroupTempMessageEvent>, public IMessageEvent {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::GroupTempMessageEvent;
        }

    public:
        /// 来源群
        Group group;
        /// 发送人
        Member sender;
        /// 信息
        MessageChain message;

    public:
        explicit GroupTempMessageEvent(BaseEventData j);

    public:
        Contact *chat() override {
            return &sender;
        }
        const Contact *chat() const override {
            return &sender;
        }
        Contact *from() override {
            return &sender;
        }
        const Contact *from() const override {
            return &sender;
        }
        MessageChain *getMessageChain() override {
            return &message;
        }
        const MessageChain *getMessageChain() const override {
            return &message;
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_GROUPTEMPMESSAGEEVENT_H
