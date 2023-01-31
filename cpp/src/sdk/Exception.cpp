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
#include "Exceptions/Bot.h"
#include "Exceptions/BotIsBeingMuted.h"
#include "Exceptions/EventCancelled.h"
#include "Exceptions/Friend.h"
#include "Exceptions/Group.h"
#include "Exceptions/Member.h"
#include "Exceptions/TimeOut.h"
#include "Logger.h"


namespace MiraiCP {
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
