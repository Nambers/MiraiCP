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

#ifndef MIRAICP_PRO_EXCEPTION_UPLOAD_H
#define MIRAICP_PRO_EXCEPTION_UPLOAD_H

#include "Exception.h"


namespace MiraiCP {
    /// 文件读取异常.
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT UploadException : public MiraiCPExceptionCRTP<UploadException> {
    public:
        explicit UploadException(const std::string &text, string _filename, int _lineNum) : MiraiCPExceptionCRTP("上传(图片/文件)异常" + text, std::move(_filename), _lineNum) {}

        static std::string exceptionType() { return "UploadException"; }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_EXCEPTION_UPLOAD_H
