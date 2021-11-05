// Copyright (c) 2021-2021. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_FORWARDEDMESSAGE_H
#define MIRAICP_PRO_FORWARDEDMESSAGE_H

#include "Contact.h"

namespace MiraiCP {
    ///聊天记录里每个消息
    class ForwardedNode {
    public:
        ///发送者id
        QQID id = 0;
        ///发送者昵称
        std::string name;
        ///发送信息, TODO(这里是每个节点里面的信息，其实里面的信息就是MessageChain)
        MessageChain message;
        ///发送时间
        int time = 0;

        /// @brief 聊天记录里的每条信息
        /// @param id - 发送者id
        /// @param name - 发送者昵称
        /// @param message - 发送的信息
        /// @param time - 发送时间，以时间戳记
        ForwardedNode(QQID id, const std::string &name, MessageChain message,
                      int time)
            : id(id), name(name), message(std::move(message)), time(time) {}

        /// @brief 构造聊天记录里每条信息
        /// @param c - 发送者的contact指针
        /// @param message - 发送的信息
        /// @param t - 发送时间，时间戳格式
        ForwardedNode(Contact *c, MessageChain message, int t) : id(c->id()), name(c->nickOrNameCard()),
                                                                 message(std::move(message)),
                                                                 time(t) {}
    };

    ///转发消息, 由ForwardNode组成
    /// @see class ForwardedNode
    class ForwardedMessage {
    public:
        /// json节点
        nlohmann::json sendmsg;

        /*!
        *@brief 构建一条聊天记录
        *@details 第一个参数是聊天记录发生的地方
        * 然后是每条信息
        *@example 构建聊天记录
         * @code
        *ForwardedMessage(&e.group,
        *{
        *	ForwardedNode(1930893235, "Eritque arcus", "hahaha", 1),
        *	ForwardedNode(1930893235, "Eritque arcus", "hahaha", -1)
        *}).sendTo(&e.group);
        * @endcode
        */
        ForwardedMessage(Contact *c, std::initializer_list<ForwardedNode> nodes);

        /// 发送给群或好友或群成员
        MessageSource sendTo(Contact *c, JNIEnv * = ThreadManager::getEnv());
    };

    /// 接收到的转发消息, 发送用 MiraiCP::ForwardedMessage
    class OnlineForwardedMessage : public SingleMessage {
    public:
        /// 里面每条信息
        std::vector<ForwardedNode> nodelist;
        /// 用展示出来ServiceMessage
        ServiceMessage origin;
        std::string resourceId;

        explicit OnlineForwardedMessage(json o, const std::string &rid, std::vector<ForwardedNode> nodes) : SingleMessage(-4, ""), nodelist(std::move(nodes)), resourceId(rid), origin(ServiceMessage(o["serviceId"], o["content"])) {}
        /// 不支持直接发送OnlineForwardMessage
        [[deprecated("use MiraiCP::ForwardedMessage to send")]] std::string toMiraiCode() const override {
            std::string re;
            for (const auto &a: nodelist)
                re += a.message.toMiraiCode() + "\n";
            return re;
        }
        static OnlineForwardedMessage deserializationFromMessageSourceJson(json j);
    };
} // namespace MiraiCP
#endif //MIRAICP_PRO_FORWARDEDMESSAGE_H
