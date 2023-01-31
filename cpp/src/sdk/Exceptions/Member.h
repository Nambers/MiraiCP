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

#ifndef MIRAICP_PRO_EXCEPTION_MEMBER_H
#define MIRAICP_PRO_EXCEPTION_MEMBER_H

#include "Exception.h"


namespace MiraiCP {
    /// 获取群成员错误
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT MemberException : public MiraiCPExceptionCRTP<MemberException> {
    public:
        enum MemberExceptionType : int {
            OtherType,
            NoSuchGroup,
            NoSuchMember
        };
        MemberExceptionType type = OtherType;

        /*
        *   "1" - 找不到群
        *	"2" - 找不到群成员
        */
        explicit MemberException(int _type, string _filename, int _lineNum) : MiraiCPExceptionCRTP(
                                                                                      [&]() -> string {
                                                                                          type = MemberExceptionType(_type);
                                                                                          switch (type) {
                                                                                              case NoSuchGroup:
                                                                                                  return "找不到群";
                                                                                              case NoSuchMember:
                                                                                                  return "找不到群成员";
                                                                                              default:
                                                                                                  return "";
                                                                                          }
                                                                                      }(),
                                                                                      std::move(_filename), _lineNum) {}

        static string exceptionType() { return "MemberException"; }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_EXCEPTION_MEMBER_H
