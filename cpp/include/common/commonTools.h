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


#include "MiraiCPMacros.h"
// -----------------------
#include <functional>


namespace CommonTools {
    /// defer class
    /// @see MIRAICP_DEFER
    class MiraiCPDefer {
    public:
        std::function<void()> defer_func;

        MiraiCPDefer(std::function<void()> func) : defer_func(std::move(func)) {
        }

        ~MiraiCPDefer() {
            defer_func();
        }
    };
} // namespace CommonTools
#endif //MIRAICP_PRO_COMMONTOOLS_H
