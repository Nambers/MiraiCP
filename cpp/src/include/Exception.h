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

#ifndef MIRAICP_PRO_EXCEPTION_H
#define MIRAICP_PRO_EXCEPTION_H


#include "CPPPlugin.h"
#include "MiraiCPMacros.h"
#include <exception>
#include <string>
#include <thread>


namespace MiraiCP {
    /// @brief 总异常抽象类，用于一般捕获，不要直接抛出该类，不知道抛出什么的时候请抛出 MiraiCPException
    /// @interface MiraiCPExceptionBase
    class MIRAICP_EXPORT MiraiCPExceptionBase : public ::std::exception {
    protected:
        using string = std::string;

    protected:
        /// @brief 异常内容
        string re;

    public:
        /// @brief 发生异常的文件名
        string filename;

        /// @brief 发生异常的行号
        int lineNum = 0;

    protected:
        /// 受保护构造函数，供子类调用
        MiraiCPExceptionBase(string info, string _filename, int _lineNum) : re(std::move(info)), filename(std::move(_filename)), lineNum(_lineNum) {}

    public:
        ~MiraiCPExceptionBase() override = default;

    public:
        /// 异常信息
        const char *what() const noexcept override { return re.c_str(); }

        /// 返回std::string的异常信息
        string getError() const { return re; }

        /// 实际抛出方法
        void raise() const;

    public: // 暴露的接口
        /// basicRaise 基本抛出方法，子类重写该方法
        virtual void basicRaise() const;

        // CRTP实现一次，调用静态的exceptionType
        /// 获取异常类型，通用接口
        virtual string getExceptionType() const = 0;

        // 每个子类需要单独实现该静态方法
        /// 返回异常的类型，该静态方法无法正确实现多态，请使用 getExceptionType
        /// @see getExceptionType
        static string exceptionType() { return "MiraiCPException"; }
    };

    /// @brief 总异常CRTP抽象类，不要直接抛出该类，不知道抛出什么的时候请抛出 MiraiCPException。
    /// 该类是用于继承的基类，需要新的异常类型时，继承该类并以子类作为模板参数。
    /// 子类需要实现的方法：
    /// 1. 构造函数，要求必须委托MiraiCPExceptionCRTP构造。
    /// 2. `static std::string exceptionType()` 返回一个字符串表示异常类型。
    /// 继承该类后异常类能正确实现多态。
    /// @interface MiraiCPExceptionCRTP
    /// @note 请勿给该类增加新的属性。如果要增加属性应在 MiraiCPExceptionBase 中增加
    template<class T>
    class MIRAICP_EXPORT MiraiCPExceptionCRTP : public MiraiCPExceptionBase {
    public:
        /// 委托构造函数
        explicit MiraiCPExceptionCRTP(string _re, string _filename, int _lineNum) : MiraiCPExceptionBase(std::move(_re), std::move(_filename), _lineNum) {
        }

    public:
        // CRTP类型获取实现
        string getExceptionType() const override { return T::exceptionType(); }
    };

    /// @brief 通用MiraiCP异常
    /// @param const string &description, string _filename, int _lineNum
    /// @see MiraiCPExceptionBase
    MIRAICP_EXPORT typedef MiraiCPExceptionCRTP<MiraiCPExceptionBase> MiraiCPException;

    /// 文件读取异常.
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT UploadException : public MiraiCPExceptionCRTP<UploadException> {
    public:
        explicit UploadException(const std::string &text, string _filename, int _lineNum) : MiraiCPExceptionCRTP("上传(图片/文件)异常" + text, std::move(_filename), _lineNum) {}

        static std::string exceptionType() { return "UploadException"; }
    };

    /// 通常为Mirai返回
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT IllegalStateException : public MiraiCPExceptionCRTP<IllegalStateException> {
    public:
        explicit IllegalStateException(const std::string &text, string _filename, int _lineNum) : MiraiCPExceptionCRTP("状态异常:" + text, std::move(_filename), _lineNum) {}

        static std::string exceptionType() { return "IllegalStateException"; }
    };

