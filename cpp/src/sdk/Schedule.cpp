//
// Created by antares on 11/10/22.
//

#include "Schedule.h"
#include "CPPPlugin.h"
#include "Event.h"
#include "ThreadTask.h"


void MiraiCP::schedule(size_t time, const std::string &msg) {
    LibLoader::LoaderApi::timer(CPPPlugin::config.id, msg, time);
}

// todo(Antares): 之后改为优先队列，轮询，降低线程池的占用
void MiraiCP::schedule(std::chrono::seconds sec, const std::string &msg) {
    schedule(sec.count(), msg);
}
