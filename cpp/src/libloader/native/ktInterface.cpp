// Copyright (c) 2022 - 2022. Antares, Eritque arcus and contributors.
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

/// 本文件的全部函数实现都是kt线程调用的函数


#include "ktInterface.h"
#include "LoaderLogger.h"
#include "PluginListManager.h"
#include "ThreadController.h"
#include "eventHandle.h"
#include "loaderMain.h"
#include "redirectCout.h"


namespace LibLoader {
    void registerAllPlugin(const std::string &) noexcept;

    std::thread loaderThread;
} // namespace LibLoader


/// 实际初始化函数
/// 1. 设置全局变量
/// 2. 开启loader线程并获取插件入口函数的返回值
void VerifyImpl(const char *_version, const char *_cfgPath) {
    //初始化日志模块
    MiraiCP::Redirector::start();

    LibLoader::logger.info("⭐libLoader 版本: " + MiraiCP::MiraiCPVersion);
    auto version = "v" + std::string(_version);
    if (version != MiraiCP::MiraiCPVersion) {
        LibLoader::logger.warning(
                "libLoader(" + MiraiCP::MiraiCPVersion + ")版本和MiraiCP启动器(" + version + ")不一致, 建议更新至最新");
    }

    // 测试有效的插件
    LibLoader::registerAllPlugin(std::string(_cfgPath));

    // 激活插件。创建loader thread。
    // loader thread中创建多线程加载所有插件，调用入口函数
    LibLoader::loaderThread = std::thread(LibLoader::LoaderMain::loaderMain);
}

/// 事件广播，由kt（主）线程调用
/// 广播过程中应当锁住 plugin list，以防止内存访问冲突
/// 广播给插件的过程中由插件线程完成所有任务
/// 插件线程无法给plugin list加锁，因为插件端只能向loader线程发送某个任务的申请
/// loader线程可能会尝试获取 plugin list 的锁，
/// 但Event函数在派发任务后是会立刻退出并释放锁的，
/// 不会造成死锁
void EventImpl(const char *content) {
    static std::string str;

    std::lock_guard lk(LibLoader::PluginListManager::getLock());
    str = std::string(content);

    if (str.find(R"("type":1000)") != std::string::npos) {
        LibLoader::builtInCommand(str);
        return;
    }

    // static lambda，不可以捕获参数！str被声明为static了会被自动捕获
    static std::function broadcast_func = [](const LibLoader::LoaderPluginConfig &cfg) {
        if (cfg.handle && cfg.enabled) {
            LibLoader::ThreadController::getController().submitJob(cfg.getId(), [&cfg]() {
                cfg.eventFunc(str);
            });
        }
    };

    LibLoader::PluginListManager::run_over_pluginlist(broadcast_func);
}

void PluginDisableImpl() {
    LibLoader::LoaderMain::loaderExit();
    LibLoader::loaderThread.join();
}
