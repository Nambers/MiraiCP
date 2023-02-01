// Copyright (c) 2023. Eritque arcus and contributors.
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

#include "Scheduler.h"
#include "LoaderExceptions.h"
#include "PluginListManager.h"
#include "json.hpp"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>


using timepoint = decltype(std::chrono::system_clock::now());

struct scheduleTask {
    timepoint createTime;
    timepoint tp;
    std::string pluginId;
    std::string content;
};

template<>
struct std::greater<scheduleTask> {
    bool operator()(const scheduleTask &a, const scheduleTask &b) {
        return std::greater<>()(a.tp, b.tp);
    }
};

namespace LibLoader {
    std::condition_variable &loaderWakeCV();
    std::mutex &loaderCVLock();
} // namespace LibLoader

namespace LibLoader::Scheduler {
    static std::priority_queue<scheduleTask, std::vector<scheduleTask>, std::greater<scheduleTask>> timerQueue; // NOLINT(modernize-use-transparent-functors)
    static std::mutex mtx;

    void timer(std::string pluginId, std::string content, size_t timeInSecond) {
        auto timenow = std::chrono::system_clock::now();
        auto scheduleTime = timenow + std::chrono::seconds(timeInSecond);
        scheduleTask a{timenow, scheduleTime, std::move(pluginId), std::move(content)};
        {
            std::lock_guard lk(mtx);
            timerQueue.push(std::move(a));
        }
        {
            loaderWakeCV().notify_one();
        }
    }

    inline void sendTimeoutEvent(const std::string &pluginId, const std::string &content) noexcept {
        nlohmann::json j{{"eventId", 15},
                         {"eventData", nlohmann::json{{"msg", content}}}};
        PluginListManager::broadcastToOnePlugin(pluginId, j.dump());
    }

    bool checkValidTimeStamp(const scheduleTask &task) {
        return task.createTime > PluginListManager::getPluginTimeStamp(task.pluginId);
    }

    void popSchedule() noexcept {
        std::lock_guard lk(mtx);
        while (timeup()) {
            auto &task = timerQueue.top();
            try {
                if (checkValidTimeStamp(task)) {
                    sendTimeoutEvent(task.pluginId, task.content);
                }
            } catch (const PluginNotLoadedException &e) {
                logger.error("未加载的插件传入了定时任务，任务失效");
                e.raise();
            }
            timerQueue.pop();
        }
    }

    bool timeup() noexcept {
        return !timerQueue.empty() && timerQueue.top().tp < std::chrono::system_clock::now();
    }
} // namespace LibLoader::Scheduler
