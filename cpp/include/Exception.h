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
#define MiraiCPThrow(x) throw(x).append(__FILE__, __LINE__)
#define MiraiCPThrow0(className, msg) throw className(msg, __FILE__, __LINE__)
#define ErrorHandle(x, y) ErrorHandle0(__FILE__, __LINE__, (x), (y))

#include <exception>
#include <string>

namespace MiraiCP {
    /// @brief 总异常抽象类
    /// @interface MiraiCPException
    class MiraiCPException : public std::exception {
        using string = std::string;

    protected:
        string re;

    public:
        int lineNum = 0;
        string filename;
        virtual ~MiraiCPException(){};
        /// @brief 异常事件广播
        class ExceptionBroadcasting {
        public:
            MiraiCPException *e;
            explicit ExceptionBroadcasting(MiraiCPException *ex) : e(ex) {}
            ~ExceptionBroadcasting();
        };
        /// 异常信息
        const char *what() const noexcept override { return re.c_str(); }

        /// basicRaise 基本抛出方法，子类重写该方法
        virtual void basicRaise() const;

        /// 实际抛出方法
        void raise() const;
    };

    template<class T>
    class MiraiCPBaseException : public MiraiCPException {
        using string = std::string;

    public:
        /// 异常的类型
        virtual string exceptionType() const { return "MiraiCPException"; }

    public:
        virtual ~MiraiCPBaseException(){};

        //构造时传入类型字符串
        explicit MiraiCPBaseException(std::string description = "") {
            static_assert(std::is_base_of_v<MiraiCPBaseException, T>);
            if (description.empty())
                this->re = exceptionType() + ":MiraiCP异常";
            else
                this->re = exceptionType() + ":" + std::move(description);
        }

        /// 报错位置信息, 由MiraiThrow宏传递
        T &append(std::string_view name, int line) {
            lineNum = line;
            filename = name;
            // destroy this object immediately, to call the destructor of `ExceptionBroadcasting`
            ExceptionBroadcasting(this); // NOLINT(bugprone-throw-keyword-missing)
            return *static_cast<T *>(this);
        }
    };

    /// 文件读取异常.
    /// @see MiraiCPException
    class UploadException : public MiraiCPBaseException<UploadException> {
    private:
        std::string description;


    public:
        explicit UploadException(const std::string &text) {
            this->description = "上传(图片/文件)异常" + text;
            this->re = this->exceptionType() + this->description;
        }
        std::string exceptionType() const override { return "UploadException"; }
    };

    /// 通常为Mirai返回
    /// @see MiraiCPException
    class IllegalStateException : public MiraiCPBaseException<IllegalStateException> {
    private:
        std::string description;


    public:
        explicit IllegalStateException(const std::string &text) : MiraiCPBaseException("IllegalStateException") {
            this->description = "状态异常:" + text;
            this->re = IllegalStateException::exceptionType() + this->description;
        }

        std::string exceptionType() const override { return "IllegalStateException"; }
    };

    /// 内部异常, 通常为json读写问题
    /// @see MiraiCPException
    class APIException : public MiraiCPBaseException<APIException> {
    private:
        std::string description;


    public:
        explicit APIException(const std::string &text) {
            this->description = "MiraiCP内部无法预料的错误:" + text;
            this->re = APIException::exceptionType() + this->description;
        }

        std::string exceptionType() const override { return "APIException"; }
    };

    /// 机器人操作异常
    /// @see MiraiCPException
    class BotException : public MiraiCPBaseException<BotException> {
    private:
        std::string description;


    public:
        explicit BotException(std::string d = "没有权限执行该操作") {
            this->description = std::move(d);
            // BotIsBeingMutedException doesn't have a constructor, use `this`
            this->re = this->exceptionType() + this->description;
        }

        std::string exceptionType() const override { return "BotException"; }
    };

    /// 被禁言异常, 通常发生于发送信息
    class BotIsBeingMutedException : public BotException {
    public:
        /// 剩余禁言时间, 单位秒
        int timeRemain;

    public:
        explicit BotIsBeingMutedException(int t) : BotException("发送信息失败, bot已被禁言, 剩余时间" + std::to_string(t)) {
            timeRemain = t;
        }

        std::string exceptionType() const override { return "BotIsBeingMutedException"; }
    };

    /// 禁言异常
    /// @see MiraiCPException
    class MuteException : public MiraiCPBaseException<MuteException> {
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

        std::string exceptionType() const override { return "MuteException"; }
    };

    /// 获取群成员错误
    /// @see MiraiCPException
    class MemberException : public MiraiCPBaseException<MemberException> {
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

        std::string exceptionType() const override { return "MemberException"; }
    };

    /// 获取群成员错误
    /// @see MiraiCPException
    class FriendException : public MiraiCPBaseException<FriendException> {
    private:
        std::string description;


    public:
        /*
        *   找不到好友
        */
        FriendException() : MiraiCPBaseException("FriendException") {
            this->description = "找不到好友";
            this->re = FriendException::exceptionType() + this->description;
        }

        std::string exceptionType() const override { return "FriendException"; }
    };

    /// 获取群错误
    /// @see MiraiCPException
    class GroupException : public MiraiCPBaseException<GroupException> {
    private:
        std::string description;


    public:
        GroupException() {
            this->description = "C++:找不到群";
            this->re = GroupException::exceptionType() + this->description;
        }

        std::string exceptionType() const override { return "GroupException"; }
    };

    /// 撤回异常
    /// @see MiraiCPException
    class RecallException : public MiraiCPBaseException<RecallException> {
    private:
        std::string description;


    public:
        RecallException() {
            this->description = "该消息已经被撤回";
            this->re = RecallException::exceptionType() + this->description;
        }

        std::string exceptionType() const override { return "RecallException"; }
    };

    /// 远程资源出现问题
    /// @see MiraiCPException
    class RemoteAssetException : public MiraiCPBaseException<RemoteAssetException> {
    private:
        std::string description;

    public:
        explicit RemoteAssetException(std::string e) {
            this->description = std::move(e);
            this->re = RemoteAssetException::exceptionType() + this->description;
        }

        std::string exceptionType() const override { return "RemoteAssetException"; }
    };

    /// 参数错误
    /// @see MiraiCPException
    class IllegalArgumentException : public MiraiCPBaseException<IllegalArgumentException> {
    private:
        std::string description;


    public:
        explicit IllegalArgumentException(std::string e) {
            this->description = std::move(e);
            this->re = IllegalArgumentException::exceptionType() + this->description;
        }

        std::string exceptionType() const override { return "IllegalArgumentException"; }
    };

    /// 超时
    /// @see MiraiCPException
    class TimeOutException : public MiraiCPBaseException<TimeOutException> {
    private:
        std::string description;


    public:
        explicit TimeOutException(std::string e) {
            this->description = std::move(e);
            this->re = TimeOutException::exceptionType() + this->description;
        }
        explicit TimeOutException(std::string e, const std::string &name, int line) {
            this->description = std::move(e);
            this->re = TimeOutException::exceptionType() + this->description;
            this->append(name, line);
        }

        std::string exceptionType() const override { return "TimeOutException"; }
    };

    /// 事件被取消, 一般出现在发送消息时在preSendMessageEvent取消的时候抛出
    /// @see MiraiCPException
    class EventCancelledException : public MiraiCPBaseException<EventCancelledException> {
    private:
        std::string description;

    public:
        explicit EventCancelledException(const std::string &msg) {
            description = msg;
            re = EventCancelledException::exceptionType() + msg;
        }

        std::string exceptionType() const override { return "EventCancelledException"; }
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
