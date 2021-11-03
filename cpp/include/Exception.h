#ifndef MIRAICP_PRO_EXCEPTION_H
#define MIRAICP_PRO_EXCEPTION_H

#include "Logger.h"
#include <string>

namespace MiraiCP {
    /// @brief 总异常抽象类
    /// @interface MiraiCPException
    class MiraiCPException : public std::exception {
    protected:
        /// 异常的类型
        const std::string exceptionType;

    public:
        int lineNum = 0;
        std::string filename = "";

        //构造时传入类型字符串
        explicit MiraiCPException(const std::string &&type) : exceptionType(type) {}

        /// @brief 异常事件广播
        class ExceptionBroadcasting {
        public:
            MiraiCPException *e;
            explicit ExceptionBroadcasting(MiraiCPException *ex) : e(ex) {}
            ~ExceptionBroadcasting();
        };

        MiraiCPException append(const std::string &name, int line) {
            lineNum = line;
            filename = name;
            ExceptionBroadcasting(this);
            return *this;
        }

        /// 获取异常类型
        std::string getExceptionType() { return exceptionType; }

        /// 异常信息
        virtual std::string what() { return getExceptionType() + "MiraiCP异常"; }

        /// basicRaise 基本抛出方法，子类重写该方法
        virtual void basicRaise() {
            Logger::logger.error(what());
        };

        /// 实际抛出方法
        void raise() {
            basicRaise();
            if (filename != "" && lineNum != 0)
                Logger::logger.error("文件名:" + filename + "\n行号:" + std::to_string(lineNum));
        }
    };

    /// 文件读取异常.
    /// @see MiraiCPException
    class UploadException : public MiraiCPException {
    public:
        explicit UploadException(const std::string &text) : MiraiCPException("UploadException") {
            this->description = "上传(图片/文件)异常" + text;
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            Logger::logger.error(this->description);
            //ThreadManager::getEnv()->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 通常为Mirai返回
    /// @see MiraiCPException
    class IllegalStateException : public MiraiCPException {
    public:
        explicit IllegalStateException(const std::string &text) : MiraiCPException("IllegalStateException") {
            this->description = "状态异常:" + text;
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            Logger::logger.error(this->description);
            // ThreadManager::getEnv()->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 内部异常, 通常为json读写问题
    /// @see MiraiCPException
    class APIException : public MiraiCPException {
    public:
        explicit APIException(const std::string &text) : MiraiCPException("APIException") {
            this->description = "MiraiCP内部无法预料的错误:" + text;
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            Logger::logger.error(this->description);
            //ThreadManager::getEnv()->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 机器人操作异常
    /// @see MiraiCPException
    class BotException : public MiraiCPException {
    public:
        explicit BotException(std::string d = "没有权限执行该操作", const std::string &&type = "BotException")
            : MiraiCPException(std::move(type)) {
            this->description = std::move(d);
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            //ThreadManager::getEnv()->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
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
    public:
        /*
        *	 禁言时间超出0s~30d
        */
        MuteException() : MiraiCPException("MuteException") {
            this->description = "禁言时长不在0s~30d中间";
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            //ThreadManager::getEnv()->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 获取群成员错误
    /// @see MiraiCPException
    class MemberException : public MiraiCPException {
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
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            //ThreadManager::getEnv()->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 获取群成员错误
    /// @see MiraiCPException
    class FriendException : public MiraiCPException {
    public:
        /*
        *   找不到好友
        */
        FriendException() : MiraiCPException("FriendException") {
            this->description = "找不到好友";
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            //ThreadManager::getEnv()->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 获取群错误
    /// @see MiraiCPException
    class GroupException : public MiraiCPException {
    public:
        GroupException() : MiraiCPException("GroupException") {
            this->description = "C++:找不到群";
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            //ThreadManager::getEnv()->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 撤回异常
    /// @see MiraiCPException
    class RecallException : public MiraiCPException {
    public:
        RecallException() : MiraiCPException("RecallException") {
            this->description = "该消息已经被撤回";
        }

        std::string what() override {
            return exceptionType + this->description;
        }

    private:
        std::string description;
    };

    /// 远程资源出现问题
    /// @see MiraiCPException
    class RemoteAssetException : public MiraiCPException {
    public:
        explicit RemoteAssetException(std::string e) : MiraiCPException("RemoteAssetException") {
            this->description = std::move(e);
        }

        std::string what() override {
            return exceptionType + this->description;
        }

    private:
        std::string description;
    };

    /// 参数错误
    /// @see MiraiCPException
    class IllegalArgumentException : public MiraiCPException {
    private:
        std::string description;

    public:
        explicit IllegalArgumentException(std::string e) : MiraiCPException("IllegalArgumentException") {
            this->description = std::move(e);
        }

        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            Logger::logger.error(this->what());
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
        }

        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            Logger::logger.error(this->what());
        }
    };

    inline void ErrorHandle(const std::string &re, const std::string &ErrorMsg = "") {
        if (re == "EF")
            MiraiCPThrow(FriendException());
        if (re == "EG")
            MiraiCPThrow(GroupException());
        if (re == "EM")
            MiraiCPThrow(MemberException(1));
        if (re == "EMM")
            MiraiCPThrow(MemberException(2));
        if (re == "EB")
            MiraiCPThrow(BotException("找不到bot:" + re));
        if (re == "EA")
            MiraiCPThrow(APIException(ErrorMsg));
    }
} // namespace MiraiCP

#endif //MIRAICP_PRO_EXCEPTION_H
