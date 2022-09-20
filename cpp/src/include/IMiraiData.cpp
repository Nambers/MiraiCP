//
// Created by 60168 on 2022-9-20.
//

#include "IMiraiData.h"
#include "LowLevelAPI.h"

namespace MiraiCP {
    void IMiraiData::request_refresh() {
        if (!MiraiCP::checkSafeCall() || Locker._inited.exchange(true)) return;

        std::unique_lock<std::shared_mutex> _lck(Locker._mtx);
        refreshInfo();
    }

    void IMiraiData::force_refresh() {
         Locker._inited = false;
    }
} // namespace MiraiCP
