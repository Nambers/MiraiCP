
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

class Main : public CPPPlugin {
public:
    // 配置插件信息
    Main() : CPPPlugin({"id",
                        "test",
                        "v1.0",
                        "a"}) {}
    ~Main() override = default;

public:
    void onEnable() override {
        // 监听
        Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent a) {
            Logger::logger.info("b");
            auto b = Group(a.group.id(), a.group.botid());
            Logger::logger.info("c");
            auto c = b[a.sender.id()];
            Logger::logger.info("d");
            c.changeNameCard(a.message.toMiraiCode());
        });
    }

    void onDisable() override {
        /*插件结束*/
    }
};

// 绑定当前插件实例，必须实现该函数
void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin(new Main);
}
