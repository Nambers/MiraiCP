#ifndef MIRAICP_PRO_FORWARDMESSAGE_H
#define MIRAICP_PRO_FORWARDMESSAGE_H
#include "Contact.h"
namespace MiraiCP {
    ///聊天记录里每个消息
    class ForwardNode {
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
        ForwardNode(QQID id, const std::string &name, MessageChain message,
                    int time)
            : id(id), name(name), message(std::move(message)), time(time) {}

        /// @brief 构造聊天记录里每条信息
        /// @param c - 发送者的contact指针
        /// @param message - 发送的信息
        /// @param t - 发送时间，时间戳格式
        ForwardNode(Contact *c, MessageChain message, int t) : id(c->id()), name(c->nickOrNameCard()),
                                                               message(std::move(message)),
                                                               time(t) {}
    };

    ///转发消息, 由ForwardNode组成
    /// @see class ForwardNode
    class ForwardMessage {
    public:
        /// json节点
        nlohmann::json sendmsg;

        /*!
        *@brief 构建一条聊天记录
        *@details 第一个参数是聊天记录发生的地方
        * 然后是每条信息
        *@example 构建聊天记录
         * @code
        *ForwardMessage(&e.group,
        *{
        *	ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
        *	ForwardNode(1930893235, "Eritque arcus", "hahaha", -1)
        *}).sendTo(&e.group);
        * @endcode
        */
        ForwardMessage(Contact *c, std::initializer_list<ForwardNode> nodes);

        /// 发送给群或好友或群成员
        MessageSource sendTo(Contact *c, JNIEnv * = ThreadManager::getEnv());
    };

    /// 接收到的转发消息, 发送用 MiraiCP::ForwardMessage
    class OnlineForwardMessage : public SingleMessage {
    public:
        std::vector<ForwardNode> nodelist;
        ServiceMessage origin;
        std::string resourceId;

        explicit OnlineForwardMessage(json o, const std::string &rid, std::vector<ForwardNode> nodes) : SingleMessage(-4, ""), nodelist(std::move(nodes)), resourceId(rid), origin(ServiceMessage(o["serviceId"], o["content"])) {}
        [[deprecated("use MiraiCP::ForwardMessage to send")]] std::string toMiraiCode() const override {
            return "";
        }
        static OnlineForwardMessage deserializationFromMessageSourceJson(json j);
    };
} // namespace MiraiCP
#endif //MIRAICP_PRO_FORWARDMESSAGE_H
