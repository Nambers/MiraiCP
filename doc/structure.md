# cpp部分结构
主要结构为一个主类继承插件接口类，最小文件应包含以下内容:
```C++
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
```
`demo.cpp`中除了这些内容都可以删除

# 注意事项
- 插件以`name`属性来区分，所以尽量给插件配置一个独特的名字
- enrollPlugin只会在开始的时候调用一次
