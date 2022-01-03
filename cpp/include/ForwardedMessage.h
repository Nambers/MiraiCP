// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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

#include "MessageChain.h"

#include <json.hpp>
#include <string>


namespace MiraiCP {
    class Contact;
    class ForwardedMessage;
    ///聊天记录里每个消息
    class ForwardedNode {
    public:
        ///发送者id
        QQID id = 0;
        ///发送者昵称
        std::string name;
        ///发送信息
        MessageChain message;
        std::shared_ptr<ForwardedMessage> forwardedMsg;
        ///发送时间(时间戳)
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
        ForwardedNode(Contact *c, MessageChain message, int t);
        ForwardedNode(Contact *c, ForwardedMessage message, int t);
        ForwardedNode(QQID id, const std::string &name, ForwardedMessage message, int t);
    };

    /*!转发消息, 由ForwardNode组成
     * @see class ForwardedNode
     *@example 构建聊天记录(check in version 2.9.0)
     * @code
     * ForwardedMessage(&e.group,
     *                   {ForwardedNode(1930893235, "", MessageChain(PlainText("")), 1),
     *                    ForwardedNode(1930893235, "", MessageChain(e.sender.at(), "aaa"), 1)})
     *          .sendTo(&e.group);
     * @endcode
     */
    class ForwardedMessage {
    private:
        /// json except value
        nlohmann::json sendmsg;

    public:
        std::vector<ForwardedNode> nodes;
        /*!
        *@brief 构建一条聊天记录
        *@details 第一个参数是聊天记录发生的地方, 然后是每条信息
        */
        ForwardedMessage(Contact *c, std::initializer_list<ForwardedNode> nodes);
        ForwardedMessage(Contact *c, std::vector<ForwardedNode> nodes);
        ForwardedNode operator[](int index) { nodes[index]; }
        ForwardedMessage plus(const ForwardedNode &a) {
            ForwardedMessage tmp(*this);
            tmp.nodes.push_back(a);
            return tmp;
        }
        ForwardedMessage operator+(const ForwardedNode &a) { return this->plus(a); }
        void add(const ForwardedNode &a) { this->nodes.push_back(a); }
        /// 发送给群或好友或群成员
        MessageSource sendTo(Contact *c, JNIEnv * = nullptr);
        nlohmann::json nodesToJson();
    };

    /// 接收到的转发消息, 发送用 MiraiCP::ForwardedMessage
    class OnlineForwardedMessage : public SingleMessage {
    public:
        static int type() { return -4; }
        /// 里面每条信息
        std::vector<ForwardedNode> nodelist;
        /// 用展示出来ServiceMessage
        ServiceMessage origin;
        std::string resourceId;

        ForwardedNode operator[](int i) const {
            return this->nodelist[i];
        }

        bool operator==(const OnlineForwardedMessage &m) const;
        /// 转ForwardedMessage
        /// @param c 发生的环境, 比如群聊或者好友
        ForwardedMessage toForwardedMessage(Contact *c);

        explicit OnlineForwardedMessage(nlohmann::json o, const std::string &rid, std::vector<ForwardedNode> nodes) : SingleMessage(OnlineForwardedMessage::type(), ""), nodelist(std::move(nodes)), resourceId(rid), origin(ServiceMessage(o["serviceId"], o["content"])) {}

        /// 不支持直接发送OnlineForwardMessage, ForwardedMessage发送
        [[deprecated("use MiraiCP::ForwardedMessage to send")]] std::string toMiraiCode() const override {
            return "";
        }
        static OnlineForwardedMessage deserializationFromMessageSourceJson(nlohmann::json j);
    };
} // namespace MiraiCP
#endif //MIRAICP_PRO_FORWARDEDMESSAGE_H
