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

#ifndef MIRAICP_PRO_EVENT_H
#define MIRAICP_PRO_EVENT_H

#include "Bot.h"
#include "Friend.h"
#include "Group.h"
#include "Logger.h"
#include "Member.h"
#include <utility>


namespace MiraiCP {
    /// Event 工厂
    namespace eventTypes {
//        enum Types {
//            BaseEvent [[maybe_unused]], // 0
//            GroupMessageEvent,          // 1
//            PrivateMessageEvent,        // 2
//            GroupInviteEvent,           // 3
//            NewFriendRequestEvent,      // 4
//            MemberJoinEvent,            // 5
//            MemberLeaveEvent,           // 6
//            RecallEvent,                // 7
//            BotJoinGroupEvent,          // 8
//            GroupTempMessageEvent,      // 9
//            TimeOutEvent,               // 10
//            BotOnlineEvent,             // 11
//            NudgeEvent,                 // 12
//            BotLeaveEvent,              // 13
//            MemberJoinRequestEvent,     // 14
//            MessagePreSendEvent,        // 15
//            MiraiCPExceptionEvent,      // 16
//            Command,                    // 17
//            count,                      // 事件在此位置前定义，此时count为事件种类数
//            error                       // 出现问题时使用此enum
//        };
        enum Types {
            BaseEvent [[maybe_unused]] = -1,
            BotOnlineEvent,
            BotJoinGroupEvent,
            GroupInviteEvent,
            BotLeaveEvent,
            MessageEvent,
            RecallEvent,
            MessagePreSendEvent,
            NudgeEvent,
            NewFriendRequestEvent,
            MemberLeaveEvent,
            MemberJoinEvent,
            MemberJoinRequestEvent,
            TimeOutEvent, // TODO
            MiraiCPExceptionEvent = 16,      // 16 todo 暂时保持
            Command = 17,                    // 17
            count,                      // 事件在此位置前定义，此时count为事件种类数
            error                       // 出现问题时使用此enum
        };
    }

    class BaseEventData {
    public:
        std::shared_ptr<Contact> subject = nullptr;
        std::shared_ptr<Contact> object = nullptr;
        QQID subjectId = -1;
        QQID objectId = -1;
        QQID botId = -1;
        nlohmann::json eventData;

        explicit BaseEventData(nlohmann::json j);
    };

    /// Event抽象父类
    class MiraiCPEvent {
    public:
        MiraiCPEvent() = default;

        virtual ~MiraiCPEvent() = default;

    public:
        static eventTypes::Types get_event_type() { return eventTypes::Types::error; }

        virtual eventTypes::Types getEventType() const = 0;
    };

    /// 所有事件处理timeoutevent都是机器人事件，指都有机器人实例
    template<class T>
    class BotEvent : public MiraiCPEvent {

    public:
        eventTypes::Types getEventType() const override { return T::get_event_type(); }

        /// 该事件接受的机器人
        Bot bot;

        /// 以该机器人的名义发送日志
        /// @see BotLogger
        IdLogger botlogger;

        explicit BotEvent(QQID botid) : bot(botid), botlogger(botid, &Logger::logger) {}
        virtual ~BotEvent() = default;
    };

    /// MessageEvent类型的抽象接口，用于Message类型多态实现
    class IMessageEvent {
    public:
        enum MessageEventType {
            PrivateMessageEvent,
            GroupMessageEvent,
            GroupTempMessageEvent,
            StrangerMessageEvent,
        };

        /// 获取当前聊天，可能是群，私聊，或群临时回话
        virtual Contact *chat() = 0;

        /// 获取当前聊天，可能是群，私聊，或群临时回话
        virtual Contact *from() = 0;

        virtual MessageChain *getMessageChain() = 0;

        virtual const Contact *chat() const = 0;

        virtual const Contact *from() const = 0;

        virtual const MessageChain *getMessageChain() const = 0;

        virtual MessageEventType getMessageEventType() const = 0;
    };

    /*!
     * @brief 群消息事件声明
     * @doxygenEg{1003, group.cpp, 取群聊下一条消息}
     */
    class GroupMessageEvent : public BotEvent<GroupMessageEvent>, public IMessageEvent {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MessageEvent;
        }

    public:
        ///来源群
        Group group;
        ///发送人
        Member sender;
        /// 信息
        MessageChain message;

        //        GroupMessageEvent(QQID botid, Group group, Member sender,
        //                          MessageChain mc) : BotEvent(botid), group(std::move(group)),
        //                                             sender(std::move(sender)), message(std::move(mc)){};

        explicit GroupMessageEvent(nlohmann::json j);
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

