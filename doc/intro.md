# 欢迎来到MiraiCP #

MiraiCP是一个[Mirai](https://github.com/mamoe/mirai) 的C++语言的社区SDK，基于Mirai-console插件模板开发

启动器选择: mcl下载地址[笔者整理版](https://github.com/Nambers/MiraiEXE) 或 [官方](https://github.com/iTXTech/mirai-console-loader/)

实现流程:

使用c++ + kotlin 语言编写

作者使用IDEA+VS作为IDE

实现原理：

用kotlin调用java的jni包，再用jni调用c++编写dll动态链接库，实现使用c++语言编写mirai机器人(下图已过时，不过大体上是这样的)

![项目流程](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/pic/流程.svg?raw=true)

文件结构:

kotlin_mirai: 配套kotlin插件

mirai-demo: C++SDK

使用方法:

c++版
1. 下载release文件(中包含编译好的.jar)或源代码(需自己编译出.jar)
2. c++sdk(mirai-demo)中`procession.cpp`中编辑代码实现你想要的功能
3. 生成dll动态链接库(用x64,release这个配置)
4. 找到生成的.dll文件(通常叫mirai-demo.dll目前不支持改名,除非你改源代码里的名称生成的自定义.jar插件)和.jar文件(如果是release就在根目录，如果源代码要自己生成)
5. 把.dll文件放到mcl(mirai-console-loader)下的data文件夹下的miraiCP里,然后把.jar文件放到plugin文件夹里,如果data下没有没有要先运行一次mcl然后会报错，退出，复制dll进去再运行


如果有其他问题，提交issue

欢迎提交PR贡献

目前支持的事件[events.md](https://github.com/Nambers/MiraiCP/blob/master/doc/events.md)

附:[kotlin版开发模板](https://github.com/Nambers/mirai_kotlin_example)