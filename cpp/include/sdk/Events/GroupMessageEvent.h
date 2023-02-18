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

#ifndef MIRAICP_PRO_GROUPMESSAGEEVENT_H
#define MIRAICP_PRO_GROUPMESSAGEEVENT_H


#include "../Group.h"
#include "../Member.h"
#include "BotEvent.h"

namespace MiraiCP {
    /*!
     * @brief 群消息事件声明
     * @doxygenEg{1003, group.cpp, 取群聊下一条消息}
     */
    class GroupMessageEvent : public BotEvent<GroupMessageEvent>, public IMessageEvent {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::GroupMessageEvent;
        }

    public:
        ///来源群
        Group group;
        ///发送人
        Member sender;
        /// 信息
        MessageChain message;

        explicit GroupMessageEvent(BaseEventData j);
        /*!
         * @brief 取群聊下一个消息(群聊与本事件一样)
         * @param time 超时时间限制, 单位为ms, 超时后抛出TimeOutException
         * @param halt 是否拦截该事件(不让这个消息被注册的其他监听器收到处理)
         * @return 消息链
         */
        MessageChain nextMessage(long time = -1, bool halt = true) const;

        /*!
         * @brief 取群聊中同群成员的下一个消息(发送人和群与本事件一样)
         * @param time 超时时间限制, 单位为ms, 超时后抛出TimeOutException
         * @param halt 是否拦截该事件(不让消息被注册的其他监听器收到处理)
         * @return 消息链
         */
        MessageChain senderNextMessage(long time = -1, bool halt = true) const;

    public:
        Contact *chat() override {
            return &group;
        }

        const Contact *chat() const override {
            return &group;
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

#endif //MIRAICP_PRO_GROUPMESSAGEEVENT_H
