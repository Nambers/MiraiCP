//
// Created by antares on 11/10/22.
//

#ifndef MIRAICP_PRO_SCHEDULE_H
#define MIRAICP_PRO_SCHEDULE_H

#include <chrono>
#include <string>


namespace MiraiCP {
    /*!
     * @brief 定时任务, 在一定时间后广播**一次**TimeOutEvent
     * @param sec 在多少秒后执行
     * @param msg 附加的string类型信息
     * @doxygenEg{1017, Schedule.h, 定时任务}
     */
    void schedule(size_t sec, std::string msg);

    /*!
     * @brief 定时任务, 在一定时间后广播**一次**TimeOutEvent，chrono重载
     * @param sec 在多少秒后执行
     * @param msg 附加的string类型信息
     * @doxygenEg{1017, Schedule.h, 定时任务}
     */
    void schedule(std::chrono::seconds sec, std::string msg);
} // namespace MiraiCP


#endif //MIRAICP_PRO_SCHEDULE_H
