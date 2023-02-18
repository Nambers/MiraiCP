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

#ifndef MIRAICP_PRO_MESSAGEPRESENDEVENT_H
#define MIRAICP_PRO_MESSAGEPRESENDEVENT_H


#include "BotEvent.h"


namespace MiraiCP {
    class Contact;

    /*! 每条消息发送前的事件, 总是在消息实际上被发送和广播MessagePostSendEvent前广播
     * @see MessagePostSendEvent
     * @warning 在这个事件里小心使用sendMessage, 可能会触发无限递归 preSend -> sendMessage -> preSend -> ...
     * */
    class MessagePreSendEvent : public BotEvent<MessagePreSendEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MessagePreSendEvent;
        }

    public:
        /// 发送目标
        std::shared_ptr<Contact> target;
        /// 消息
        MessageChain message;

        explicit MessagePreSendEvent(BaseEventData j);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MESSAGEPRESENDEVENT_H
