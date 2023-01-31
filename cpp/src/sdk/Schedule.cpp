//
// Created by antares on 11/10/22.
//

#include "Schedule.h"
#include "CPPPlugin.h"
#include "Event.h"
#include "loaderApiInternal.h"


void MiraiCP::schedule(size_t time, const std::string &msg) {
    LibLoader::LoaderApi::timer(CPPPlugin::config.id, msg, time);
}

void MiraiCP::schedule(std::chrono::seconds sec, const std::string &msg) {
    schedule(sec.count(), msg);
}
