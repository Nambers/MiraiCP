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
#define MiraiCPThrow(x) throw x.append(__FILE__, __LINE__)
#define ErrorHandle(x, y) ErrorHandle0(__FILE__, __LINE__, (x), (y))

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
    public:
        using string = std::string;

    protected:
        /// @brief 异常内容
        string re;

    public:
        /// @brief 发生异常的行号
        int lineNum = 0;

        /// @brief 发生异常的文件名
        string filename;

    public:
        ~MiraiCPExceptionBase() override = default;

    public:
        /// @brief 异常事件广播
        class ExceptionBroadcasting {
        public:
            MiraiCPExceptionBase *e;
            explicit ExceptionBroadcasting(MiraiCPExceptionBase *ex) : e(ex) {}
            virtual ~ExceptionBroadcasting();
        };

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
        /// 返回异常的类型
        /// @see getExceptionType
        static string exceptionType() { return "MiraiCPException"; }
    };

    /// @brief 总异常CRTP抽象类，不要直接抛出该类，不知道抛出什么的时候请抛出 MiraiCPException
    /// @interface MiraiCPExceptionCRTP
    template<class T>
    class MiraiCPExceptionCRTP : public MiraiCPExceptionBase {
    public:
        // 构造时传入类型字符串
        // 最好不要调用该构造函数
        explicit MiraiCPExceptionCRTP(const std::string &description = "") {
            if (description.empty())
                this->re = T::exceptionType() + ":MiraiCP异常";
            else
                this->re = T::exceptionType() + ":" + std::move(description);
        }

    public:
        // CRTP类型获取实现
        string getExceptionType() override { return T::exceptionType(); }

        /// 报错位置信息, 由MiraiThrow宏传递
        virtual T &append(string name, int line);
    };

    /// @brief 通用MiraiCP异常
    /// @see MiraiCPExceptionBase
    typedef MiraiCPExceptionCRTP<MiraiCPExceptionBase> MiraiCPException;

    /// 文件读取异常.
    /// @see MiraiCPExceptionBase
    class UploadException : public MiraiCPExceptionCRTP<UploadException> {
    private:
        std::string description;

    public:
        explicit UploadException(const std::string &text) {
            this->description = "上传(图片/文件)异常" + text;
            this->re = this->exceptionType() + this->description;
        }

        static std::string exceptionType() { return "UploadException"; }
    };

    /// 通常为Mirai返回
    /// @see MiraiCPExceptionBase
    class IllegalStateException : public MiraiCPExceptionCRTP<IllegalStateException> {
    private:
        std::string description;


    public:
        explicit IllegalStateException(const std::string &text) : MiraiCPExceptionCRTP("IllegalStateException") {
            this->description = "状态异常:" + text;
            this->re = exceptionType() + this->description;
        }

        static std::string exceptionType() { return "IllegalStateException"; }
    };

    /// 内部异常, 通常为json读写问题
    /// @see MiraiCPExceptionBase
    class APIException : public MiraiCPExceptionCRTP<APIException> {
    private:
        std::string description;


    public:
        explicit APIException(const std::string &text) {
            this->description = "MiraiCP内部无法预料的错误:" + text;
            this->re = APIException::exceptionType() + this->description;
        }

        static std::string exceptionType() { return "APIException"; }
    };

    /// 机器人操作异常
    /// @see MiraiCPExceptionBase
    class BotException : public MiraiCPExceptionCRTP<BotException> {
    private:
        std::string description;

    public:
        explicit BotException(std::string d = "没有权限执行该操作") {
            this->description = std::move(d);
            // BotIsBeingMutedException doesn't have a constructor, use `this`
            this->re = exceptionType() + this->description;
        }

        static std::string exceptionType() { return "BotException"; }
    };

    /// 被禁言异常, 通常发生于发送信息
    class BotIsBeingMutedException : public MiraiCPExceptionCRTP<BotIsBeingMutedException> {
    private:
        std::string description;

    public:
        /// 剩余禁言时间, 单位秒
        int timeRemain;

    public:
        explicit BotIsBeingMutedException(int t) {
            this->description = "发送信息失败, bot已被禁言, 剩余时间" + std::to_string(t);
            this->re = exceptionType() + this->description;
            timeRemain = t;
        }

        static std::string exceptionType() { return "BotIsBeingMutedException"; }
    };

    /// 禁言异常
    /// @see MiraiCPExceptionBase
    class MuteException : public MiraiCPExceptionCRTP<MuteException> {
    private:
        std::string description;


    public:
        /*
        *	 禁言时间超出0s~30d
        */
        MuteException() {
            this->description = "禁言时长不在0s~30d中间";
            this->re = MuteException::exceptionType() + this->description;
        }

        static std::string exceptionType() { return "MuteException"; }
    };

    /// 获取群成员错误
    /// @see MiraiCPExceptionBase
    class MemberException : public MiraiCPExceptionCRTP<MemberException> {
    private:
        std::string description;


    public:
        int type = 0;

        /*
        *   "1" - 找不到群
        *	"2" - 找不到群成员
        */
        explicit MemberException(int type) {
            this->type = type;
            switch (type) {
                case 1:
                    this->description = "找不到群";
                    break;
                case 2:
                    this->description = "找不到群成员";
                    break;
                default:
                    break;
            }
            this->re = MemberException::exceptionType() + this->description;
        }

        static std::string exceptionType() { return "MemberException"; }
    };

    /// 获取群成员错误
    /// @see MiraiCPExceptionBase
    class FriendException : public MiraiCPExceptionCRTP<FriendException> {
    private:
        std::string description;


    public:
        /*
        *   找不到好友
        */
        FriendException() {
            this->description = "找不到好友";
            this->re = exceptionType() + this->description;
        }

        static std::string exceptionType() { return "FriendException"; }
    };

    /// 获取群错误
    /// @see MiraiCPExceptionBase
    class GroupException : public MiraiCPExceptionCRTP<GroupException> {
    private:
        std::string description;


    public:
        GroupException() {
            this->description = "C++:找不到群";
            this->re = GroupException::exceptionType() + this->description;
        }

        static std::string exceptionType() { return "GroupException"; }
    };

    /// 撤回异常
    /// @see MiraiCPExceptionBase
    class RecallException : public MiraiCPExceptionCRTP<RecallException> {
    private:
        std::string description;


    public:
        RecallException() {
            this->description = "该消息已经被撤回";
            this->re = RecallException::exceptionType() + this->description;
        }

        static std::string exceptionType() { return "RecallException"; }
    };

    /// 远程资源出现问题
    /// @see MiraiCPExceptionBase
    class RemoteAssetException : public MiraiCPExceptionCRTP<RemoteAssetException> {
    private:
        std::string description;

    public:
        explicit RemoteAssetException(std::string e) {
            this->description = std::move(e);
            this->re = RemoteAssetException::exceptionType() + this->description;
        }

        static std::string exceptionType() { return "RemoteAssetException"; }
    };

    /// 参数错误
    /// @see MiraiCPExceptionBase
    class IllegalArgumentException : public MiraiCPExceptionCRTP<IllegalArgumentException> {
    private:
        std::string description;


    public:
        explicit IllegalArgumentException(std::string e) {
            this->description = std::move(e);
            this->re = IllegalArgumentException::exceptionType() + this->description;
        }

        static std::string exceptionType() { return "IllegalArgumentException"; }
    };

    /// 超时
    /// @see MiraiCPExceptionBase
    class TimeOutException : public MiraiCPExceptionCRTP<TimeOutException> {
    private:
        std::string description;


    public:
        explicit TimeOutException(std::string e) {
            this->description = std::move(e);
            this->re = TimeOutException::exceptionType() + this->description;
        }

        static std::string exceptionType() { return "TimeOutException"; }
    };

    /// 事件被取消, 一般出现在发送消息时在preSendMessageEvent取消的时候抛出
    /// @see MiraiCPExceptionBase
    class EventCancelledException : public MiraiCPExceptionCRTP<EventCancelledException> {
    private:
        std::string description;

    public:
        explicit EventCancelledException(const std::string &msg) {
            description = msg;
            re = EventCancelledException::exceptionType() + msg;
        }

        static std::string exceptionType() { return "EventCancelledException"; }
    };

    inline void ErrorHandle0(const std::string &name, int line, const std::string &re, const std::string &ErrorMsg = "") {
        if (re == "EF")
            throw FriendException().append(name, line);
        if (re == "EG")
            throw GroupException().append(name, line);
        if (re == "EM")
            throw MemberException(1).append(name, line);
        if (re == "EMM")
            throw MemberException(2).append(name, line);
        if (re == "EB")
            throw BotException("找不到bot:" + re).append(name, line);
        if (re == "EA")
            throw APIException(ErrorMsg).append(name, line);
        if (re == "EC")
            throw EventCancelledException("发送信息被取消").append(name, line);
        if (re == "ET")
            throw TimeOutException("发送信息超时").append(name, line);
        // equal to Tools::start_with
        if (re.rfind("EBM", 0) == 0)
            throw BotIsBeingMutedException(std::stoi(re.substr(3))).append(name, line);
    }
} // namespace MiraiCP

#endif //MIRAICP_PRO_EXCEPTION_H
