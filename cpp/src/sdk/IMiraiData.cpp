// Copyright (c) 2022 - 2022. Eritque arcus and contributors.
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

#include "IMiraiData.h"
#include "LowLevelAPI.h"
#include "json.hpp"
#include <mutex>


namespace MiraiCP {
    void IMiraiData::requestRefresh() {
        if (!MiraiCP::checkSafeCall() || Locker._inited.exchange(true)) return;

        std::unique_lock<std::shared_mutex> _lck(Locker._mtx);
        refreshInfo();
    }

    void IMiraiData::forceRefreshNextTime() {
        Locker._inited = false;
    }

    std::string IMiraiData::toString() const {
        return toJson().dump();
    }

    nlohmann::json IMiraiData::toJson() const {
        std::shared_lock<std::shared_mutex> lck(Locker._mtx);
        return internalToJson();
    }

    nlohmann::json IMiraiData::internalToString() const {
        return internalToJson().dump();
    }
} // namespace MiraiCP
