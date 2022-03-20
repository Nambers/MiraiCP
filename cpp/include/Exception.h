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

// #define MiraiCPThrow(x) throw x.append(__FILE__, __LINE__)
#define ErrorHandle(x, y) ErrorHandle0(__FILE__, __LINE__, (x), (y))
#define MIRAICP_EXCEPTION_WHERE __FILE__, __LINE__

#if defined(_MSC_VER)
#define ShouldNotUse(msg) _Pragma("warning(error:4996)") [[deprecated(msg)]] _Pragma("warning(warning:4996)")
#else
#if defined(__GNUC__)
#define ShouldNotUse(msg) [[deprecated(msg)]] __attribute__((error(msg)))
#else
#define ShouldNotUse(msg)
#endif
#endif

#include <exception>
#include <string>

namespace MiraiCP {
    /// @brief 总异常抽象类，用于一般捕获，不要直接抛出该类，不知道抛出什么的时候请抛出 MiraiCPException
    /// @interface MiraiCPExceptionBase
    class MiraiCPExceptionBase : public ::std::exception {
    protected:
        using string = std::string;

    protected:
        /// @brief 异常内容
        string re;

    public:
        /// @brief 发生异常的行号
        int lineNum = 0;

        /// @brief 发生异常的文件名
        string filename;

    protected:
        /// 受保护构造函数，供子类调用
        MiraiCPExceptionBase(string _filename, int _lineNum) : filename(std::move(_filename)), lineNum(_lineNum) {}

    public:
        ~MiraiCPExceptionBase() override = default;

    protected:
        /// @brief 异常事件广播
        void exception_broadcast();

    public:
        /// 异常信息
        const char *what() const noexcept override { return re.c_str(); }

        /// 实际抛出方法
        void raise() const;

    public: // 暴露的接口
        /// basicRaise 基本抛出方法，子类重写该方法
        virtual void basicRaise() const;

        // CRTP实现一次，调用静态的exceptionType
        /// 获取异常类型，通用接口
        virtual string getExceptionType() = 0;

        // 每个子类需要单独实现该静态方法
        /// 返回异常的类型，该静态方法无法正确实现多态，请使用 getExceptionType
        /// @see getExceptionType
        static string exceptionType() { return "MiraiCPException"; }
    };

    /// @brief 总异常CRTP抽象类，不要直接抛出该类，不知道抛出什么的时候请抛出 MiraiCPException
    /// @interface MiraiCPExceptionCRTP
    /// @note 请勿给该类增加新的属性。如果要增加属性应在 MiraiCPExceptionBase 中增加
    template<class T>
    class MiraiCPExceptionCRTP : public MiraiCPExceptionBase {
    protected:
        /// 委托构造函数
        MiraiCPExceptionCRTP(string _filename, int _lineNum) : MiraiCPExceptionBase(std::move(_filename), _lineNum) {
            exception_broadcast();
        }

    public:
        // 构造时传入类型字符串
        // 最好不要调用该构造函数
        explicit MiraiCPExceptionCRTP(const string &description, string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            if (description.empty())
                this->re = T::exceptionType() + ":MiraiCP异常";
            else
                this->re = T::exceptionType() + ":" + description;
        }

    public:
        // CRTP类型获取实现
        string getExceptionType() override { return T::exceptionType(); }
    };

    /// @brief 通用MiraiCP异常
    /// @param const string &description, string _filename, int _lineNum
    /// @see MiraiCPExceptionBase
    typedef MiraiCPExceptionCRTP<MiraiCPExceptionBase> MiraiCPException;

    /// 文件读取异常.
    /// @see MiraiCPExceptionBase
    class UploadException : public MiraiCPExceptionCRTP<UploadException> {
    public:
        explicit UploadException(const std::string &text, string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            this->re = "上传(图片/文件)异常" + text;
        }

        static std::string exceptionType() { return "UploadException"; }
    };

    /// 通常为Mirai返回
    /// @see MiraiCPExceptionBase
    class IllegalStateException : public MiraiCPExceptionCRTP<IllegalStateException> {
    public:
        explicit IllegalStateException(const std::string &text, string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            this->re = "状态异常:" + text;
        }

        static std::string exceptionType() { return "IllegalStateException"; }
    };

    /// 内部异常, 通常为json读写问题
    /// @see MiraiCPExceptionBase
    class APIException : public MiraiCPExceptionCRTP<APIException> {
    public:
        explicit APIException(const std::string &text, string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            this->re = "MiraiCP内部无法预料的错误:" + text;
        }

