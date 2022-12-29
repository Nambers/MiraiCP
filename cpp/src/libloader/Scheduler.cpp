//
// Created by antares on 11/11/22.
//

#include "Scheduler.h"
#include "PluginListManager.h"
#include "json.hpp"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>


using timepoint = decltype(std::chrono::system_clock::now());

struct scheduleTask {
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
}

namespace LibLoader::Scheduler {
    static std::priority_queue<scheduleTask, std::vector<scheduleTask>, std::greater<scheduleTask>> timerQueue; // NOLINT(modernize-use-transparent-functors)
    static std::mutex mtx;

    void timer(std::string pluginId, std::string content, size_t timeInSecond) {
        auto scheduleTime = std::chrono::system_clock::now() + std::chrono::seconds(timeInSecond);
        scheduleTask a{scheduleTime, std::move(pluginId), std::move(content)};
        std::lock_guard lk(mtx);
        timerQueue.push(std::move(a));
        loaderWakeCV().notify_one();
    }

    inline void sendTimeoutEvent(const std::string &pluginId, const std::string &content) noexcept {
        nlohmann::json j{{"type", 10}, {"msg", content}};
        PluginListManager::broadcastToOnePlugin(pluginId, j.dump());
    }

    void popSchedule() noexcept {
        std::lock_guard lk(mtx);
        while (!timerQueue.empty() && timerQueue.top().tp < std::chrono::system_clock::now()) {
            auto task = timerQueue.top();
            timerQueue.pop();
            sendTimeoutEvent(task.pluginId, task.content);
        }
    }

    bool empty() noexcept {
        return timerQueue.empty();
    }
} // namespace LibLoader::Scheduler