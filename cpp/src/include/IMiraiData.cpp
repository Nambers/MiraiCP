//
// Created by 60168 on 2022-9-20.
//

#include "IMiraiData.h"
#include "LowLevelAPI.h"
#include "json.hpp"
#include <mutex>

namespace MiraiCP {
    void IMiraiData::request_refresh() {
        if (!MiraiCP::checkSafeCall() || Locker._inited.exchange(true)) return;

        std::unique_lock<std::shared_mutex> _lck(Locker._mtx);
        refreshInfo();
    }

    void IMiraiData::force_refresh_nexttime() {
        Locker._inited = false;
    }

    std::string IMiraiData::toString() const {
        return toJson().dump();
    }
} // namespace MiraiCP
