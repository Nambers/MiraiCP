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

#ifndef MIRAICP_PRO_IMIRAIDATA_H
#define MIRAICP_PRO_IMIRAIDATA_H

#include "json_fwd.hpp"
#include <atomic>
#include <shared_mutex>

namespace MiraiCP {
    struct IMiraiData {
    private:
        struct MiraiDataLocker final {
            std::shared_mutex _mtx;
            std::atomic<bool> _inited = false;
        };

    protected:
        MiraiDataLocker Locker;

    public:
        IMiraiData() = default;

        virtual ~IMiraiData() = default;

    public:
        /*!
         * @brief 将json数据读入，由子类实现
         */
        virtual void deserialize(nlohmann::json in_json) = 0;

        /*!
         * @brief 转为json，由子类实现
         */
        virtual nlohmann::json toJson() const = 0;

    public:
        /*!
         * @brief 请求一次refresh，但不一定会进行
         */
        void requestRefresh();

        /*!
         * @brief 强制下次request_refresh()时进行refresh，
         * @note 调用该函数本身不会进行refresh
         * @see requestRefresh
         */
        void forceRefreshNextTime();

        /*!
         * @brief 序列化为string
         */
        std::string toString() const;

        std::shared_mutex &getMutex() {
            return Locker._mtx;
        }

    private:
        /*!
         * @brief 刷新数据，由子类实现
         */
        virtual void refreshInfo() = 0;
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_IMIRAIDATA_H
