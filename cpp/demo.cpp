// MiraiCP依赖文件(只需要引入这一个)
#include <miraiCP.hpp>
using namespace MiraiCP;

// 插件实例
class Main:public CPPPlugin {
public:
    // 配置插件信息
    Main(): CPPPlugin(PluginConfig("name", "1.0", "Eritque arcus", "hello world", "2021")){}
    void onEnable() override {
        // 监听
    }

    void onDisable() override {
        /*插件结束*/
    }
};

// 绑定当前插件实例
void MiraiCP::enrollPlugin(){
    MiraiCP::enrollPlugin0(new Main());
}
