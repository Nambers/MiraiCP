//
// Created by antares on 5/1/22.
//
#include <MiraiCP.hpp>
#include <string>
using namespace MiraiCP;

class Main : public CPPPlugin {
public:
    // 配置插件信息
    Main() : CPPPlugin(PluginConfig(
                     "id",
                     "test",
                     "v1.0",
                     "a")) {}

    void onEnable() override {
        auto handle0 = Event::registerEvent<GroupMessageEvent>([](const GroupMessageEvent &e) {
            e.group.sendMessage("Hello world!");
        });

        auto handle1 = Event::registerEvent<BotOnlineEvent>([](const BotOnlineEvent &e) {
            auto f = Friend(1234567890, 9876543210);
            f.sendMessage("Bot is live!");
        });

        auto handle2 = Event::registerEvent<MiraiCPExceptionEvent>([&](const MiraiCPExceptionEvent &e) {
            auto ct = Friend(1234567890, 9876543210);
            Logger::logger.info(e.getException());
            ct.sendMessage("Error occurs, type: " + e.getException()->getExceptionType() + ".\nError occurs in file: " + e.getException()->filename + ", line: " + std::to_string(e.getException()->lineNum));
        });

        // 停止handle0注册的GroupMessageEvent
        handle0->stop();
    }
};

// 绑定当前插件实例
void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin0(new Main());
}
