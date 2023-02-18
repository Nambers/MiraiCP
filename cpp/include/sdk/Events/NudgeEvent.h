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

#ifndef MIRAICP_PRO_NUDGEEVENT_H
#define MIRAICP_PRO_NUDGEEVENT_H


#include "BotEvent.h"


namespace MiraiCP {
    class Contact;

    /*! 戳一戳事件
     * @warning nudgeEvent事件也会被bot自己发的Nudge触发, 可能会造成无限循环
     */
    class NudgeEvent : public BotEvent<NudgeEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::NudgeEvent;
        }

    public:
        ///发送人
        std::shared_ptr<Contact> from;
        /// 目标
        std::shared_ptr<Contact> target;
        /// 发送的环境, 可能为Group / Friend
        std::shared_ptr<Contact> subject;

    public:
        explicit NudgeEvent(BaseEventData j);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_NUDGEEVENT_H
