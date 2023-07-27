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

//
// Created by antares on 11/11/22.
//

#ifndef MIRAICP_PRO_SCHEDULER_H
#define MIRAICP_PRO_SCHEDULER_H

#include <chrono>
#include <string>


namespace LibLoader::Scheduler {
    /// @brief libLoader线程tick调用，弹出所有计时器
    void popSchedule() noexcept;

    /// @brief 新增一个计时器，对接libLoader API
    void timer(std::string id, std::string content, size_t timeInSec);

    /// @brief 获取schduler是否空
    bool timeup() noexcept;
} // namespace LibLoader::Scheduler


#endif //MIRAICP_PRO_SCHEDULER_H
