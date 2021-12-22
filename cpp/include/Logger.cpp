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

#include "Logger.h"
#include "CPPPlugin.h"
#include "Config.h"
#include "MiraiCode.h"
#include "Tools.h"
#include "ThreadManager.h"

namespace MiraiCP {
    using json = nlohmann::json;
    // 静态成员
    Logger Logger::logger = Logger();
    /// @deprecated use Logger::logger, 用单例模式, since v2.8.1
    [[deprecated("Use Logger::logger instead")]] Logger *const logger = &Logger::logger;

    // 结束静态成员

    /*
    日志类实现
    throw: InitException 即找不到签名
    */
    void Logger_interface::init(JNIEnv *env) {
        if (env == nullptr) env = ThreadManager::getEnv();
        this->log = env->GetStaticMethodID(Config::CPPLib, "KSendLog", "(Ljava/lang/String;I)V");
    }


    void Logger_interface::log0(const std::string &content, int level, json j, JNIEnv *env) {
        if (env == nullptr) env = ThreadManager::getEnv();
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        env->CallStaticVoidMethod(Config::CPPLib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }

    void Logger::log1(const std::string &content, int level, JNIEnv *env) {
        json j;
        j["id"] = -2;
        j["log"] = content;
        log0(content, level, j, env);
    }

    void IdLogger::log1(const std::string &content, int level, JNIEnv *env) {
        json j;
        j["id"] = id;
        j["log"] = content;
        log0(content, level, j, env);
    }

    void PluginLogger::log1(const std::string &content, int level, JNIEnv *env) {
        json j;
        j["id"] = -1;
        j["name"] = CPPPlugin::plugin->config.id;
        j["log"] = content;
        log0(content, level, j, env);
    }
} // namespace MiraiCP
