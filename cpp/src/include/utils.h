// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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
namespace MiraiCP {
    /*!
     * @brief 定时任务, 在一定时间后广播**一次**TimeOutEvent
     * @param time 在多少毫秒后执行
     * @param msg 附加的string类型信息
     * @doxygenEg{1017, schedule.cpp, 定时任务}
     */
    inline void schedule(long time, const std::string &msg, JNIEnv *env = nullptr) {
        nlohmann::json j;
        j["time"] = time;
        j["msg"] = msg;
        Config::koperation(Config::TimeOut, j, env);
    }
    /// 注册插件函数, 需要被实现, 类似onStart();
    void enrollPlugin();

    /// 用指针绑定插件
    inline void enrollPlugin0(CPPPlugin *p) {
        CPPPlugin::plugin = p;
    }
} // namespace MiraiCP
extern "C" {
void Event(std::string content);
void PluginDisable();
void Init(JavaVM *gvm, jclass, jmethodID logger, jmethodID);
}

#endif //MIRAICP_PRO_UTILS_H