        MessageEventType getMessageEventType() const override { return IMessageEvent::GroupMessageEvent; }

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

    /*!
     * @detail 私聊消息事件类声明
     * @doxygenEg{1004, group.cpp, 取好友下一条信息}
     */
    class PrivateMessageEvent : public BotEvent<PrivateMessageEvent>, public IMessageEvent {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MessageEvent;
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
        explicit PrivateMessageEvent(nlohmann::json j);

        /*!
         * @brief 取下一个消息(发送人和接收人和本事件一样)
         * @warning 如果两次发送信息间隔过短可能会漏过信息
         * @param time 超时时间限制, 单位为ms, 超时后抛出TimeOutException
         * @param halt 是否拦截该事件(不被注册的监听器收到处理)
         * @return 消息链
         */
        MessageChain nextMessage(long time = -1, bool halt = true) const;

        MessageEventType getMessageEventType() const override { return IMessageEvent::PrivateMessageEvent; }

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

    /// 群聊邀请事件类声明
    class GroupInviteEvent : public BotEvent<GroupInviteEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::GroupInviteEvent;
        }

    public:
        /// 事件序列化文本
        std::string source;
        /// 发起人昵称
        std::string inviterNick;
        /// 被邀请进的组
        std::string groupName;
        /// 群号
        QQID groupid = 0;
        /// 发起人id
        QQID inviterid = 0;


        static void operation0(const std::string &source, QQID botid, bool accept);

        void reject() {
            GroupInviteEvent::operation0(this->source, this->bot.id(), false);
        }

        void accept() {
            GroupInviteEvent::operation0(this->source, this->bot.id(), true);
        }

        /*!
         * @brief 群邀请事件
         * @param botid 当前botid
         * @param source 序列化后字符串
         * @param inviterNick 邀请人昵称
         * @param inviterid 邀请人id
         * @param groupName 群聊名称
         * @param groupid 群号
         */
        //        GroupInviteEvent(QQID botid, std::string source, std::string inviterNick,
        //                         QQID inviterid, std::string groupName, QQID groupid)
        //            : BotEvent(botid), source(std::move(source)), inviterNick(std::move(inviterNick)), groupName(std::move(groupName)),
        //              groupid(groupid), inviterid(inviterid) {}

