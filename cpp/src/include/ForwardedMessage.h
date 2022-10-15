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
#include <utility>
#include <variant>


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

    public:
        static std::optional<ForwardedMessageDisplayStrategy> defaultStrategy() { return std::nullopt; }

    public:
        ForwardedMessageDisplayStrategy() = default;

        ForwardedMessageDisplayStrategy(std::string title, std::string brief, std::string source, std::string summary, std::vector<std::string> preview) : title(std::move(title)), brief(std::move(brief)), source(std::move(source)), summary(std::move(summary)), preview(std::move(preview)) {}

    public:
        nlohmann::json toJson() {
            nlohmann::json j;
            j["title"] = title;
            j["brief"] = brief;
            j["source"] = source;
            j["summary"] = summary;
            j["preview"] = preview;
            return j;
        }
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
        std::variant<MessageChain, std::shared_ptr<ForwardedMessage>> message;
        ///发送时间(时间戳)
        int time = 0;

    private:
        bool isForwardedMessage;
        std::optional<ForwardedMessageDisplayStrategy> display = std::nullopt;

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
        ForwardedNode(QQID id, std::string name, ForwardedMessage message, int t, std::optional<ForwardedMessageDisplayStrategy> display = ForwardedMessageDisplayStrategy::defaultStrategy());

        /*
        ForwardedNode(Contact *c, MessageChain message, int t);

        ForwardedNode(QQID id, std::string name, ForwardedMessage &message, int t);
        */
    public:
        bool isForwarded() const { return isForwardedMessage; }
    };

    class BaseForwardedMessage {
    protected:
        /// 每条信息
        std::vector<ForwardedNode> nodes;

    protected:
        explicit BaseForwardedMessage(std::vector<ForwardedNode> inNodes) : nodes(std::move(inNodes)) {}

    public:
        virtual ~BaseForwardedMessage() = default;

        ForwardedNode &operator[](int i) {
            return nodes[i];
        }

        const ForwardedNode &operator[](int i) const {
            return nodes[i];
        }
    };

    /*!转发消息, 由ForwardNode组成
     * @see class ForwardedNode
     * @doxygenEg{1005, forwardMessage.cpp, 构建聊天记录}
     */
    class ForwardedMessage : public BaseForwardedMessage {
    private:
        /// json except value
        nlohmann::json sendmsg;

    public:
        /// 显示策略
        std::optional<ForwardedMessageDisplayStrategy> display = std::nullopt;

        ~ForwardedMessage() override = default;

    public:
        /*!
        *@brief 构建一条聊天记录
        *@details 第一个参数是聊天记录发生的地方, 然后是每条信息
        */
        ForwardedMessage(std::initializer_list<ForwardedNode> nodes, std::optional<ForwardedMessageDisplayStrategy> display = ForwardedMessageDisplayStrategy::defaultStrategy()) : ForwardedMessage(std::vector(nodes), std::move(display)) {}

        ForwardedMessage(std::vector<ForwardedNode> inNodes, std::optional<ForwardedMessageDisplayStrategy> display = ForwardedMessageDisplayStrategy::defaultStrategy())
            : BaseForwardedMessage(std::move(inNodes)), display(std::move(display)) {}

    public:
        void add(const ForwardedNode &a) { this->nodes.push_back(a); }

        /// 发送给群或好友或群成员
        MessageSource sendTo(Contact *c);

        nlohmann::json nodesToJson();

        ForwardedMessage plus(const ForwardedNode &a) {
            ForwardedMessage tmp(*this);
            tmp.nodes.push_back(a);
            return tmp;
        }

    public:
        ForwardedMessage operator+(const ForwardedNode &a) { return this->plus(a); }

    public:
        static ForwardedMessage deserializationFromMessageSourceJson(const nlohmann::json &j);
    };

    /// 接收到的转发消息, 发送用 MiraiCP::ForwardedMessage
    class OnlineForwardedMessage : public SingleMessage, public BaseForwardedMessage {
    public:
        /// 用展示出来ServiceMessage
        ServiceMessage origin;
        // unknown 用途, 有一些情况下没有
        // std::optional<std::string> resourceId;

    public:
        explicit OnlineForwardedMessage(nlohmann::json o, /*std::optional<std::string> rid,*/ std::vector<ForwardedNode> nodes)
            : SingleMessage(OnlineForwardedMessage::type(), ""),
              BaseForwardedMessage(std::move(nodes)),
              /*resourceId(std::move(rid)),*/
              origin(ServiceMessage(o["serviceId"], o["content"])) {}
        ~OnlineForwardedMessage() override = default;

    public:
        /// 转ForwardedMessage
        /// @param c 发生的环境, 比如群聊或者好友
        ForwardedMessage toForwardedMessage(std::optional<ForwardedMessageDisplayStrategy> display = ForwardedMessageDisplayStrategy::defaultStrategy()) const;

        /// 不支持直接发送OnlineForwardMessage, ForwardedMessage发送
        ShouldNotUse("use MiraiCP::ForwardedMessage to send") std::string toMiraiCode() const override {
            return "";
        }

    public:
        bool operator==(const OnlineForwardedMessage &m) const;

    public:
        static int type() { return -4; }

        static OnlineForwardedMessage deserializationFromMessageSourceJson(const nlohmann::json &j);
    };
} // namespace MiraiCP
#endif //MIRAICP_PRO_FORWARDEDMESSAGE_H
