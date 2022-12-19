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
#include "MiraiCP.hpp"
#include "MockTests.h"
using namespace MiraiCP;
using namespace MockTests;

const PluginConfig CPPPlugin::config{
        "id1",          // 插件id
        "test",         // 插件名称
        "v1.0",         // 插件版本
        "a",            // 插件作者
        "测试中文描述", // 可选：插件描述
        // 可选：日期
};

class Main : public CPPPlugin {
public:
    Main() : CPPPlugin() {}
    ~Main() override = default;

public:
    void onEnable() override {
        Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent a) {
            Message::messageSerialization(a.message);
            testEnd();
        });
    }

    void onDisable() override {}
};

void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin<Main>();
}
