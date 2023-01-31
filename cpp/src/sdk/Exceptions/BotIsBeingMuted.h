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

#ifndef MIRAICP_PRO_EXCEPTION_BOTISBEINGMUTED_H
#define MIRAICP_PRO_EXCEPTION_BOTISBEINGMUTED_H


#include "Exception.h"


namespace MiraiCP {
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
}
#endif //MIRAICP_PRO_EXCEPTION_BOTISBEINGMUTED_H
