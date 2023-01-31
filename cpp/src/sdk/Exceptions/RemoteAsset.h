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

#ifndef MIRAICP_PRO_EXCEPTION_REMOTEASSET_H
#define MIRAICP_PRO_EXCEPTION_REMOTEASSET_H

#include "Exception.h"


namespace MiraiCP {
    /// 远程资源出现问题
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT RemoteAssetException : public MiraiCPExceptionCRTP<RemoteAssetException> {
    public:
        explicit RemoteAssetException(const string &e, string _filename, int _lineNum) : MiraiCPExceptionCRTP(e, std::move(_filename), _lineNum) {}

        static string exceptionType() { return "RemoteAssetException"; }
    };
}

#endif //MIRAICP_PRO_EXCEPTION_REMOTEASSET_H
