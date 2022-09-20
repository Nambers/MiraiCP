//
// Created by 60168 on 2022-9-20.
//

#ifndef MIRAICP_PRO_IMIRAIDATA_H
#define MIRAICP_PRO_IMIRAIDATA_H

#include "json_fwd.hpp"
#include <shared_mutex>

namespace MiraiCP {
    struct IMiraiData;

    struct IMiraiData {
    private:
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
        void request_refresh();

        /*!
         * @brief 强制下次request_refresh()时进行refresh，
         * @note 调用该函数本身不会进行refresh
         * @see request_refresh
         */
        void force_refresh_nexttime();

        /*!
         * @brief 序列化为string
         */
        std::string toString() const;

        std::shared_mutex &get_mutex() {
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
