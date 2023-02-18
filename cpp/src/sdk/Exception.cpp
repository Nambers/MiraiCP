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

#include "Exceptions/API.h"
#include "Exceptions/Member.h"
#include "Exceptions/TimeOut.h"
#include "Logger.h"


namespace MiraiCP {
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
        explicit BotIsBeingMutedException(int t, string _filename, int _lineNum) : MiraiCPExceptionCRTP(
                                                                                           "发送信息失败, bot已被禁言, 剩余时间" + std::to_string(t), std::move(_filename), _lineNum),
                                                                                   timeRemain(t) {}

        static string exceptionType() { return "BotIsBeingMutedException"; }
    };

    /// 事件被取消, 一般出现在发送消息时在preSendMessageEvent取消的时候抛出
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT EventCancelledException : public MiraiCPExceptionCRTP<EventCancelledException> {
    public:
        explicit EventCancelledException(const string &msg, string _filename, int _lineNum) : MiraiCPExceptionCRTP(msg, std::move(_filename), _lineNum) {}

        static string exceptionType() { return "EventCancelledException"; }
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

    void MiraiCPExceptionBase::basicRaise() const {
        Logger::logger.error(this->what());
    }

    void MiraiCPExceptionBase::raise() const {
        this->basicRaise();
        if (!filename.empty() && lineNum != 0) {
            Logger::logger.error("文件名:" + filename + "\n行号:" + std::to_string(lineNum));
        }
    }

    void ErrorHandle0(const std::string &name, int line, const std::string &re, const std::string &ErrorMsg) {
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
