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

#ifndef MIRAICP_PRO_EXCEPTION_MUTE_H
#define MIRAICP_PRO_EXCEPTION_MUTE_H

#include "Exception.h"


namespace MiraiCP {
    /// 禁言异常
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT MuteException : public MiraiCPExceptionCRTP<MuteException> {
    public:
        /*
        *	 禁言时间超出0s~30d
        */
        MuteException(string _filename, int _lineNum) : MiraiCPExceptionCRTP("禁言时长不在0s~30d中间", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "MuteException"; }
    };
} // namespace MiraiCP
#endif //MIRAICP_PRO_EXCEPTION_MUTE_H