    /// 内部异常, 通常为json读写问题
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT APIException : public MiraiCPExceptionCRTP<APIException> {
    public:
        explicit APIException(const std::string &text, string _filename, int _lineNum) : MiraiCPExceptionCRTP("MiraiCP内部无法预料的错误:" + text, std::move(_filename), _lineNum) {}

        static string exceptionType() { return "APIException"; }
    };

    /// 机器人操作异常
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT BotException : public MiraiCPExceptionCRTP<BotException> {
    public:
        explicit BotException(string _filename, int _lineNum) : MiraiCPExceptionCRTP("没有权限执行该操作", std::move(_filename), _lineNum) {}

        explicit BotException(const string &d, string _filename, int _lineNum) : MiraiCPExceptionCRTP(d, std::move(_filename), _lineNum) {}

        static string exceptionType() { return "BotException"; }
    };

    /// 被禁言异常, 通常发生于发送信息
    class MIRAICP_EXPORT BotIsBeingMutedException : public MiraiCPExceptionCRTP<BotIsBeingMutedException> {
    public:
        /// 剩余禁言时间, 单位秒
        int timeRemain;

    public:
        explicit BotIsBeingMutedException(int t, string _filename, int _lineNum) : timeRemain(t), MiraiCPExceptionCRTP("发送信息失败, bot已被禁言, 剩余时间" + std::to_string(t), std::move(_filename), _lineNum) {}

        static string exceptionType() { return "BotIsBeingMutedException"; }
    };

    /// 禁言异常
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT MuteException : public MiraiCPExceptionCRTP<MuteException> {
    public:
        /*
        *	 禁言时间超出0s~30d
        */
        MuteException(string _filename, int _lineNum) : MiraiCPExceptionCRTP("禁言时长不在0s~30d中间", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "MuteException"; }
    };

    /// 获取群成员错误
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT MemberException : public MiraiCPExceptionCRTP<MemberException> {
    public:
        enum MemberExceptionType : int {
            OtherType,
            NoSuchGroup,
            NoSuchMember
        };
        MemberExceptionType type = OtherType;

        /*
        *   "1" - 找不到群
        *	"2" - 找不到群成员
        */
        explicit MemberException(int _type, string _filename, int _lineNum) : MiraiCPExceptionCRTP(
                                                                                      [&]() -> string {
                                                                                          type = MemberExceptionType(_type);
                                                                                          switch (type) {
                                                                                              case NoSuchGroup:
                                                                                                  return "找不到群";
                                                                                              case NoSuchMember:
                                                                                                  return "找不到群成员";
                                                                                              default:
                                                                                                  return "";
                                                                                          }
                                                                                      }(),
                                                                                      std::move(_filename), _lineNum) {}

        static string exceptionType() { return "MemberException"; }
    };

    /// 获取群成员错误
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT FriendException : public MiraiCPExceptionCRTP<FriendException> {
    public:
        /*
        *   找不到好友
        */
        FriendException(string _filename, int _lineNum) : MiraiCPExceptionCRTP("找不到好友", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "FriendException"; }
    };

    /// 获取群错误
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT GroupException : public MiraiCPExceptionCRTP<GroupException> {
    public:
        GroupException(string _filename, int _lineNum) : MiraiCPExceptionCRTP("找不到群", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "GroupException"; }
    };

    /// 撤回异常
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT RecallException : public MiraiCPExceptionCRTP<RecallException> {
    public:
        RecallException(string _filename, int _lineNum) : MiraiCPExceptionCRTP("该消息已经被撤回", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "RecallException"; }
    };

    /// 远程资源出现问题
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT RemoteAssetException : public MiraiCPExceptionCRTP<RemoteAssetException> {
    public:
        explicit RemoteAssetException(const string &e, string _filename, int _lineNum) : MiraiCPExceptionCRTP(e, std::move(_filename), _lineNum) {}

        static string exceptionType() { return "RemoteAssetException"; }
    };

