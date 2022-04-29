// MiraiCP依赖文件(只需要引入这一个)
#include <MiraiCP.hpp>
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
        // 监听
        Event::processor.registerEvent<GroupMessageEvent>([](GroupMessageEvent a){
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

// 绑定当前插件实例
void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin0(new Main());
}
