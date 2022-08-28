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

#ifndef MIRAICP_PRO_REDIRECTCOUT_H
#define MIRAICP_PRO_REDIRECTCOUT_H


#include "MiraiCPMacros.h"
#include <sstream>

namespace MiraiCP::Redirector {
    MIRAICP_EXPORT void setRedirectedObjs(std::ostream *, std::ostream *);
    /// start redirecting cout and cerr
    MIRAICP_EXPORT void start();
    /// reset all redirecting
    MIRAICP_EXPORT void reset();
} // namespace MiraiCP
#endif //MIRAICP_PRO_REDIRECTCOUT_H