        static string exceptionType() { return "APIException"; }
    };

    /// 机器人操作异常
    /// @see MiraiCPExceptionBase
    class BotException : public MiraiCPExceptionCRTP<BotException> {
    public:
        explicit BotException(string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            re = "没有权限执行该操作";
        }

        explicit BotException(string d, string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            re = std::move(d);
        }

        static string exceptionType() { return "BotException"; }
    };

    /// 被禁言异常, 通常发生于发送信息
    class BotIsBeingMutedException : public MiraiCPExceptionCRTP<BotIsBeingMutedException> {
    public:
        /// 剩余禁言时间, 单位秒
        int timeRemain;

    public:
        explicit BotIsBeingMutedException(int t, string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            this->re = "发送信息失败, bot已被禁言, 剩余时间" + std::to_string(t);
            timeRemain = t;
        }

        static string exceptionType() { return "BotIsBeingMutedException"; }
    };

    /// 禁言异常
    /// @see MiraiCPExceptionBase
    class MuteException : public MiraiCPExceptionCRTP<MuteException> {
    public:
        /*
        *	 禁言时间超出0s~30d
        */
        MuteException(string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            this->re = "禁言时长不在0s~30d中间";
        }

        static string exceptionType() { return "MuteException"; }
    };

    /// 获取群成员错误
    /// @see MiraiCPExceptionBase
    class MemberException : public MiraiCPExceptionCRTP<MemberException> {
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
        explicit MemberException(int _type, string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            type = MemberExceptionType(_type);
            switch (type) {
                case NoSuchGroup:
                    this->re = "找不到群";
                    break;
                case NoSuchMember:
                    this->re = "找不到群成员";
                    break;
                default:
                    break;
            }
            this->re = "";
        }

        static string exceptionType() { return "MemberException"; }
    };

    /// 获取群成员错误
    /// @see MiraiCPExceptionBase
    class FriendException : public MiraiCPExceptionCRTP<FriendException> {
    public:
        /*
        *   找不到好友
        */
        FriendException(string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            this->re = "找不到好友";
        }

        static string exceptionType() { return "FriendException"; }
    };

    /// 获取群错误
    /// @see MiraiCPExceptionBase
    class GroupException : public MiraiCPExceptionCRTP<GroupException> {
    public:
        GroupException(string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            this->re = "C++:找不到群";
        }

        static string exceptionType() { return "GroupException"; }
    };

    /// 撤回异常
    /// @see MiraiCPExceptionBase
    class RecallException : public MiraiCPExceptionCRTP<RecallException> {
    public:
        RecallException(string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            this->re = "该消息已经被撤回";
        }

        static string exceptionType() { return "RecallException"; }
    };

    /// 远程资源出现问题
    /// @see MiraiCPExceptionBase
    class RemoteAssetException : public MiraiCPExceptionCRTP<RemoteAssetException> {
    public:
        explicit RemoteAssetException(string e, string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            this->re = std::move(e);
        }

        static string exceptionType() { return "RemoteAssetException"; }
    };

    /// 参数错误
    /// @see MiraiCPExceptionBase
    class IllegalArgumentException : public MiraiCPExceptionCRTP<IllegalArgumentException> {
    public:
        explicit IllegalArgumentException(string e, string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            this->re = std::move(e);
        }

        static string exceptionType() { return "IllegalArgumentException"; }
    };

    /// 超时
    /// @see MiraiCPExceptionBase
    class TimeOutException : public MiraiCPExceptionCRTP<TimeOutException> {
    public:
        explicit TimeOutException(std::string e, string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            this->re = std::move(e);
        }

        static string exceptionType() { return "TimeOutException"; }
    };

    /// 事件被取消, 一般出现在发送消息时在preSendMessageEvent取消的时候抛出
    /// @see MiraiCPExceptionBase
    class EventCancelledException : public MiraiCPExceptionCRTP<EventCancelledException> {
    public:
        explicit EventCancelledException(const string &msg, string _filename, int _lineNum) : MiraiCPExceptionCRTP(std::move(_filename), _lineNum) {
            re = msg;
        }

        static string exceptionType() { return "EventCancelledException"; }
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
        // equal to Tools::start_with
        if (re.rfind("EBM", 0) == 0)
            throw BotIsBeingMutedException(std::stoi(re.substr(3)), name, line);
    }
} // namespace MiraiCP

#endif //MIRAICP_PRO_EXCEPTION_H
