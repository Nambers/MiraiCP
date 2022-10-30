
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

class Main : public CPPPlugin {
public:
    // 配置插件信息
    Main() : CPPPlugin() {}
    ~Main() override = default;

public:
    void onEnable() override {
        Logger::logger.info("loaded");
        // 监听
        Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent a) {
            Logger::logger.info("b");
            auto b = Group(a.group.id(), a.group.botid());
            Logger::logger.info("c");
            auto c = b[a.sender.id()];
            Logger::logger.info("d");
            a.group.sendMessage("a");
            // c.changeNameCard(a.message.toMiraiCode());
            Logger::logger.info(a.group.avatarUrl());
            a.bot.getFriend(a.bot.getFriendList()[0]).sendMessage("--test end--");
        });
        for (auto &&pluginid: LoaderApi::showAllPluginId()) {
            Logger::logger.info("pluginid: " + pluginid);
        }
    }

    void onDisable() override {
        /*插件结束*/
    }
};

// 绑定当前插件实例，除去new Main以外请不要进行其他操作，
// 初始化请在onEnable中进行
void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin(new Main());
}
