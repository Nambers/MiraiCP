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
#include <string>
#include <utility>
#include <variant>
#include <vector>


namespace MiraiCP {
    class Contact;
    class ForwardedMessage;

    /// 转发信息显示策略, 目前好像只在转发信息内的转发信息生效
    class ForwardedMessageDisplayStrategy {
        using string = std::string;

    public:
        string title = "群聊的聊天记录";
        string brief = "[聊天记录]";
        string source = "聊天记录";
        string summary = "查看1条转发信息";
        std::vector<string> preview{"Name: message"};
        bool operator==(const ForwardedMessageDisplayStrategy &other) const;

    public:
        static ForwardedMessageDisplayStrategy defaultStrategy() { return {}; }

    public:
        ForwardedMessageDisplayStrategy() = default;

        ForwardedMessageDisplayStrategy(std::string title, std::string brief, std::string source, std::string summary, std::vector<std::string> preview);

    public:
        [[nodiscard]] nlohmann::json toJson() const;

        static ForwardedMessageDisplayStrategy fromJson(const nlohmann::json &j);
    };

    ///聊天记录里每个消息
    /// todo 传入头像
    class ForwardedNode {
    public:
        ///发送者id
        QQID id = 0;
        ///发送者昵称
        std::string name;
        ///发送信息
        MessageChain message;
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
        ForwardedNode(QQID id, std::string name, MessageChain message, int time);

        /// @brief 构造聊天记录里每条信息
        /// @param c - 发送者的contact指针
        /// @param message - 发送的信息
        /// @param t - 发送时间，时间戳格式
        ForwardedNode(QQID id, std::string name, ForwardedMessage message, int t);

    public:
        [[nodiscard]] bool isForwarded() const { return isForwardedMessage; }
    };

    /*!转发消息, 由ForwardNode组成
     * @see class ForwardedNode
     * @doxygenEg{1005, forwardMessage.cpp, 构建聊天记录}
     */
    class ForwardedMessage : public SingleMessage {
    private:
        /// 每条信息
        std::vector<ForwardedNode> nodes;
        /// 显示策略
        ForwardedMessageDisplayStrategy display;

    public:
        ForwardedNode &operator[](size_t i) {
            return nodes[i];
        }

        const ForwardedNode &operator[](size_t i) const {
            return nodes[i];
        }

        void add(ForwardedNode a);

        auto &operator+=(ForwardedNode a);

        bool operator==(const ForwardedMessage &m) const;

    public:
        /*!
        *@brief 构建一条聊天记录
        *@details 第一个参数是聊天记录发生的地方, 然后是每条信息
        */
        ForwardedMessage(std::initializer_list<ForwardedNode> nodes, ForwardedMessageDisplayStrategy display = ForwardedMessageDisplayStrategy::defaultStrategy());

        explicit ForwardedMessage(std::vector<ForwardedNode> inNodes, ForwardedMessageDisplayStrategy display = ForwardedMessageDisplayStrategy::defaultStrategy());

        ForwardedMessage(const ForwardedMessage &) = default;
        ForwardedMessage(ForwardedMessage &&) = default;
        [[nodiscard]] nlohmann::json toJson() const override;

    public:
        /// 发送给群或好友或群成员
        MessageSource sendTo(Contact *c) const;

        [[nodiscard]] nlohmann::json nodesToJson() const;

    public:
        static ForwardedMessage deserializationFromMessageJson(const nlohmann::json &j);
    };

    /// 接收到的转发消息, 发送用 MiraiCP::ForwardedMessage
    /// @deprecated Use ForwardedMessage instead
    using OnlineForwardedMessage = ForwardedMessage;
} // namespace MiraiCP
#endif //MIRAICP_PRO_FORWARDEDMESSAGE_H
