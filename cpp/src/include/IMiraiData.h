//
// Created by 60168 on 2022-9-20.
//

#ifndef MIRAICP_PRO_IMIRAIDATA_H
#define MIRAICP_PRO_IMIRAIDATA_H

#include "json_fwd.hpp"
#include <shared_mutex>

namespace MiraiCP {
    class IMiraiData;

    class IMiraiData {
        struct MiraiDataLocker final {
            std::shared_mutex _mtx;
            std::atomic_bool _inited = false;
        };

    protected:
        MiraiDataLocker Locker;

    public:
        IMiraiData() = default;
        virtual ~IMiraiData() = default;

    public:
        virtual void deserialize(nlohmann::json in_json) = 0;

    public:
        void request_refresh();
        void force_refresh();

    private:
        virtual void refreshInfo() = 0;
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_IMIRAIDATA_H
