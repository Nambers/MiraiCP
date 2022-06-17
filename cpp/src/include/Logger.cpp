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

#include "Logger.h"
#include "CPPPlugin.h"
#include "KtOperation.h"
#include "MiraiCode.h"
#include "Tools.h"
#include "loaderApi.h"

extern "C" {
extern MiraiCP::PluginConfig PLUGIN_INFO;
}
namespace MiraiCP {
    using json = nlohmann::json;
    // 静态成员
    Logger Logger::logger = Logger();

    // 结束静态成员

    /*
    日志类实现
    throw: InitException 即找不到签名
    */
    void Logger_interface::init(Logger_interface::logMethodType logMethod1) {
        logMethod = logMethod1;
    }

    void Logger::log1(const std::string &content, int level, JNIEnv *env) {
        LibLoader::LoaderApi::loggerInterface(content, MiraiCP::CPPPlugin::config.name, -1, level);
        // LibLoader::LoaderApi::loggerInterface(content, "", -2, level);
    }

    void IdLogger::log1(const std::string &content, int level, JNIEnv *env) {
        LibLoader::LoaderApi::loggerInterface(content, "", static_cast<long long>(id), level);
    }
} // namespace MiraiCP
