// MiraiCP依赖文件(只需要引入这一个)
#include <MiraiCP.hpp>
using namespace MiraiCP;

class Main : public CPPPlugin {
public:
    // 配置插件信息
    Main() : CPPPlugin(PluginConfig(
                     "tech.eritquearcus.miraicp",
                     "test",
                     "v1.0",
                     "Eritque arcus")) {}
    void onEnable() override {
        // 监听
        Event::processor.registerEvent<GroupMessageEvent>([](GroupMessageEvent e) {
            Logger::logger.info(e.group.uploadImg("C:/Users/19308/desktop/6019893df194b1a92220efa4836d6cc.png").isUploaded(e.bot.id));
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


