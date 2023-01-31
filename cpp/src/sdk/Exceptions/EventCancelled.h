// Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_EXCEPTION_EVENTCANCELLED_H
#define MIRAICP_PRO_EXCEPTION_EVENTCANCELLED_H

#include "Exception.h"


namespace MiraiCP {
    /// 事件被取消, 一般出现在发送消息时在preSendMessageEvent取消的时候抛出
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT EventCancelledException : public MiraiCPExceptionCRTP<EventCancelledException> {
    public:
        explicit EventCancelledException(const string &msg, string _filename, int _lineNum) : MiraiCPExceptionCRTP(msg, std::move(_filename), _lineNum) {}

        static string exceptionType() { return "EventCancelledException"; }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_EXCEPTION_EVENTCANCELLED_H
