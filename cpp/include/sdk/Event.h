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

#ifndef MIRAICP_PRO_EVENT_H
#define MIRAICP_PRO_EVENT_H

#include "Bot.h"
#include "Friend.h"
#include "Group.h"
#include "Logger.h"
#include "Member.h"
#include <json.hpp>
#include <mutex>
#include <utility>


namespace MiraiCP {
    /// Event 工厂
    namespace eventTypes {
        enum Types {
            BaseEvent [[maybe_unused]] = -1,
            BotOnlineEvent,             // 0
            BotJoinGroupEvent,          // 1
            GroupInviteEvent,           // 2
            BotLeaveEvent,              // 3
            FriendMessageEvent,         // 4
            GroupMessageEvent,          // 5
            GroupTempMessageEvent,      // 6
            FriendRecallEvent,          // 7
            MemberRecallEvent,          // 8
            MessagePreSendEvent,        // 9
            NudgeEvent,                 // 10
            NewFriendRequestEvent,      // 11
            MemberLeaveEvent,           // 12
            MemberJoinEvent,            // 13
            MemberJoinRequestEvent,     // 14
            TimeOutEvent,               // 15
            MiraiCPExceptionEvent = 16, // 16 todo 暂时保持
            Command = 17,               // 17
            count,                      // 事件在此位置前定义，此时count为事件种类数
            error                       // 出现问题时使用此enum
        };
    }

    class BaseEventData {
    public:
        class BuiltInContact {
        public:
            enum ContactType {
                TypeFriend = 1,     // 1
                TypeGroup,          // 2
                TypeMember,         // 3
                TypeBot,            // 4
                TypeStranger,       // 5
                TypeAnonymousMember // 6
            };
            QQID id{};
            QQID botId{};
            QQID groupId{};
            ContactType type{};

            explicit BuiltInContact(nlohmann::json in_json);

            std::shared_ptr<Contact> toContactPointer();
        };

        std::optional<BuiltInContact> subject = std::nullopt;
        std::optional<BuiltInContact> object = std::nullopt;
        QQID botId;
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
        /// 获取当前聊天，可能是群，私聊，或群临时会话
        virtual Contact *chat() = 0;

        /// 获取当前聊天的发送人，可能是群成员，私聊的好友，或群临时会话对应的群成员
        virtual Contact *from() = 0;

        virtual MessageChain *getMessageChain() = 0;

        virtual const Contact *chat() const = 0;

        virtual const Contact *from() const = 0;

        virtual const MessageChain *getMessageChain() const = 0;
    };

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
        Group group;
        /// 邀请的好友, 如果在邀请后删除好友则为空
        Friend inviter;
        /// 本次申请 id
        size_t requestEventId = 0;

        void reject();

        void accept();

        explicit GroupInviteEvent(BaseEventData j);
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
        Friend from;
        /// 如果是从群聊申请的则为来源群, 否则为空
        std::optional<Group> fromGroup;
        /// @brief 对方昵称
        std::string nick;
        /// @brief 申请理由
        std::string message;
        /// @brief 事件识别 id
        size_t requestEventId;

        /// @brief 拒绝好友申请
        /// @param ban - 是否加入黑名单
        void reject(bool ban = false);

        /// @brief 接受好友申请
        void accept();

        explicit NewFriendRequestEvent(BaseEventData j);
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
        std::optional<Member> inviter;

