// Copyright (C) 2020-2023 Eritque arcus and contributors.
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

using namespace MiraiCP;

const PluginConfig CPPPlugin::config{
        "测试id",       // 插件id
        "测试名称",     // 插件名称
        "v0.0.1",       // 插件版本
        "大聪明",       // 插件作者
        "测试中文描述", // 可选：插件描述
                        // 可选：日期
};

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
            a.group.sendMessage(std::string("b") + "b");
            Logger::logger.info(a.group.avatarUrl());
            auto r = a.group.getFileById("/e3080b6d-aea9-4d66-9f07-7289cf097cdc");
            Logger::logger.info("file size: ", r.size);
        });
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
