#ifndef MIRAICP_PRO_UTILS_H
#define MIRAICP_PRO_UTILS_H

#include "CPPPlugin.h"
#include "Config.h"
#include <string>

namespace MiraiCP {
    const std::string MiraiCPVersion = "v2.8.0-M2";

    inline void schedule(long time, const std::string &msg, JNIEnv *env = ThreadManager::getEnv()) {
        json j;
        j["time"] = time;
        j["msg"] = msg;
        Config::koperation(Config::TimeOut, j, env);
    }
    void enrollPlugin();

    inline void enrollPlugin0(CPPPlugin *p) {
        CPPPlugin::plugin = p;
    }
} // namespace MiraiCP

#endif //MIRAICP_PRO_UTILS_H
