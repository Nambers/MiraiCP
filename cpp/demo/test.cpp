
// MiraiCP依赖文件(只需要引入这一个)
#include <MiraiCP.hpp>
using namespace MiraiCP;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

extern "C" {
MiraiCP::PluginConfig PLUGIN_INFO{
        "id",   // 插件id
        "test", // 插件名称
        "v1.0", // 插件版本
        "a",    // 插件作者
                // 可选：插件描述
                // 可选：日期
};
}

#pragma clang diagnostic pop

class Main : public CPPPlugin {
public:
    // 配置插件信息
    Main() : CPPPlugin() {}
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
