// Copyright (c) 2021-2021. Eritque arcus and contributors.
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
#include "MiraiCode.h"
#include "Tools.h"

namespace MiraiCP {
    std::string MiraiCode::toString() {
        return Tools::escapeFromMiraiCode(this->content);
    }
    MiraiCode::MiraiCode(const std::string &a, bool convert) { // NOLINT(google-explicit-constructor)
        if (!convert)
            content = a;
        else
            content = Tools::escapeToMiraiCode(a);
    }
} // namespace MiraiCP