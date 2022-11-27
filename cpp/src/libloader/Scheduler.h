//
// Created by antares on 11/11/22.
//

#ifndef MIRAICP_PRO_SCHEDULER_H
#define MIRAICP_PRO_SCHEDULER_H

#include <chrono>
#include <string>


namespace LibLoader::Scheduler {
    /// @brief libLoader线程tick调用，弹出所有计时器
    void popSchedule();

    /// @brief 新增一个计时器，对接libLoader API
    void timer(std::string id, std::string content, size_t timeInSec);
} // namespace LibLoader::Scheduler


#endif //MIRAICP_PRO_SCHEDULER_H
