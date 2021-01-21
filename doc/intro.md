# 欢迎来到MiraiCP #

MiraiCP是一个[Mirai](https://github.com/mamoe/mirai) 的C++语言的社区SDK，基于Mirai-console插件开发

启动器选择:mcl下载地址[笔者整理版](https://github.com/Nambers/MiraiEXE)

实现流程:

使用c++ + kotlin 语言编写

作者使用IDEA+VS作为IDE

实现原理：

用kotlin调用java的jni包，再用jni调用c++编写dll动态链接库，实现使用c++语言编写mirai机器人

![流程](https://github.com/Nambers/MiraiCP/tree/master/doc/pic/流程.png)

文件结构:

kotlin_mirai: 针对mirai 的kotlin插件

mirai-demo: C++编写SDK，针对mirai 的kotlin插件的插件

使用方法（开始制作插件）:

c++版
1. 下载release文件或源代码(需自己编译出.jar)
2. c++sdk中procession.cpp中编辑你想要的功能
3. 生成dll动态链接库(x64,release)
4. 放到mcl(mirai-console-loader)下的data文件夹下的miraiCP里,然后把release版本中的.jar文件放到plugin文件夹里,如果data下没有没有要先运行一次mcl然后会报错，退出，复制dll进去再运行


如果有其他问题，提交issue

附:[kotlin版开发模板](https://github.com/Nambers/mirai_kotlin_example)