// Copyright (c) 2021-2021. Eritque arcus and contributors.
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
#define MiraiCPThrow(x) throw((x).append(__FILE__, __LINE__))
#define ErrorHandle(x, y) ErrorHandle0(__FILE__, __LINE__, (x), (y))

#include <exception>
#include <string>

namespace MiraiCP {
    /// @brief 总异常抽象类
    /// @interface MiraiCPException
    class MiraiCPException : public std::exception {
    protected:
        /// 异常的类型
        const std::string exceptionType;
        std::string re;

    public:
        int lineNum = 0;
        std::string filename;

        //构造时传入类型字符串
        explicit MiraiCPException(const std::string &type, const std::string &description = "") : exceptionType(type) {
            if (description.empty())
                this->re = type + ":MiraiCP异常";
            else
                this->re = type + ":" + description;
        }

        /// @brief 异常事件广播
        class ExceptionBroadcasting {
        public:
            MiraiCPException *e;
            explicit ExceptionBroadcasting(MiraiCPException *ex) : e(ex) {}
            ~ExceptionBroadcasting();
        };

        /// 报错位置信息, 由MiraiThrow宏传递
        MiraiCPException append(const std::string &name, int line) {
            lineNum = line;
            filename = name;
            ExceptionBroadcasting(this);
            return *this;
        }

        /// 获取异常类型
        std::string getExceptionType() const { return exceptionType; }

        /// 异常信息
        const char *what() const noexcept override { return re.c_str(); }

        /// basicRaise 基本抛出方法，子类重写该方法
        virtual void basicRaise() const ;

        /// 实际抛出方法
        void raise() const;
    };

    /// 文件读取异常.
    /// @see MiraiCPException
    class UploadException : public MiraiCPException {
    private:
        std::string description;

    public:
        explicit UploadException(const std::string &text) : MiraiCPException("UploadException") {
            this->description = "上传(图片/文件)异常" + text;
            this->re = this->exceptionType + this->description;
        }
    };

    /// 通常为Mirai返回
    /// @see MiraiCPException
    class IllegalStateException : public MiraiCPException {
    private:
        std::string description;

    public:
        explicit IllegalStateException(const std::string &text) : MiraiCPException("IllegalStateException") {
            this->description = "状态异常:" + text;
            this->re = exceptionType + this->description;
        }
    };

    /// 内部异常, 通常为json读写问题
    /// @see MiraiCPException
    class APIException : public MiraiCPException {
    private:
        std::string description;

    public:
        explicit APIException(const std::string &text) : MiraiCPException("APIException") {
            this->description = "MiraiCP内部无法预料的错误:" + text;
            this->re = exceptionType + this->description;
        }
    };

    /// 机器人操作异常
    /// @see MiraiCPException
    class BotException : public MiraiCPException {
    private:
        std::string description;

    public:
        explicit BotException(std::string d = "没有权限执行该操作", const std::string &type = "BotException")
            : MiraiCPException(std::move(type)) {
            this->description = std::move(d);
            this->re = type + this->description;
        }
    };

    /// 被禁言异常, 通常发生于发送信息
    class BotIsBeingMutedException : public BotException {
    public:
        /// 剩余禁言时间, 单位秒
        int timeRemain;

        explicit BotIsBeingMutedException(int t) : BotException("发送信息失败, bot已被禁言, 剩余时间" + std::to_string(t),
                                                                "BotIsBeingMutedException") {
            timeRemain = t;
        }
    };

    /// 禁言异常
    /// @see MiraiCPException
    class MuteException : public MiraiCPException {
    private:
        std::string description;

    public:
        /*
        *	 禁言时间超出0s~30d
        */
        MuteException() : MiraiCPException("MuteException") {
            this->description = "禁言时长不在0s~30d中间";
            this->re = exceptionType + this->description;
        }
    };

    /// 获取群成员错误
    /// @see MiraiCPException
    class MemberException : public MiraiCPException {
    private:
        std::string description;

    public:
        int type = 0;

        /*
        *   "1" - 找不到群
        *	"2" - 找不到群成员
        */
        explicit MemberException(int type) : MiraiCPException("MemberException") {
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
            this->re = exceptionType + this->description;
        }
    };

    /// 获取群成员错误
    /// @see MiraiCPException
    class FriendException : public MiraiCPException {
    private:
        std::string description;

    public:
        /*
        *   找不到好友
        */
        FriendException() : MiraiCPException("FriendException") {
            this->description = "找不到好友";
            this->re = exceptionType + this->description;
        }
    };

    /// 获取群错误
    /// @see MiraiCPException
    class GroupException : public MiraiCPException {
    private:
        std::string description;

    public:
        GroupException() : MiraiCPException("GroupException") {
            this->description = "C++:找不到群";
            this->re = exceptionType + this->description;
        }
    };

    /// 撤回异常
    /// @see MiraiCPException
    class RecallException : public MiraiCPException {
    private:
        std::string description;

    public:
        RecallException() : MiraiCPException("RecallException") {
            this->description = "该消息已经被撤回";
            this->re = exceptionType + this->description;
        }
    };

    /// 远程资源出现问题
    /// @see MiraiCPException
    class RemoteAssetException : public MiraiCPException {
    private:
        std::string description;

    public:
        explicit RemoteAssetException(std::string e) : MiraiCPException("RemoteAssetException") {
            this->description = std::move(e);
            this->re = exceptionType + this->description;
        }
    };

    /// 参数错误
    /// @see MiraiCPException
    class IllegalArgumentException : public MiraiCPException {
    private:
        std::string description;

    public:
        explicit IllegalArgumentException(std::string e) : MiraiCPException("IllegalArgumentException") {
            this->description = std::move(e);
            this->re = exceptionType + this->description;
        }
    };

    /// 超时
    /// @see MiraiCPException
    class TimeOutException : public MiraiCPException {
    private:
        std::string description;

    public:
        explicit TimeOutException(std::string e) : MiraiCPException("TimeOutException") {
            this->description = std::move(e);
            this->re = exceptionType + this->description;
        }
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
    }
} // namespace MiraiCP

#endif //MIRAICP_PRO_EXCEPTION_H
