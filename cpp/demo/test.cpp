
// Copyright (c) 2022. Eritque arcus and contributors.
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

// MiraiCP依赖文件(只需要引入这一个)
#include <MiraiCP.hpp>
using namespace MiraiCP;

const PluginConfig CPPPlugin::config{
        "id1",          // 插件id
        "test",         // 插件名称
        "v1.0",         // 插件版本
        "a",            // 插件作者
        "测试中文描述", // 可选：插件描述
                        // 可选：日期
};

// 测试全局变量构造
Member mem(nlohmann::json{{"id", 1234567}, {"botid", 1245634}, {"groupid", 123456}});
Member mem2(123, 456, 789);

// test

size_t test_task(std::chrono::seconds sleeptime) {
    std::this_thread::sleep_for(sleeptime);
    Logger::logger.info("Wake!");
    return sleeptime.count();
}

void test_task2() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Logger::logger.info("Wake2!");
}

class Main : public CPPPlugin {
public:
    // 配置插件信息
    Main() : CPPPlugin() {}
    ~Main() noexcept override = default;

public:
    void onEnable() override {
        Logger::logger.info("loading test plugin!");
        // 监听
        Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent a) {
            a.group.sendMessage("a");
            Logger::logger.info(a.group.avatarUrl());
            auto r = a.group.getFileById("/e3080b6d-aea9-4d66-9f07-7289cf097cdc");
            Logger::logger.info("file size: ", r.size);
        });
        Event::registerEvent<TimeOutEvent>([](TimeOutEvent a) {
            Logger::logger.info("timeout");
            Logger::logger.info(a.msg);
        });
        schedule(1, "This is a message");
        auto fu = ThreadTask::promiseTask(test_task, std::chrono::seconds(2));
        ThreadTask::addTask(test_task2);
        ThreadTask::addTask(test_task, std::chrono::seconds(1));
        size_t t = fu.get();
        Logger::logger.info("Get future: " + std::to_string(t));
        ////
        // LoaderApi::disableAllPlugins();
        //        int *p = nullptr;
        //        *p = 1;
    }

    void onDisable() override {
        /*插件结束*/
        Logger::logger.info("插件正在禁用！");
    }
};

// 绑定当前插件实例，请不要进行其他操作，
// 初始化请在onEnable中进行
void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin<Main>();
}
