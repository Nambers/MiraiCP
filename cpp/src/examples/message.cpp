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
        Event::processor.registerEvent<GroupMessageEvent>([](GroupMessageEvent e) {
            // 发送MessageChain
            e.group.sendMessage(e.message);
            // 发送SingleMessage
            e.group.sendMessage(PlainText("a"));
            e.group.sendMessage(e.sender.at(), PlainText(""), Face(5));
            // 回复信息
            e.group.quoteAndSendMessage(PlainText("a"), e.message.source.value());
            // 撤回信息
            e.message.source.value().recall();
            e.group.sendMessage("hi").recall();
        });
    }
};

void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin0(new Main());
}
