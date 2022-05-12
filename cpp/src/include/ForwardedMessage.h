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
#include "MiraiDefs.h"
#include <variant>

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
        std::variant<MessageChain, std::shared_ptr<ForwardedMessage>> message;
        ///发送时间(时间戳)
        int time = 0;

    private:
        bool isForwardedMessage;

    public:
        /// @brief 聊天记录里的每条信息
        /// @param id - 发送者id
        /// @param name - 发送者昵称
        /// @param message - 发送的信息
        /// @param time - 发送时间，以时间戳记
        ForwardedNode(QQID id, std::string name, MessageChain message,
                      int time)
            : id(id), name(std::move(name)), message(std::move(message)), time(time), isForwardedMessage(false) {}

        /// @brief 构造聊天记录里每条信息
        /// @param c - 发送者的contact指针
        /// @param message - 发送的信息
        /// @param t - 发送时间，时间戳格式
        ForwardedNode(QQID id, std::string name, ForwardedMessage message, int t);

        /*
        ForwardedNode(Contact *c, MessageChain message, int t);

        ForwardedNode(QQID id, std::string name, ForwardedMessage &message, int t);
        */
    public:
        bool isForwarded() const { return isForwardedMessage; }
    };

    /*!转发消息, 由ForwardNode组成
     * @see class ForwardedNode
     * @doxygenEg{1005, forwardMessage.cpp, 构建聊天记录}
     */
    class ForwardedMessage {
    private:
        /// json except value
        nlohmann::json sendmsg;

    public:
        std::vector<ForwardedNode> nodes;

    public:
        /*!
        *@brief 构建一条聊天记录
        *@details 第一个参数是聊天记录发生的地方, 然后是每条信息
        */
        ForwardedMessage(Contact *c, std::initializer_list<ForwardedNode> nodes);

        ForwardedMessage(Contact *c, std::vector<ForwardedNode> nodes);

        ForwardedMessage(int chattype, QQID id, QQID groupid, std::vector<ForwardedNode> nodes);

    public:
        void add(const ForwardedNode &a) { this->nodes.push_back(a); }

        /// 发送给群或好友或群成员
        MessageSource sendTo(Contact *c, JNIEnv * = nullptr);

        nlohmann::json nodesToJson();

        ForwardedMessage plus(const ForwardedNode &a) {
            ForwardedMessage tmp(*this);
            tmp.nodes.push_back(a);
            return tmp;
        }

    public:
        ForwardedNode &operator[](int index) { return nodes[index]; }

        const ForwardedNode &operator[](int index) const { return nodes[index]; }

        ForwardedMessage operator+(const ForwardedNode &a) { return this->plus(a); }

    public:
        static ForwardedMessage deserializationFromMessageSourceJson(const nlohmann::json &j);
    };

    /// 接收到的转发消息, 发送用 MiraiCP::ForwardedMessage
    class OnlineForwardedMessage : public SingleMessage {
    public:
        /// 里面每条信息
        std::vector<ForwardedNode> nodelist;
        /// 用展示出来ServiceMessage
        ServiceMessage origin;
        // unknown 用途, 有一些情况下没有
        // std::optional<std::string> resourceId;

    public:
        explicit OnlineForwardedMessage(nlohmann::json o, /*std::optional<std::string> rid,*/ std::vector<ForwardedNode> nodes) : SingleMessage(OnlineForwardedMessage::type(), ""), nodelist(std::move(nodes)), /*resourceId(std::move(rid)),*/ origin(ServiceMessage(o["serviceId"], o["content"])) {}

    public:
        /// 转ForwardedMessage
        /// @param c 发生的环境, 比如群聊或者好友
        ForwardedMessage toForwardedMessage(Contact *c);

        /// 不支持直接发送OnlineForwardMessage, ForwardedMessage发送
        ShouldNotUse("use MiraiCP::ForwardedMessage to send") std::string toMiraiCode() const override {
            return "";
        }

    public:
        ForwardedNode &operator[](int i) {
            return nodelist[i];
        }

        const ForwardedNode &operator[](int i) const {
            return nodelist[i];
        }

        bool operator==(const OnlineForwardedMessage &m) const;

    public:
        static int type() { return -4; }

        static OnlineForwardedMessage deserializationFromMessageSourceJson(const nlohmann::json &j);
    };
} // namespace MiraiCP
#endif //MIRAICP_PRO_FORWARDEDMESSAGE_H
