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
#include <MiraiCP.hpp>
#include <string>
using namespace MiraiCP;

const PluginConfig CPPPlugin::config{
        "id",   // 插件id
        "test", // 插件名称
        "v1.0", // 插件版本
        "a",    // 插件作者
                // 可选：插件描述
                // 可选：日期
};

class Main : public CPPPlugin {
public:
    // 配置插件信息
    Main() : CPPPlugin() {}

    void onEnable() override {
        auto handle0 = Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent e) {
            e.group.sendMessage("Hello world!");
        });

        Event::registerEvent<PrivateMessageEvent>([=](PrivateMessageEvent e) {
            // 停止handle0注册的GroupMessageEvent
            handle0->stop();
        });

        auto handle1 = Event::registerEvent<BotOnlineEvent>([](BotOnlineEvent e) {
            auto f = Friend(1234567890, 9876543210);
            f.sendMessage("Bot is live!");
        });

        auto handle2 = Event::registerEvent<MiraiCPExceptionEvent>([&](const MiraiCPExceptionEvent &e) {
            auto ct = Friend(1234567890, 9876543210);
            Logger::logger.info(e.getException());
            ct.sendMessage("Error occurs, type: " + e.getException()->getExceptionType() + ".\nError occurs in file: " + e.getException()->filename + ", line: " + std::to_string(e.getException()->lineNum));
        });
    }
};

// 绑定当前插件实例
void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin(new Main());
}