        explicit MemberJoinEvent(BaseEventData j);
    };

    /// 群成员离开
    class MemberLeaveEvent : public BotEvent<MemberLeaveEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MemberLeaveEvent;
        }

    public:
        /// 退出的成员q号
        Member member;
        /// 目标群
        Group group;
        /// 操作人, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
        std::optional<Member> operater;
        /*!
        * @brief 事件类型
        *           1 - 被踢出
        *           2 - 主动退出
        */
        int type = 0;

    public:
        explicit MemberLeaveEvent(BaseEventData j);
    };

    class FriendRecallEvent : public BotEvent<FriendRecallEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::FriendRecallEvent;
        }

    public:
        /// 时间戳
        int time = 0;
        /// 信息发生着
        Friend author;
        /// 信息撤回者
        Friend operater;
        /// 信息id
        std::string ids;
        /// 信息内部ids
        std::string internalIds;

    public:
        explicit FriendRecallEvent(BaseEventData j);
    };

    class MemberRecallEvent : public BotEvent<MemberRecallEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MemberRecallEvent;
        }

    public:
        /// 时间戳
        int time = 0;
        /// 信息发生着
        Member author;
        /// 信息撤回者
        Member operater;
        /// 信息id
        std::string ids;
        /// 信息内部ids
        std::string internalIds;

    public:
        explicit MemberRecallEvent(BaseEventData j);
    };

    /// 撤回信息
    namespace RecallEvent {
        // deprecated
        using FriendRecallEvent = FriendRecallEvent;
        // deprecated
        using MemberRecallEvent = MemberRecallEvent;
    }; // namespace RecallEvent

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
        std::optional<Member> inviter;
        /// 1-主动加入,2-被邀请加入,3-提供恢复群主身份加入
        int type;

    public:
        explicit BotJoinGroupEvent(BaseEventData j);
    };

    /// 群临时会话
    class GroupTempMessageEvent : public BotEvent<GroupTempMessageEvent>, public IMessageEvent {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::GroupTempMessageEvent;
        }

    public:
        /// 来源群
        Group group;
        /// 发送人
        Member sender;
        /// 信息
        MessageChain message;

    public:
        explicit GroupTempMessageEvent(BaseEventData j);

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

        eventTypes::Types getEventType() const override { return get_event_type(); }
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

    public:
        explicit NudgeEvent(BaseEventData j);
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
        Group group;
        EventType type;
        Member operater;

    public:
        explicit BotLeaveEvent(BaseEventData j);
    };

    /// 申请加群事件, bot需为管理员或者群主
    class MemberJoinRequestEvent : public BotEvent<MemberJoinRequestEvent> {
    private:
        std::string source;

    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MemberJoinRequestEvent;
        }

    public:
        /// 申请的群, 如果不存在就表明广播这个事件的时候机器人已经退出该群
        Group group;
        /// 邀请人, 如果不存在表明这个邀请人退出了群或没有邀请人为主动进群
        std::optional<Member> inviter;
        /// 申请人
        Member from;
        /// 申请人昵称
        std::string fromNick;
        /// 申请信息
        std::string message;

    public:
        explicit MemberJoinRequestEvent(BaseEventData j);

        /// 通过
        void accept();

        /// 拒绝
        void reject(std::string msg);
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

        explicit MessagePreSendEvent(BaseEventData j);
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

        eventTypes::Types getEventType() const override { return get_event_type(); }

        const MiraiCPExceptionBase *getException() const {
            return exceptionPtr;
        }
    };

    /// 事件监听操控, 可用于stop停止监听和resume继续监听
    class NodeHandle {
        struct NodeHandleInternal;

    private:
        NodeHandleInternal *handle;

    public:
        explicit NodeHandle(bool a);
        NodeHandle(NodeHandle &&) = delete;
        NodeHandle(const NodeHandle &) = delete;
        ~NodeHandle();
        [[nodiscard]] bool isEnable() const;
        void stop();
        void resume();
    };

    class MIRAICP_EXPORT Event {
    private: // typedefs
        class eventNode {
        public:
            std::function<bool(MiraiCPEvent *)> func;

        private:
            /// 回调的handle，用于管理
            NodeHandle _handle;

        public:
            eventNode() : func(nullptr), _handle(true) {}

            explicit eventNode(std::function<bool(MiraiCPEvent *)> f) : func(std::move(f)), _handle(true) {}

        public:
            /// 返回true代表block之后的回调
            bool run(MiraiCPEvent *a) const {
                return _handle.isEnable() && func(a);
            }

            NodeHandle *getHandle() {
                return &_handle;
            }
        };

        using priority_level = unsigned char;
        using event_vector = std::vector<std::unique_ptr<eventNode>>;
        using eventNodeTable = std::vector<std::map<priority_level, event_vector>>;

    private: // member
        eventNodeTable _all_events_;
        std::mutex eventsMtx;

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

        static void incomingEvent(BaseEventData j, int type);
        /// 广播一个事件, 必须为MiraiCPEvent的派生类
        template<typename EventClass>
        static void broadcast(EventClass &&val) {
            static_assert(std::is_base_of_v<MiraiCPEvent, EventClass>, "只支持广播MiraiCPEvent的派生类");
            MiraiCPEvent *p = &val;
            for (auto &&[k, v]: processor._all_events_[id<EventClass>()]) {
                for (auto &&a: v) {
                    if (a->run(p)) return;
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
        static NodeHandle *registerEvent(std::function<void(EventClass)> callback, priority_level level = 100) {
            static_assert(std::is_base_of_v<MiraiCPEvent, EventClass>, "只支持注册MiraiCPEvent的派生类事件");
            std::function<bool(MiraiCPEvent *)> tmp = [=](MiraiCPEvent *p) {
                callback(*static_cast<EventClass *>(p));
                return false;
            };
            std::lock_guard lk(processor.eventsMtx);
            auto &row = processor._all_events_[id<EventClass>()][level];
            row.emplace_back(std::make_unique<eventNode>(tmp));
            return row[row.size() - 1]->getHandle();
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
        static NodeHandle *registerBlockingEvent(std::function<bool(EventClass)> callback, priority_level level = 100) {
            static_assert(std::is_base_of_v<MiraiCPEvent, EventClass>, "只支持注册MiraiCPEvent的派生类事件");
            std::function<bool(MiraiCPEvent *)> tmp = [=](MiraiCPEvent *p) {
                return callback(*static_cast<EventClass *>(p));
            };
            std::lock_guard lk(processor.eventsMtx);
            auto &row = processor._all_events_[id<EventClass>()][level];
            row.emplace_back(std::make_unique<eventNode>(tmp));
            return row[row.size() - 1]->getHandle();
        }
    };
} // namespace MiraiCP


#endif //MIRAICP_PRO_EVENT_H
