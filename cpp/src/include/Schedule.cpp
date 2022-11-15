//
// Created by antares on 11/10/22.
//

#include "Schedule.h"
#include "Event.h"
#include "ThreadTask.h"


void MiraiCP::schedule(size_t time, std::string msg) {
    schedule(std::chrono::seconds(time), std::move(msg));
}

// todo(Antares): 之后改为优先队列，轮询，降低线程池的占用
void MiraiCP::schedule(std::chrono::seconds sec, std::string msg) {
    ThreadTask::addTask(
            [sec](std::string t) {
                std::this_thread::sleep_for(sec);
                Event::broadcast(TimeOutEvent(std::move(t)));
            },
            std::move(msg));
}
