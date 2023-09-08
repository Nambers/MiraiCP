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

#include "MiraiCPMacros.h"
// -----------------------
#include "CPPPlugin.h"
#include "nlohmann/json.hpp"
#include <exception>
#include <string>
#include <thread>

#define EXCEPTION_DEFINE(name, cause)                                                                                 \
    class MIRAICP_EXPORT name : public MiraiCPExceptionCRTP<name> {                                                   \
    public:                                                                                                           \
        explicit name(string _filename, int _lineNum) : MiraiCPExceptionCRTP(cause, std::move(_filename), _lineNum) { \
        }                                                                                                             \
        static string exceptionType() { return #name; }                                                               \
    }
#define EXCEPTION_DEFINE_W_NUM(name, cause)                                                                                     \
    class MIRAICP_EXPORT name : public MiraiCPExceptionCRTP<name> {                                                             \
    public:                                                                                                                     \
        explicit name(int info, string _filename, int _lineNum) : MiraiCPExceptionCRTP(cause, std::move(_filename), _lineNum) { \
        }                                                                                                                       \
        static string exceptionType() { return #name; }                                                                         \
    }
#define EXCEPTION_DEFINE_W_STR(name, cause)                                                                                        \
    class MIRAICP_EXPORT name : public MiraiCPExceptionCRTP<name> {                                                                \
    public:                                                                                                                        \
        explicit name(string info, string _filename, int _lineNum) : MiraiCPExceptionCRTP(cause, std::move(_filename), _lineNum) { \
        }                                                                                                                          \
        static string exceptionType() { return #name; }                                                                            \
    }

namespace MiraiCP {
    /// @brief 总异常抽象类，用于一般捕获，不要直接抛出该类，不知道抛出什么的时候请抛出 MiraiCPException
    /// @interface MiraiCPExceptionBase
    class MIRAICP_EXPORT MiraiCPExceptionBase : public ::std::exception {
    protected:
        using string = std::string;

    protected:
        /// @brief 异常内容
        string cause;


    public:
        /// @brief 发生异常的文件名
        string filename;

        /// @brief 发生异常的行号
        int lineNum = 0;

    protected:
        /// 受保护构造函数，供子类调用
        MiraiCPExceptionBase(const nlohmann::json &info, string _filename, int _lineNum) : cause(info.dump()), filename(std::move(_filename)), lineNum(_lineNum) {}

    public:
        ~MiraiCPExceptionBase() override = default;

    public:
        /// 异常信息
        const char *what() const noexcept override { return cause.c_str(); }

        /// 返回std::string的异常信息
        string getError() const { return cause; }

        /// 返回解析后的异常信息
        nlohmann::json getCause() const { return nlohmann::json::parse(cause); }

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
    template<typename T>
    class MIRAICP_EXPORT MiraiCPExceptionCRTP : public MiraiCPExceptionBase {
    public:
        /// 委托构造函数
        explicit MiraiCPExceptionCRTP(nlohmann::json info, string _filename, int _lineNum) : MiraiCPExceptionBase(std::move(info), std::move(_filename), _lineNum) {
        }

    public:
        // CRTP类型获取实现
        string getExceptionType() const override { return T::exceptionType(); }
    };

    /// @brief 通用MiraiCP异常
    /// @param const string &description, string _filename, int _lineNum
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT MiraiCPException : public MiraiCPExceptionCRTP<MiraiCPExceptionBase> {};

    EXCEPTION_DEFINE(BotPermException, (nlohmann::json{{"cause", "没有权限执行该操作"}}));
    EXCEPTION_DEFINE_W_STR(BotNotFoundException, (nlohmann::json{{"cause", "找不到bot"}, {"botId", info}}));
    EXCEPTION_DEFINE_W_NUM(BotIsBeingMutedException, (nlohmann::json{{"cause", "发送信息失败, bot已被禁言"}, {"timeRemain", info}}));
    /// 事件被取消, 一般出现在发送消息时在preSendMessageEvent取消的时候抛出
    EXCEPTION_DEFINE(EventCancelledException, (nlohmann::json{{"cause", "发送信息被取消"}}));
    EXCEPTION_DEFINE(FriendException, (nlohmann::json{{"cause", "找不到好友"}}));
    /// 获取群错误
    EXCEPTION_DEFINE(GroupException, (nlohmann::json{{"cause", "找不到群"}}));
    EXCEPTION_DEFINE(MemberGroupNotFoundException, (nlohmann::json{{"cause", "获取群成员错误:找不到群"}}));
    EXCEPTION_DEFINE(MemberNotFoundException, (nlohmann::json{{"cause", "获取群成员错误:找不到群成员"}}));
    /// 内部异常, 通常为json读写问题
    EXCEPTION_DEFINE_W_STR(APIException, (nlohmann::json{{"cause", "MiraiCP内部无法预料的错误"}, {"info", info}}));
    EXCEPTION_DEFINE_W_STR(IllegalArgumentException, (nlohmann::json{{"cause", "参数错误"}, {"info", info}}));
    /// 通常为Mirai返回
    EXCEPTION_DEFINE_W_STR(IllegalStateException, (nlohmann::json{{"cause", "状态错误"}, {"info", info}}));
    /// 远程资源出现问题
    EXCEPTION_DEFINE_W_STR(RemoteAssetException, (nlohmann::json{{"cause", "远程资源出现问题"}, {"info", info}}));
    EXCEPTION_DEFINE_W_STR(TimeOutException, (nlohmann::json{{"cause", "执行超时"}, {"info", info}}));
    /// 文件读取异常
    EXCEPTION_DEFINE_W_STR(UploadException, (nlohmann::json{{"cause", "上传(图片/文件)异常"}, {"info", info}}));
    /// 插件没有权限时抛出该异常
    /// 该异常仅可能在插件尝试调用libLoader 高级权限的Api接口时抛出
    /// 如插件尝试重载、加载、卸载插件等操作，但配置文件中并没有赋予该插件权限时
    EXCEPTION_DEFINE(PluginNotAuthorizedException, (nlohmann::json{{"cause", "插件" + CPPPlugin::config.getId() + "没有管理权限"}}));
    /// 插件未加载抛出该异常
    /// 在插件能正常运行时不会抛出，出现该异常事件时请不要再次尝试收发消息等Mirai操作，
    /// 否则可能导致异常处理时再次抛出异常
    EXCEPTION_DEFINE(PluginNotEnabledException, (nlohmann::json{{"cause", "插件" + CPPPlugin::config.getId() + "未加载"}}));
    /// LoaderAPI返回空Msg unique指针
    EXCEPTION_DEFINE(LibLoaderNoResponseException, (nlohmann::json{{"cause", "LibLoader返回了空的响应"}}));
    /// 禁言异常
    EXCEPTION_DEFINE(MuteException, (nlohmann::json{{"cause", "禁言时长不在0s~30d中间"}}));
    /// 撤回异常
    EXCEPTION_DEFINE(RecallException, (nlohmann::json{{"cause", "该消息已经被撤回"}}));

    /// 如果在 MiraiCPNewThread 中捕获到了非 MiraiCP 之外的异常抛出
    /// @see MiraiCPNewThread
    class MiraiCPThreadException : public MiraiCPExceptionCRTP<MiraiCPThreadException> {
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
        MIRAICP_EXPORT static std::string getThreadIdStr(const std::thread::id &id) {
            static std::stringstream ss;
            ss << id;
            auto result = ss.str();
            ss.str("");
            return result;
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_EXCEPTION_H