        explicit GroupInviteEvent(nlohmann::json j);
    };

    /// 好友申请事件声明
    class NewFriendRequestEvent : public BotEvent<NewFriendRequestEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::NewFriendRequestEvent;
        }

    public:
        /// @brief 序列化的事件信息
        std::string source;
        /// @brief 对方id
        QQID fromid;
        QQID fromgroupid;
        /// @brief 对方昵称
        std::string nick;
        /// @brief 申请理由
        std::string message;

        /// @brief 接受好友申请
        /// @param source 事件序列化信息
        static void operation0(const std::string &source, QQID botid, bool accept, bool ban = false);

        /// @brief 拒绝好友申请
        /// @param ban - 是否加入黑名单
        void reject(bool ban = false) {
            NewFriendRequestEvent::operation0(this->source, this->bot.id(), false, ban);
        }

        /// @brief 接受申请
        void accept() {
            NewFriendRequestEvent::operation0(this->source, this->bot.id(), true);
        }

        /*!
         * @brief 好友申请事件
         * @param botid 对应botid
         * @param source 序列化后信息
         * @param fromid 对方id
         * @param fromgroupid 从哪个群申请的，否则为0
         * @param nick 对方昵称
         * @param message 申请理由
         */
        //        NewFriendRequestEvent(QQID botid,
        //                              std::string source,
        //                              QQID fromid,
        //                              QQID fromgroupid,
        //                              std::string nick,
        //                              std::string message)
        //            : BotEvent(botid),
        //              source(std::move(source)),
        //              fromid(fromid),
        //              fromgroupid(fromgroupid),
        //              nick(std::move(nick)),
        //              message(std::move(message)) {}
        explicit NewFriendRequestEvent(nlohmann::json j);
    };

    /// 新群成员加入
    class MemberJoinEvent : public BotEvent<MemberJoinEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MemberJoinEvent;
        }

    public:
        enum joinType {
            error = 0,
            invited = 1,
            applied = 2,
            rehab = 3
        };

        /*!
        * @brief 事件类型
        *   1 - 被邀请进来
        *   2 - 主动加入
        *   3 - 原群主通过 https://huifu.qq.com/ 恢复原来群主身份并入群
        */
        joinType type = joinType::error;
        ///新进入的成员
        Member member;
        ///目标群
        Group group;
        ///邀请人, 当type = 1时存在，否则则和member变量相同
        QQID inviterid;

        /*!
         * @brief 新群成员入群事件
         * @param botid botid
         * @param type 类别 @see MemberJoinEvent::type
         * @param member 入群群成员
         * @param group 群组
         * @param inviterid 邀请群成员id，如果不存在和member id参数一致
         */
        //        MemberJoinEvent(QQID botid, int type, const Member &member, const Group &group,
        //                        QQID inviterid) : BotEvent(botid), type(joinType(type)), member(member),
        //                                          group(group),
        //                                          inviterid(inviterid) {}
        explicit MemberJoinEvent(nlohmann::json j);
    };

    /// 群成员离开
    class MemberLeaveEvent : public BotEvent<MemberLeaveEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MemberLeaveEvent;
        }

    public:
        /// 退出的成员q号
        QQID memberid;
        /// 目标群
        Group group;
        /// 操作人, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
        QQID operaterid;
        /*!
        * @brief 事件类型
        *           1 - 被踢出
        *           2 - 主动退出
        */
        int type = 0;
        /*!
         * @brief 群成员离开
         * @param botid
         * @param type
         * @param memberid 退出的群成员
         * @param group 群
         * @param operaterid 操作人id, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
         */
        //        MemberLeaveEvent(QQID botid, QQID memberid,
        //                         Group group,
        //                         QQID operaterid, int type) : BotEvent(botid), memberid(memberid),
        //                                                      group(std::move(group)),
        //                                                      operaterid(operaterid), type(type) {}
        explicit MemberLeaveEvent(nlohmann::json j);
    };

    /// 撤回信息
    class RecallEvent : public BotEvent<RecallEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::RecallEvent;
        }

    public:
        /// 为1时是好友私聊中撤回，为2时为群聊内撤回
        int type = 0;
        /// 时间戳
        int time = 0;
        /// 原发送者
        QQID authorid = 0;
        /// 撤回者
        QQID operatorid = 0;
        /// 信息id
        std::string ids;
        //内部ids
        std::string internalids;
        //当type是2的时候存在，否则为0
        QQID groupid = 0;

        /*!
         * @brief 撤回事件
         * @param botid 对应bot
         * @param type 类型
         * @param time 时间
         * @param authorid 发送者id
         * @param operatorid 撤回者id
         * @param ids 消息源ids
         * @param internalids 消息源internalids
         * @param groupid
         */
        //        RecallEvent(QQID botid, int type, int time, QQID authorid,
        //                    QQID operatorid, std::string ids, std::string internalids,
        //                    QQID groupid) : BotEvent(botid), type(type), time(time), authorid(authorid),
        //                                    operatorid(operatorid), ids(std::move(ids)),
        //                                    internalids(std::move(internalids)),
        //                                    groupid(groupid) {}
        explicit RecallEvent(nlohmann::json j);
    };

    /// 机器人进入某群
    class BotJoinGroupEvent : public BotEvent<BotJoinGroupEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::BotJoinGroupEvent;
        }

    public:
        /// 进入的群
        Group group;
        /// 当type=2时存在，为邀请人，否则为空，调用可能会报错
        QQID inviterid;
        /// 1-主动加入,2-被邀请加入,3-提供恢复群主身份加入
        int type;

        /*!
         * @brief bot加入群
         * @param botid 对应bot
         * @param type 类别
         * @param group 加入的群
         * @param inviter 邀请人
         */
        //        BotJoinGroupEvent(QQID botid,
        //                          Group group,
        //                          QQID inviter,
        //                          int type)
        //            : BotEvent(botid),
        //              group(std::move(group)),
        //              inviterid(inviter),
        //              type(type) {}
        explicit BotJoinGroupEvent(nlohmann::json j);
    };

    /// 群临时会话
    class GroupTempMessageEvent : public BotEvent<GroupTempMessageEvent>, public IMessageEvent {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MessageEvent;
        }

    public:
        /// 来源群
        Group group;
        /// 发送人
        Member sender;
        /// 信息
        MessageChain message;

        /*!
         * @brief 群临时会话消息事件
         * @param botid 对应bot
         * @param group 发起的群
         * @param sender 发送消息对象
         * @param message 消息
         * @param messageSource 消息源
         */
        //        GroupTempMessageEvent(QQID botid, Group group, Member sender,
        //                              MessageChain message) : BotEvent(botid),
        //                                                      group(std::move(group)),
        //                                                      sender(std::move(sender)),
        //                                                      message(std::move(message)) {}
        explicit GroupTempMessageEvent(nlohmann::json j);

        MessageEventType getMessageEventType() const override { return IMessageEvent::GroupTempMessageEvent; }

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

    /// 定时任务结束
    class TimeOutEvent : public MiraiCPEvent {
    public:
        /// 事件所附信息
        std::string msg;

    public:
        explicit TimeOutEvent(std::string msg) : msg(std::move(msg)) {}

    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::TimeOutEvent;
        }

        eventTypes::Types getEventType() const override { return this->get_event_type(); }
    };


    /// 机器人上线事件
    class BotOnlineEvent : public BotEvent<BotOnlineEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::BotOnlineEvent;
        }

    public:
        explicit BotOnlineEvent(QQID botid) : BotEvent(botid) {}
    };

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

        //        NudgeEvent(std::shared_ptr<Contact> c, std::shared_ptr<Contact> target, std::shared_ptr<Contact> subject, QQID botid)
        //            : BotEvent(botid),
        //              from(std::move(c)),
        //              target(std::move(target)),
        //              subject(std::move(subject)) {}
        explicit NudgeEvent(nlohmann::json j);
    };

    /// 机器人退群事件
    /// 可能有3种类型, 主动退/被踢/解散
    /// 目前mirai的botLeave事件还不稳定暂时不支持类型
    class BotLeaveEvent : public BotEvent<BotLeaveEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::BotLeaveEvent;
        }

        /// 事件类型
        enum class EventType {
            /// 主动退出
            Active = 0,
            /// 被踢出
            Kick,
            /// 群被解散
            Disband
        };

    public:
        /// 退出的群
        /// @attension 收到这个事件时已经退出该群, 可能取不到相关信息
        QQID groupid;
        EventType type;
        std::optional<QQID> operatorId = std::nullopt;

        //        BotLeaveEvent(QQID ingroupid, QQID botid, int type, QQID operatorId)
        //            : BotEvent(botid),
        //              groupid(ingroupid), type(static_cast<EventType>(type)) {
        //            if (operatorId != -1) {
        //                this->operatorId = operatorId;
        //            }
        //        }

        explicit BotLeaveEvent(nlohmann::json j);
    };

    /// 申请加群事件, bot需为管理员或者群主
    class MemberJoinRequestEvent : public BotEvent<MemberJoinRequestEvent> {
    private:
        std::string source;

    public:
        /**
         * @brief 底层通过MemberJoinRequest
         * @param s 序列化后的文本
         */
        static void operate(std::string_view s,
                            QQID botid,
                            bool sign,
                            const std::string &msg = "");

    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MemberJoinRequestEvent;
        }

    public:
        /// 申请的群, 如果不存在就表明广播这个事件的时候机器人已经退出该群
        Group group;
        /// 邀请人, 如果不存在表明这个邀请人退出了群或没有邀请人为主动进群
        std::optional<Member> inviter;
        /// 申请人id
        QQID requesterId;

    public:
        //        MemberJoinRequestEvent(std::optional<Group> g, std::optional<Member> i, QQID botid, QQID requesterId,
        //                               std::string source)
        //            : BotEvent(botid), source(std::move(source)), group(std::move(g)), inviter(std::move(i)),
        //              requesterId(requesterId){};
        explicit MemberJoinRequestEvent(nlohmann::json j);
        /// 通过
        void accept() {
            operate(this->source, this->bot.id(), true);
        }

        /// 拒绝
        void reject(const std::string &msg) {
            operate(this->source, this->bot.id(), false, msg);
        }
    };

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

        //MessagePreSendEvent(std::shared_ptr<Contact> c, MessageChain mc, QQID botid) : BotEvent(botid), target(std::move(c)), message(std::move(mc)) {}
        explicit MessagePreSendEvent(nlohmann::json j);
    };

    class MiraiCPExceptionBase; // forward declaration

    /// @brief 异常抛出事件
    class MiraiCPExceptionEvent : public MiraiCPEvent {
    private:
        MiraiCPExceptionBase *exceptionPtr;

    public:
        explicit MiraiCPExceptionEvent(MiraiCPExceptionBase *err) {
            exceptionPtr = err;
        }

    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MiraiCPExceptionEvent;
        }

        eventTypes::Types getEventType() const override { return this->get_event_type(); }

        const MiraiCPExceptionBase *getException() const {
            return exceptionPtr;
        }
    };

    /// 事件监听操控, 可用于stop停止监听和resume继续监听
    class NodeHandle {
    private:
        bool _enable;

    public:
        explicit NodeHandle(bool a) : _enable(a) {}
        bool isEnable() const { return _enable; }
        void stop() { _enable = false; }
        void resume() { _enable = true; }
    };

    class MIRAICP_EXPORT Event {
    private: // typedefs
        class eventNode {
        private:
            /// 回调的handle，用于管理
            std::shared_ptr<NodeHandle> _handle;

        public:
            std::function<bool(MiraiCPEvent *)> func;

        public:
            eventNode() : _handle(), func(nullptr) {}

            explicit eventNode(std::function<bool(MiraiCPEvent *)> f) : _handle(new NodeHandle(true)), func(std::move(f)) {}

            eventNode(const eventNode &_o) = default; // for MSVC compatible, or you will get an error
            eventNode(eventNode &&_o) noexcept : _handle(std::move(_o._handle)), func(std::move(_o.func)) {}

        public:
            /// 返回true代表block之后的回调
            bool run(MiraiCPEvent *a) const {
                return _handle->isEnable() && func(a);
            }

            std::shared_ptr<NodeHandle> getHandle() {
                return _handle;
            }
        };

        using priority_level = unsigned char;
        using event_vector = std::vector<eventNode>;
        using eventNodeTable = std::vector<std::map<priority_level, event_vector>>;

    private: // member
        eventNodeTable _all_events_;

    private:
        Event() : _all_events_(int(eventTypes::Types::count)){};

    public: // singleton mode
        static Event processor;

    private:
        template<typename EventClass>
        constexpr static int id() {
            static_assert(std::is_base_of_v<MiraiCPEvent, EventClass>, "只支持广播继承MiraiCPEvent的事件");
            return static_cast<int>(EventClass::get_event_type());
        }

    public:
        static bool noRegistered(int index) {
            return processor._all_events_[index].empty();
        }

        /// 清空全部配置
        static void clear() noexcept {
            for (auto &a: processor._all_events_) a.clear();
        }

        static void incomingEvent(nlohmann::json j, int type);
        /// 广播一个事件, 必须为MiraiCPEvent的派生类
        template<typename EventClass>
        static void broadcast(EventClass &&val) {
            static_assert(std::is_base_of_v<MiraiCPEvent, EventClass>, "只支持广播MiraiCPEvent的派生类");
            MiraiCPEvent *p = &val;
            for (auto &&[k, v]: processor._all_events_[id<EventClass>()]) {
                for (auto &&a: v) {
                    if (a.run(p)) return;
                }
            }
        }

        /**
         * @brief 注册一个事件的回调
         * @param T 事件类型
         * @param callback 要注册的回调函数，忽略返回值
         * @param priority_level 优先级，范围：0-255，越低的优先级越先执行，默认100
         * @doxygenEg{1018, callbackHandle.cpp, NodeHandle使用}
         */
        template<typename EventClass>
        static std::shared_ptr<NodeHandle> registerEvent(std::function<void(EventClass)> callback, priority_level level = 100) {
            static_assert(std::is_base_of_v<MiraiCPEvent, EventClass>, "只支持注册MiraiCPEvent的派生类事件");
            std::function<bool(MiraiCPEvent *)> tmp = [=](MiraiCPEvent *p) {
                callback(*static_cast<EventClass *>(p));
                return false;
            };
            auto t = eventNode(tmp);
            auto ans = t.getHandle();
            // 先获得shared_ptr才可以emplace_back
            processor._all_events_[id<EventClass>()][level].emplace_back(std::move(t));
            return ans;
        }

        /**
         * @brief 注册一个可以阻塞后续回调函数的回调。
         * 回调返回true时，将会忽略所有优先级低于当前回调，以及注册顺序晚于当前回调且优先级等于当前回调的所有其他回调函数
         * @param T 事件类型
         * @param callback 要注册的回调函数，必须返回bool值
         * @param priority_level 优先级，范围：0-255，越低的优先级越先执行，默认100
         * @doxygenEg{1019, callbackHandle.cpp, NodeHandle使用}
         */
        template<typename EventClass>
        static std::shared_ptr<NodeHandle> registerBlockingEvent(std::function<bool(EventClass)> callback, priority_level level = 100) {
            static_assert(std::is_base_of_v<MiraiCPEvent, EventClass>, "只支持注册MiraiCPEvent的派生类事件");
            std::function<bool(MiraiCPEvent *)> tmp = [=](MiraiCPEvent *p) {
                return callback(*dynamic_cast<EventClass *>(p));
            };
            auto t = eventNode(tmp);
            auto ans = t.getHandle();
            // 先获得shared_ptr才可以emplace_back
            processor._all_events_[id<EventClass>()][level].emplace_back(std::move(t));
            return ans;
        }
    };
} // namespace MiraiCP


#endif //MIRAICP_PRO_EVENT_H
