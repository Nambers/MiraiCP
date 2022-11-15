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
    /// 声明 Contact 类数据安全锁接口以及部分实现的抽象类
    struct IMiraiData {
    private:
        /// 锁
        struct MiraiDataLocker final {
            /// 该数据是否上锁
            std::shared_mutex _mtx;
            /// 该数据是否已经初始化
            std::atomic<bool> _inited = false;
        };

    protected:
        /// 锁实例
        mutable MiraiDataLocker Locker;

    public:
        IMiraiData() = default;

        virtual ~IMiraiData() = default;

    public:
        /// 转为json, 由子类实现, 多线程安全(带锁)
        nlohmann::json toJson() const;

    protected:
        /// 读入json数据, 由子类实现, 无锁
        virtual void deserialize(nlohmann::json in_json) = 0;

        /// 转为json, 由子类实现, 无锁
        virtual nlohmann::json internalToJson() const = 0;

        /**
         * 转为json，internalToJson 的具体实现决定, 无锁
         * @see internalToJson
         */
        nlohmann::json internalToString() const;

    public:
        /// 请求一次刷新数据, 但不保证会进行
        void requestRefresh();

        /*!
         * @brief 确保下次调用 requestRefresh 时刷新数据
         * @note 调用该函数本身不会刷新数据
         * @see requestRefresh
         */
        void forceRefreshNextTime();

        /// 序列化为string, 带锁
        std::string toString() const;

        /// 获取锁
        std::shared_mutex &getMutex() {
            return Locker._mtx;
        }

    private:
        /// 刷新数据, 由子类实现
        virtual void refreshInfo() = 0;
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_IMIRAIDATA_H
