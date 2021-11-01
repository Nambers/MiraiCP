#ifndef MIRAICP_PRO_EVENT_H
#define MIRAICP_PRO_EVENT_H

#include "Bot.h"
#include "Contact.h"

namespace MiraiCP {
    /// Event 基类
    class MiraiCPEvent {};

    /// 所以事件处理timeoutevent都是机器人事件，指都有机器人实例
    class BotEvent : public MiraiCPEvent {
    public:
        /// 该事件接受的机器人
        Bot bot;
        /// 以该机器人的名义发送日志
        /// @see BotLogger
        IdLogger botlogger;

        explicit BotEvent(QQID botid) : bot(botid), botlogger(botid, &Logger::logger) {
        }
    };

    ///群消息事件声明
    class GroupMessageEvent : public BotEvent {
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
         * @warning 如果两次发送信息间隔过短可能会漏过信息
         * @param time 超时时间限制
         * @param halt 是否拦截该事件(不被注册的监听器收到处理)
         * @return MiraiCP::Message
         */
        MessageChain
        nextMessage(long time = -1, bool halt = true, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__));

        /*!
         * @brief 取群聊中同群成员的下一个消息(发送人和群与本事件一样)
         * @warning 如果两次发送信息间隔过短可能会漏过信息
         * @param time 超时时间限制
         * @param halt 是否拦截该事件(不被注册的监听器收到处理)
         * @return MiraiCP::Message
         */
        MessageChain
        senderNextMessage(long time = -1, bool halt = true, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__));
    };

    /// 私聊消息事件类声明
    class PrivateMessageEvent : public BotEvent {
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
                                                                          message(mc){};

        /*!
         * @brief 取下一个消息(发送人和接收人和本事件一样)
         * @warning 如果两次发送信息间隔过短可能会漏过信息
         * @param time 超时时间限制
         * @param halt 是否拦截该事件(不被注册的监听器收到处理)
         * @return MiraiCP::Message
         */
        MessageChain
        nextMessage(long time = -1, bool halt = true, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__));
    };

    /// 群聊邀请事件类声明
    class GroupInviteEvent : public BotEvent {
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
                               JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            nlohmann::json j;
            j["text"] = source;
            j["operate"] = accept;
            j["botid"] = botid;
            std::string re = Config::koperation(Config::Gioperation, j, env);
            if (re == "E") Logger::logger.error("群聊邀请事件同意失败(可能因为重复处理),id:" + source);
        }

        void reject(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            GroupInviteEvent::operation0(this->source, this->bot.id, false, env);
        }

        void accept(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
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
    class NewFriendRequestEvent : public BotEvent {
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
                               JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__), bool ban = false) {
            nlohmann::json j;
            j["text"] = source;
            j["operate"] = accept;
            j["botid"] = botid;
            j["ban"] = ban;
            std::string re = Config::koperation(Config::Nfroperation, j, env);
            if (re == "E") Logger::logger.error("好友申请事件同意失败(可能因为重复处理),id:" + source);
        }

        /// @brief 拒绝好友申请
        void reject(bool ban = false, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            NewFriendRequestEvent::operation0(this->source, this->bot.id, false, env);
        }

        /// @brief 接受申请
        /// @param ban - 是否加入黑名单
        void accept(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
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
    class MemberJoinEvent : public BotEvent {
    public:
        /*!
        * @brief 事件类型
        *   1 - 被邀请进来
        *   2 - 主动加入
        *   3 - 原群主通过 https://huifu.qq.com/ 恢复原来群主身份并入群
        */
        int type = 0;
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
                        QQID inviterid) : BotEvent(botid), type(type), member(member),
                                          group(group),
                                          inviterid(inviterid) {}
    };

    /// 群成员离开
    class MemberLeaveEvent : public BotEvent {
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
    class RecallEvent : public BotEvent {
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
    class BotJoinGroupEvent : public BotEvent {
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
    class GroupTempMessageEvent : public BotEvent {
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

        explicit TimeOutEvent(std::string msg) : msg(std::move(msg)) {}
    };


    /// 机器人上线事件
    class BotOnlineEvent : public BotEvent {
    public:
        explicit BotOnlineEvent(QQID botid) : BotEvent(botid) {}
    };

    /// 戳一戳事件
    class NudgeEvent : public BotEvent {
    public:
        /// 谁发送的
        Contact from;
        Contact target;

        NudgeEvent(Contact c, Contact target, QQID botid) : BotEvent(botid), from(std::move(c)),
                                                            target(std::move(target)) {}
    };

    /// 机器人退群事件
    /// 可能有3种类型, 主动退/被踢/解散
    /// 目前mirai的botLeave事件还不稳定暂时不支持类型
    class BotLeaveEvent : public BotEvent {
    public:
        /// 退出的群
        /// @attension 收到这个事件时已经退出该群, 可能取不到相关信息
        QQID groupid;

        BotLeaveEvent(QQID g, QQID botid) : BotEvent(botid), groupid(g) {}
    };

    /// 申请加群事件, bot需为管理员或者群主
    class MemberJoinRequestEvent : public BotEvent {
    private:
        std::string source;

        void operate(const std::string &s, QQID botid, bool sign, const std::string &msg = "",
                     JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) const {
            json j;
            j["source"] = s;
            j["botid"] = botid;
            j["sign"] = sign;
            j["msg"] = msg;
            Config::koperation(Config::MemberJoinRequest, j, env);
        }

    public:
        /// 申请的群, 如果不存在就表明广播这个事件的时候机器人已经退出该群
        std::optional<Group> group;
        /// 邀请人, 如果不存在表明这个邀请人退出了群或没有邀请人为主动进群
        std::optional<Member> inviter;

        MemberJoinRequestEvent(std::optional<Group> g, std::optional<Member> i, QQID botid, const std::string &source)
            : BotEvent(botid), group(std::move(g)), inviter(std::move(i)), source(source){};

        /// 通过
        void accept() {
            operate(this->source, this->bot.id, true);
        }

        /// 拒绝
        void reject(const std::string &msg) {
            operate(this->source, this->bot.id, false, msg);
        }
    };

    /// @brief 异常抛出事件
    class MiraiCPExceptionEvent : public MiraiCPEvent {
    private:
        MiraiCPException *exceptionPtr;

    public:
        explicit MiraiCPExceptionEvent(MiraiCPException *err) {
            exceptionPtr = err;
        }

        MiraiCPException &getException() {
            return *exceptionPtr;
        }
    };

    class Event {
    private:
        template<typename T>
        int id() const {
            static_assert(std::is_base_of_v<MiraiCPEvent, T>, "只支持广播继承MiraiCPEvent的事件");
            if constexpr (std::is_same_v<T, GroupMessageEvent>) {
                return 0;
            } else if constexpr (std::is_same_v<T, PrivateMessageEvent>) {
                return 1;
            } else if constexpr (std::is_same_v<T, GroupInviteEvent>) {
                return 2;
            } else if constexpr (std::is_same_v<T, NewFriendRequestEvent>) {
                return 3;
            } else if constexpr (std::is_same_v<T, MemberJoinEvent>) {
                return 4;
            } else if constexpr (std::is_same_v<T, MemberLeaveEvent>) {
                return 5;
            } else if constexpr (std::is_same_v<T, RecallEvent>) {
                return 6;
            } else if constexpr (std::is_same_v<T, BotJoinGroupEvent>) {
                return 7;
            } else if constexpr (std::is_same_v<T, GroupTempMessageEvent>) {
                return 8;
            } else if constexpr (std::is_same_v<T, BotOnlineEvent>) {
                return 9;
            } else if constexpr (std::is_same_v<T, NudgeEvent>) {
                return 10;
            } else if constexpr (std::is_same_v<T, BotLeaveEvent>) {
                return 11;
            } else if constexpr (std::is_same_v<T, MemberJoinRequestEvent>) {
                return 12;
            } else if constexpr (std::is_same_v<T, TimeOutEvent>) {
                return 13;
            } else if constexpr (std::is_same_v<T, MiraiCPExceptionEvent>) {
                return 14;
            }
            Logger::logger.error("内部错误, 位置:C-Head");
            return -1;
        }
        Event() = default;
        using type = std::variant<GroupMessageEvent, PrivateMessageEvent, GroupInviteEvent, NewFriendRequestEvent, MemberJoinEvent, MemberLeaveEvent, RecallEvent, BotJoinGroupEvent, GroupTempMessageEvent, BotOnlineEvent, TimeOutEvent, NudgeEvent, BotLeaveEvent, MemberJoinRequestEvent, MiraiCPExceptionEvent>;
        template<typename T>
        class Node {
        public:
            bool enable = true;
            std::function<void(T)> func;
            explicit Node(std::function<void(T)> f) : func(std::move(f)) {}
            void run(type a) const {
                func(std::get<T>(a));
            }
        };
        using e = std::variant<Node<GroupMessageEvent>, Node<PrivateMessageEvent>, Node<GroupInviteEvent>, Node<NewFriendRequestEvent>, Node<MemberJoinEvent>, Node<MemberLeaveEvent>, Node<RecallEvent>, Node<BotJoinGroupEvent>, Node<GroupTempMessageEvent>, Node<BotOnlineEvent>, Node<NudgeEvent>, Node<BotLeaveEvent>, Node<MemberJoinRequestEvent>, Node<MiraiCPExceptionEvent>, Node<TimeOutEvent>>;
        std::vector<e> vec[15] = {std::vector<e>()};

    public:
        /// 事件监听操控, 可用于stop停止监听和resume继续监听
        class NodeHandle {
        private:
            bool *enable;

        public:
            explicit NodeHandle(bool *a) {
                this->enable = a;
            }
            void stop() {
                *enable = false;
            }
            void resume() {
                *enable = true;
            }
        };
        // singleton mode
        static Event processor;
        /// 广播一个事件, 必须为MiraiCPEvent的派生类
        template<typename T>
        void broadcast(T val) {
            static_assert(std::is_base_of_v<MiraiCPEvent, T>, "只支持广播MiraiCPEvent的派生类");
            /// 清空stack中内容, 不然可能保留上一次Event的操作
            ThreadManager::getThread()->stack.clear();
            ThreadManager::getThread()->stack.push(__FILE__, __LINE__, __func__, typeid(T).name());
            for (e a: vec[id<T>()]) {
                std::get<Node<T>>(a).run(static_cast<type>(val));
            }
        }
        /// 注册一个事件
        template<typename T>
        NodeHandle registerEvent(std::function<void(T)> a) {
            static_assert(std::is_base_of_v<MiraiCPEvent, T>, "只支持注册MiraiCPEvent的派生类事件");
            auto t = Node<T>(a);
            vec[id<T>()].push_back(static_cast<e>(t));
            return NodeHandle(&t.enable);
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_EVENT_H