    /// 参数错误
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT IllegalArgumentException : public MiraiCPExceptionCRTP<IllegalArgumentException> {
    public:
        explicit IllegalArgumentException(const string &e, string _filename, int _lineNum) : MiraiCPExceptionCRTP(e, std::move(_filename), _lineNum) {
        }

        static string exceptionType() { return "IllegalArgumentException"; }
    };

    /// 超时
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT TimeOutException : public MiraiCPExceptionCRTP<TimeOutException> {
    public:
        explicit TimeOutException(const std::string &e, string _filename, int _lineNum) : MiraiCPExceptionCRTP(e, std::move(_filename), _lineNum) {}

        static string exceptionType() { return "TimeOutException"; }
    };

    /// 事件被取消, 一般出现在发送消息时在preSendMessageEvent取消的时候抛出
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT EventCancelledException : public MiraiCPExceptionCRTP<EventCancelledException> {
    public:
        explicit EventCancelledException(const string &msg, string _filename, int _lineNum) : MiraiCPExceptionCRTP(msg, std::move(_filename), _lineNum) {}

        static string exceptionType() { return "EventCancelledException"; }
    };

    /// 插件没有权限时抛出该异常
    /// 该异常仅可能在插件尝试调用libLoader 高级权限的Api接口时抛出
    /// 如插件尝试重载、加载、卸载插件等操作，但配置文件中并没有赋予该插件权限时
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT PluginNotAuthorizedException : public MiraiCPExceptionCRTP<PluginNotAuthorizedException> {
    public:
        explicit PluginNotAuthorizedException(string _filename, int _lineNum) : MiraiCPExceptionCRTP("插件" + CPPPlugin::config.getId() + "没有管理权限", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "PluginNotAuthorizedException"; }
    };

    /// 插件未加载抛出该异常
    /// 在插件能正常运行时不会抛出，出现该异常事件时请不要再次尝试收发消息等Mirai操作，
    /// 否则可能导致异常处理时再次抛出异常
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT PluginNotEnabledException : public MiraiCPExceptionCRTP<PluginNotEnabledException> {
    public:
        explicit PluginNotEnabledException(string _filename, int _lineNum) : MiraiCPExceptionCRTP("插件" + CPPPlugin::config.getId() + "未加载", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "PluginNotEnabledException"; }
    };

    /// 如果在 MiraiCPNewThread 中捕获到了非 MiraiCP 之外的异常抛出
    /// @see MiraiCPNewThread
    class MIRAICP_EXPORT MiraiCPThreadException : public MiraiCPExceptionCRTP<MiraiCPThreadException> {
    public:
        /// 抛出异常的线程 ID
        std::thread::id threadId;

    public:
        explicit MiraiCPThreadException(const std::string &exception_content, std::thread::id threadId, string _filename, int _lineNum)
            : MiraiCPExceptionCRTP(exception_content + " at threadId: " + getThreadIdStr(threadId), std::move(_filename), _lineNum),
              threadId(threadId) {}

    public:
        std::string getThreadIdStr() const { return getThreadIdStr(threadId); }

    public:
        static string exceptionType() { return "MiraiCPThreadException"; }

    private:
        static std::string getThreadIdStr(const std::thread::id &id);
    };

    inline void ErrorHandle0(const std::string &name, int line, const std::string &re, const std::string &ErrorMsg = "") {
        if (re == "EF")
            throw FriendException(name, line);
        if (re == "EG")
            throw GroupException(name, line);
        if (re == "EM")
            throw MemberException(1, name, line);
        if (re == "EMM")
            throw MemberException(2, name, line);
        if (re == "EB")
            throw BotException("找不到bot:" + re, name, line);
        if (re == "EA")
            throw APIException(ErrorMsg, name, line);
        if (re == "EC")
            throw EventCancelledException("发送信息被取消", name, line);
        if (re == "ET")
            throw TimeOutException("发送信息超时", name, line);
        if (re == "EP")
            throw BotException(name, line);
        // equal to Tools::start_with
        if (re.rfind("EBM", 0) == 0)
            throw BotIsBeingMutedException(std::stoi(re.substr(3)), name, line);
    }
} // namespace MiraiCP

#endif //MIRAICP_PRO_EXCEPTION_H
