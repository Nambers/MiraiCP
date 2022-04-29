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

namespace MiraiCP {
    /// Event 工厂
    enum struct eventTypes {
        BaseEvent [[maybe_unused]], // 0
        GroupMessageEvent,          // 1
        PrivateMessageEvent,        // 2
        GroupInviteEvent,           // 3
        NewFriendRequestEvent,      // 4
        MemberJoinEvent,            // 5
        MemberLeaveEvent,           // 6
        RecallEvent,                // 7
        BotJoinGroupEvent,          // 8
        GroupTempMessageEvent,      // 9
        TimeOutEvent,               // 10
        BotOnlineEvent,             // 11
        NudgeEvent,                 // 12
        BotLeaveEvent,              // 13
        MemberJoinRequestEvent,     // 14
        MessagePreSendEvent,        // 15
        MiraiCPExceptionEvent,      // 16
        Command,                    // 17
        count,                      // 事件在此位置前定义，此时count为事件种类数
        error                       // 出现问题时使用此enum
    };

    /// Event抽象父类
    class MiraiCPEvent {
    public:
        MiraiCPEvent() = default;
        virtual ~MiraiCPEvent() = default;

    public:
        static eventTypes get_event_type() { return eventTypes::error; }
        virtual eventTypes getEventType() const = 0;
    };

    /// 所有事件处理timeoutevent都是机器人事件，指都有机器人实例
    template<class T>
    class BotEvent : public MiraiCPEvent {

    public:
        eventTypes getEventType() const override { return T::get_event_type(); }

        // TODO: 考虑设置一个Bot全局变量，此处存储一个Bot指针，减少无用构造.

        /// 该事件接受的机器人
        Bot bot;

        /// 以该机器人的名义发送日志
        /// @see BotLogger
        IdLogger botlogger;

        explicit BotEvent(QQID botid) : bot(botid), botlogger(botid, &Logger::logger) {}
    };

