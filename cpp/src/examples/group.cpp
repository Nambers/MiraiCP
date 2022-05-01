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
using namespace MiraiCP;

class Main : public CPPPlugin {
public:
    Main() : CPPPlugin(PluginConfig(
                     "id",
                     "test",
                     "v1.0",
                     "a")) {}
    void onEnable() override {
        Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent e) {
            // 用 id 构建群对象
            Group a(e.group.id(), e.bot.id);
            // 发送D:\\ValveUnhandledExceptionFilter.txt本地文件到qq群的 /test.txt 路径
            RemoteFile tmp = e.group.sendFile("/test.txt", "D:\\ValveUnhandledExceptionFilter.txt");
            e.group.sendMessage(e.group.getFile("/", tmp.id).name);
            e.group.sendMessage(e.group.getFile("/test.txt").name);
            e.group.sendMessage(e.group.getFileListString("/"));
            // 取群文件列表
            e.group.getFileListString("/");
        });
    }
};

void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin0(new Main());
}
