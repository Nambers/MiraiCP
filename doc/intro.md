# MiraiCP实现流程:

使用c++ 和 kotlin 语言编写

使用IDEA和VS/Clion作为IDE

实现原理：

用kotlin调用java的jni包，再用jni调用c++编写dll动态链接库，实现使用c++语言编写mirai机器人，项目中传递的消息文本均为MiraiCode格式，发送消息时也提供MiraiCode转码后发送

![项目流程](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/pic/p1.svg?raw=true)

