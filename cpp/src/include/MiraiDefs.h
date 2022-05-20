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

#ifndef MIRAICP_PRO_MIRAIDEFS_H
#define MIRAICP_PRO_MIRAIDEFS_H

// #define MiraiCPThrow(x) throw x.append(__FILE__, __LINE__)
#define ErrorHandle(x, y) ErrorHandle0(__FILE__, __LINE__, (x), (y))
#define MIRAICP_EXCEPTION_WHERE __FILE__, __LINE__
#if defined(_MSC_VER)
#define ShouldNotUse(msg) _Pragma("warning(error:4996)") [[deprecated(msg)]] _Pragma("warning(warning:4996)")
#else // MSVC
#if defined(__GNUC__)
#define ShouldNotUse(msg) [[deprecated(msg)]] __attribute__((error(msg)))
#else // GUNC
#define ShouldNotUse(msg)
#endif // ShouldNotUse
#endif

#include <string>

namespace MiraiCP {
    using QQID = unsigned long long;
    const std::string MiraiCPVersion = "v2.11.0";
} // namespace MiraiCP

#endif //MIRAICP_PRO_MIRAIDEFS_H
