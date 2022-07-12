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

const PluginConfig CPPPlugin::config{
        "id",   // 插件id
        "test", // 插件名称
        "v1.0", // 插件版本
        "a",    // 插件作者
                // 可选：插件描述
                // 可选：日期
};

class a : public IRawCommand {
public:
    IRawCommand::Config config() override {
        return {"test", {"sname1", "sname2"}};
    }
    void onCommand(std::optional<Contact> c, const Bot &b, const MessageChain &a) override {
        Logger::logger.info(a.toMiraiCode());
    }
    a() = default;
};

class Main : public CPPPlugin {
public:
    Main() : CPPPlugin() {}
    void onEnable() override {
        CommandManager::commandManager.registerCommand(a());
    }
};

void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin(new Main());
}
