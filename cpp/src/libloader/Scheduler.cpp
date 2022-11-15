//
// Created by antares on 11/11/22.
//

#include "Scheduler.h"
#include "json.hpp"
#include <chrono>
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

namespace LibLoader::Scheduler {
    static std::priority_queue<scheduleTask, std::vector<scheduleTask>, std::greater<scheduleTask>> timerQueue; // NOLINT(modernize-use-transparent-functors)
    static std::mutex mtx;
    void addSchedule(std::string pluginId, size_t timeInSecond, std::string content) {
        auto scheduleTime = std::chrono::system_clock::now() + std::chrono::seconds(timeInSecond);
        scheduleTask a{scheduleTime, std::move(pluginId), std::move(content)};
        std::lock_guard lk(mtx);
        timerQueue.push(std::move(a));
    }

    inline void sendTimeoutEvent(const std::string &pluginId, std::string content) {
        // nlohmann::json j{{"msg", std::move(content)}};
        //        LibLoader::ThreadController::getController().submitJob(pluginId, [](){
        //            cfg.eventFunc(j.dump());
        //        });
        // todo(Antares): finish this
    }

    void popSchedule() {
        std::lock_guard lk(mtx);
        while (!timerQueue.empty() && timerQueue.top().tp < std::chrono::system_clock::now()) {
            auto task = timerQueue.top();
            timerQueue.pop();
            sendTimeoutEvent(task.pluginId, std::move(task.content));
        }
    }
} // namespace LibLoader::Scheduler