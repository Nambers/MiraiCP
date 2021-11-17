// Copyright (c) 2021-2021. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_UTILS_H
#define MIRAICP_PRO_UTILS_H

#include "CPPPlugin.h"
#include "Config.h"
#include <string>

namespace MiraiCP {
    const std::string MiraiCPVersion = "v2.8.0";

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
