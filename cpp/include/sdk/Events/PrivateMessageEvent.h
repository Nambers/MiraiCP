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

#ifndef MIRAICP_PRO_PRIVATEMESSAGEEVENT_H
#define MIRAICP_PRO_PRIVATEMESSAGEEVENT_H


#include "../Friend.h"
#include "BotEvent.h"


namespace MiraiCP {
    /*!
     * @detail 私聊消息事件类声明
     * @doxygenEg{1004, group.cpp, 取好友下一条信息}
     */
    class PrivateMessageEvent : public BotEvent<PrivateMessageEvent>, public IMessageEvent {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::FriendMessageEvent;
        }

    public:
        /// 发起人
        Friend sender;
        /// 信息
        MessageChain message{};

        /*!
         * @brief 构建私聊信息
         * @param botid 对应botid
         * @param sender 发送者
         * @param message 消息
         * @param messageSource 消息源
         */
        explicit PrivateMessageEvent(BaseEventData j);

        /*!
         * @brief 取下一个消息(发送人和接收人和本事件一样)
         * @warning 如果两次发送信息间隔过短可能会漏过信息
         * @param time 超时时间限制, 单位为ms, 超时后抛出TimeOutException
         * @param halt 是否拦截该事件(不被注册的监听器收到处理)
         * @return 消息链
         */
        MessageChain nextMessage(long time = -1, bool halt = true) const;

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

#endif //MIRAICP_PRO_PRIVATEMESSAGEEVENT_H
