// Copyright (c) 2022. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_COMMONTOOLS_H
#define MIRAICP_PRO_COMMONTOOLS_H

#include "cassert"
#ifndef assert
#define assert(x) void(0)
#endif

#include <functional>

#define MIRAICP_EXCEPTION_WHERE __FILE__, __LINE__

#define MiraiCP_defer(code)                              \
    auto __defered_statement_wrapper__ = [&]() { code }; \
    CommonTools::MiraiCPDefer __defered_object__(__defered_statement_wrapper__)

#define MiraiCP_defer_lambda(lambda)             \
    auto __defered_statement_wrapper__ = lambda; \
    CommonTools::MiraiCPDefer __defered_object__(__defered_statement_wrapper__)

namespace CommonTools {
    /// defer class
    /// @see MiraiCP_defer
    class MiraiCPDefer {
    public:
        std::function<void()> defer_func;

        MiraiCPDefer(std::function<void()> &&func) : defer_func(std::move(func)) {
        }

        virtual ~MiraiCPDefer() {
            defer_func();
        }
    };
} // namespace CommonTools
#endif //MIRAICP_PRO_COMMONTOOLS_H