    /*!
     * @brief 群消息事件声明
     * @example 取群聊下一条消息(check in version 2.9.0)
     * @code
     * auto tmp = e.nextMessage();
     * e.group.sendMessage(tmp);
     * @endcode
     */
    class GroupMessageEvent : public BotEvent<GroupMessageEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::GroupMessageEvent;
        }

    public:
        ///来源群
        Group group;
        ///发送人
        Member sender;
        /// 信息
        MessageChain message;

        GroupMessageEvent(QQID botid, const Group &group, const Member &sender,
                          MessageChain mc) : BotEvent(botid), group(group),
                                             sender(sender), message(std::move(mc)){};

        /*!
         * @brief 取群聊下一个消息(群聊与本事件一样)
         * @param time 超时时间限制, 单位为ms, 超时后抛出TimeOutException
         * @param halt 是否拦截该事件(不让这个消息被注册的其他监听器收到处理)
         * @return 消息链
         */
        MessageChain nextMessage(long time = -1, bool halt = true, JNIEnv *env = nullptr) const;

        /*!
         * @brief 取群聊中同群成员的下一个消息(发送人和群与本事件一样)
         * @param time 超时时间限制, 单位为ms, 超时后抛出TimeOutException
         * @param halt 是否拦截该事件(不让消息被注册的其他监听器收到处理)
         * @return 消息链
         */
        MessageChain senderNextMessage(long time = -1, bool halt = true, JNIEnv *env = nullptr) const;
    };

    /*!
     * @detail 私聊消息事件类声明
     * @example 取好友下一条信息(check in version 2.9.0)
     * @code
     * auto tmp = e.nextMessage();
     * e.sender.sendMessage(tmp);
     * @endcode
     */
    class PrivateMessageEvent : public BotEvent<PrivateMessageEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::PrivateMessageEvent;
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
        PrivateMessageEvent(QQID botid, Friend sender, MessageChain mc) : BotEvent(botid), sender(std::move(sender)),
                                                                          message(std::move(mc)){};

        /*!
         * @brief 取下一个消息(发送人和接收人和本事件一样)
         * @warning 如果两次发送信息间隔过短可能会漏过信息
         * @param time 超时时间限制, 单位为ms, 超时后抛出TimeOutException
         * @param halt 是否拦截该事件(不被注册的监听器收到处理)
         * @return 消息链
         */
        MessageChain nextMessage(long time = -1, bool halt = true, JNIEnv *env = nullptr) const;
    };

    /// 群聊邀请事件类声明
    class GroupInviteEvent : public BotEvent<GroupInviteEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::GroupInviteEvent;
        }

    public:
        /// 事件序列化文本
        std::string source;
        /// 发起人昵称
        std::string inviterNick;
        /// 发起人id
        QQID inviterid = 0;
        /// 被邀请进的组
        std::string groupName;
        /// 群号
        QQID groupid = 0;

        static void operation0(const std::string &source, QQID botid, bool accept,
                               JNIEnv *env = nullptr);

        void reject(JNIEnv *env = nullptr) {
            GroupInviteEvent::operation0(this->source, this->bot.id, false, env);
        }

        void accept(JNIEnv *env = nullptr) {
            GroupInviteEvent::operation0(this->source, this->bot.id, true, env);
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
        GroupInviteEvent(QQID botid, const std::string &source, const std::string &inviterNick,
                         QQID inviterid, const std::string &groupName, QQID groupid)
            : BotEvent(botid), source(source), inviterNick(inviterNick), inviterid(inviterid), groupName(groupName),
              groupid(groupid) {}
    };

    /// 好友申请事件声明
    class NewFriendRequestEvent : public BotEvent<NewFriendRequestEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::NewFriendRequestEvent;
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
        static void operation0(const std::string &source, QQID botid, bool accept,
                               JNIEnv *env = nullptr, bool ban = false);

        /// @brief 拒绝好友申请
        void reject(bool ban = false, JNIEnv *env = nullptr) {
            NewFriendRequestEvent::operation0(this->source, this->bot.id, false, env);
        }

        /// @brief 接受申请
        /// @param ban - 是否加入黑名单
        void accept(JNIEnv *env = nullptr) {
            NewFriendRequestEvent::operation0(this->source, this->bot.id, true, env);
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
        NewFriendRequestEvent(QQID botid, const std::string &source,
                              QQID fromid,
                              QQID fromgroupid, const std::string &nick,
                              const std::string &message)
            : BotEvent(botid), source(source), fromid(fromid), fromgroupid(fromgroupid), nick(nick),
              message(message) {}
    };

    /// 新群成员加入
    class MemberJoinEvent : public BotEvent<MemberJoinEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::MemberJoinEvent;
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
        MemberJoinEvent(QQID botid, int type, const Member &member, const Group &group,
                        QQID inviterid) : BotEvent(botid), type(joinType(type)), member(member),
                                          group(group),
                                          inviterid(inviterid) {}
    };

    /// 群成员离开
    class MemberLeaveEvent : public BotEvent<MemberLeaveEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::MemberLeaveEvent;
        }

    public:
        /*!
        * @brief 事件类型
        *           1 - 被踢出
        *           2 - 主动退出
        */
        int type = 0;
        /// 退出的成员q号
        QQID memberid;
        /// 目标群
        Group group;
        /// 操作人, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
        QQID operaterid;

        /*!
         * @brief 群成员离开
         * @param botid
         * @param type
         * @param memberid 退出的群成员
         * @param group 群
         * @param operaterid 操作人id, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
         */
        MemberLeaveEvent(QQID botid, int type, QQID memberid,
                         Group group,
                         QQID operaterid) : BotEvent(botid), type(type), memberid(memberid),
                                            group(std::move(group)),
                                            operaterid(operaterid) {}
    };

    /// 撤回信息
    class RecallEvent : public BotEvent<RecallEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::RecallEvent;
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
        RecallEvent(QQID botid, int type, int time, QQID authorid,
                    QQID operatorid, std::string ids, std::string internalids,
                    QQID groupid) : BotEvent(botid), type(type), time(time), authorid(authorid),
                                    operatorid(operatorid), ids(std::move(ids)),
                                    internalids(std::move(internalids)),
                                    groupid(groupid) {}
    };

    /// 机器人进入某群
    class BotJoinGroupEvent : public BotEvent<BotJoinGroupEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::BotJoinGroupEvent;
        }

    public:
        /// 1-主动加入,2-被邀请加入,3-提供恢复群主身份加入
        int type;
        /// 进入的群
        Group group;
        /// 当type=2时存在，为邀请人，否则为空，调用可能会报错
        QQID inviterid;

        /*!
         * @brief bot加入群
         * @param botid 对应bot
         * @param type 类别
         * @param group 加入的群
         * @param inviter 邀请人
         */
        BotJoinGroupEvent(QQID botid, int type, Group group,
                          QQID inviter)
            : BotEvent(botid), type(type), group(std::move(group)), inviterid(inviter) {}
    };

    /// 群临时会话
    class GroupTempMessageEvent : public BotEvent<GroupTempMessageEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::GroupTempMessageEvent;
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
        GroupTempMessageEvent(QQID botid, Group group, Member sender,
                              MessageChain message) : BotEvent(botid),
                                                      group(std::move(group)),
                                                      sender(std::move(sender)),
                                                      message(std::move(message)) {}
    };

    /// 定时任务结束
    class TimeOutEvent : public MiraiCPEvent {
    public:
        /// 事件所附信息
        std::string msg;

    public:
        explicit TimeOutEvent(std::string msg) : msg(std::move(msg)) {}

    public:
        static eventTypes get_event_type() {
            return eventTypes::TimeOutEvent;
        }

        eventTypes getEventType() const override { return this->get_event_type(); }
    };


    /// 机器人上线事件
    class BotOnlineEvent : public BotEvent<BotOnlineEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::BotOnlineEvent;
        }

    public:
        explicit BotOnlineEvent(QQID botid) : BotEvent(botid) {}
    };

    /*! 戳一戳事件
    /* @warning nudgeEvent事件也会被bot自己发的Nudge触发, 可能会造成无限循环
     */
    class NudgeEvent : public BotEvent<NudgeEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::NudgeEvent;
        }

    public:
        ///发送人
        Contact from;
        /// 目标
        Contact target;
        /// 发送的环境, 可能为Group / Friend
        Contact subject;

        NudgeEvent(const Contact &c, const Contact &target, const Contact &subject, QQID botid) : BotEvent(botid), from(c),
                                                                                                  target(target), subject(subject) {}
    };

    /// 机器人退群事件
    /// 可能有3种类型, 主动退/被踢/解散
    /// 目前mirai的botLeave事件还不稳定暂时不支持类型
    class BotLeaveEvent : public BotEvent<BotLeaveEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::BotLeaveEvent;
        }

    public:
        /// 退出的群
        /// @attension 收到这个事件时已经退出该群, 可能取不到相关信息
        QQID groupid;

        BotLeaveEvent(QQID g, QQID botid) : BotEvent(botid), groupid(g) {}
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
                            const std::string &msg = "",
                            JNIEnv *env = nullptr);

    public:
        static eventTypes get_event_type() {
            return eventTypes::MemberJoinRequestEvent;
        }

    public:
        /// 申请的群, 如果不存在就表明广播这个事件的时候机器人已经退出该群
        std::optional<Group> group;
        /// 邀请人, 如果不存在表明这个邀请人退出了群或没有邀请人为主动进群
        std::optional<Member> inviter;
        /// 申请人id
        QQID requesterId;

    public:
        MemberJoinRequestEvent(std::optional<Group> g, std::optional<Member> i, QQID botid, QQID requesterId, std::string source)
            : BotEvent(botid), group(std::move(g)), inviter(std::move(i)), source(std::move(source)), requesterId(requesterId){};

        /// 通过
        void accept() {
            operate(this->source, this->bot.id, true);
        }

        /// 拒绝
        void reject(const std::string &msg) {
            operate(this->source, this->bot.id, false, msg);
        }
    };

    /*! 每条消息发送前的事件, 总是在消息实际上被发送和广播MessagePostSendEvent前广播
     * @see MessagePostSendEvent
     * @warning 在这个事件里小心使用sendMessage, 可能会触发无限递归 preSend -> sendMessage -> preSend -> ...
     * */
    class MessagePreSendEvent : public BotEvent<MessagePreSendEvent> {
    public:
        static eventTypes get_event_type() {
            return eventTypes::MessagePreSendEvent;
        }

    public:
        /// 发送目标
        Contact target;
        /// 消息
        MessageChain message;

        explicit MessagePreSendEvent(Contact c, MessageChain mc, QQID botid) : BotEvent(botid), target(std::move(c)), message(std::move(mc)) {}
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
        static eventTypes get_event_type() {
            return eventTypes::MiraiCPExceptionEvent;
        }

        eventTypes getEventType() const override { return this->get_event_type(); }

        MiraiCPExceptionBase *getException() {
            return exceptionPtr;
        }
    };

    class Event {
    private: // typedefs
        class eventNode {
        public:
            bool enable = true;
            std::function<bool(MiraiCPEvent *)> func;

            explicit eventNode(std::function<bool(MiraiCPEvent *)> f) : func(std::move(f)) {}

            bool run(MiraiCPEvent *a) const {
                return func(a);
            }
        };

        /// 事件监听操控, 可用于stop停止监听和resume继续监听
        class NodeHandle {
        private:
            bool *enable;

        public:
            explicit NodeHandle(bool *a) { this->enable = a; }
            void stop() { *enable = false; }
            void resume() { *enable = true; }
        };

        using priority_level = unsigned char;
        using event_vector = std::vector<eventNode>;
        using eventNodeTable = std::vector<std::map<priority_level, event_vector>>;

    private: // member
        eventNodeTable _all_events_;

    private:
        Event() : _all_events_(int(eventTypes::count)){};

    public: // singleton mode
        static Event processor;

    private:
        template<typename T>
        int id() const {
            static_assert(std::is_base_of_v<MiraiCPEvent, T>, "只支持广播继承MiraiCPEvent的事件");
            return int(T::get_event_type());
        }

    public:
        bool noRegistered(int index) {
            return _all_events_[index].empty();
        }

        /// 广播一个事件, 必须为MiraiCPEvent的派生类
        template<typename T>
        void broadcast(T &&val) {
            static_assert(std::is_base_of_v<MiraiCPEvent, T>, "只支持广播MiraiCPEvent的派生类");
            MiraiCPEvent *p = &val;
            for (auto &&[k, v]: _all_events_[id<T>()]) {
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
         */
        template<typename T>
        NodeHandle registerEvent(std::function<void(T)> callback, priority_level level = 100) {
            static_assert(std::is_base_of_v<MiraiCPEvent, T>, "只支持注册MiraiCPEvent的派生类事件");
            std::function<bool(MiraiCPEvent *)> tmp = [=](MiraiCPEvent *p) {
                callback(*dynamic_cast<T *>(p));
                return false;
            };
            eventNode t(tmp);
            _all_events_[id<T>()][level].emplace_back(t);
            return NodeHandle(&t.enable);
        }

        /**
         * @brief 注册一个可以阻塞后续回调函数的回调。
         * 回调返回true时，将会忽略所有优先级低于当前回调，以及注册顺序晚于当前回调且优先级等于当前回调的所有其他回调函数
         * @param T 事件类型
         * @param callback 要注册的回调函数，必须返回bool值
         * @param priority_level 优先级，范围：0-255，越低的优先级越先执行，默认100
         */
        template<typename T>
        NodeHandle registerBlockingEvent(std::function<bool(T)> callback, priority_level level = 100) {
            static_assert(std::is_base_of_v<MiraiCPEvent, T>, "只支持注册MiraiCPEvent的派生类事件");
            std::function<bool(MiraiCPEvent *)> tmp = [=](MiraiCPEvent *p) {
                return callback(*dynamic_cast<T *>(p));
            };
            eventNode t(tmp);
            _all_events_[id<T>()][level].emplace_back(t);
            return NodeHandle(&t.enable);
        }
    };
} // namespace MiraiCP


#endif //MIRAICP_PRO_EVENT_H