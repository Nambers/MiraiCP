# MiraiCP #
## 附带教程 https://blog.csdn.net/qq_40832960/article/details/108294625 ##
Mirai下载地址 https://github.com/Nambers/MiraiEXE
码云地址:https://gitee.com/JMcreate/MiraiCP
github地址:https://github.com/Nambers/MiraiCP
![Github](https://img.shields.io/badge/Author-Nambers-blue)
![GitHub](https://img.shields.io/github/license/Nambers/MiraiCP)
基于mirai 框架的qq机器人 
使用c++,kotlin 语言编写
作者使用IDEA+VS

实现原理：
用kotlin调用java的jni包，再用jni调用c++编写dll动态链接库，实现使用c++语言编写mirai机器人

文件结构:
kotlin_mirai: 针对mirai 的kotlin插件
Mirai: miraiok包，mirai框架的执行程序
mirai-demo: C++编写SDK，针对mirai 的kotlin插件的插件

使用方法（开始制作插件）:

kotlin版
https://github.com/Nambers/mirai_kotlin_example

c++版
1. 下载全部文件
2. c++sdk中编辑你想要的功能，在pch.cpp里
3. 生成dll动态链接库
4. 放到miraiok包下的libraries里面
5. 如果改名或改存放地点了（不是D:/Mirai/libraries/mirai-demo.dll），要到kotline——mirai文件夹里的，CPP_lib.kt里的路径改成你的路径，并且用gradle中的jar任务重新编译，然后到build/libs文件夹下找到jar文件，复制到miraiok包下的plugin文件夹里面
6. 运行miraiOK_windows_386.exe


错误提示:
如果报错 can't find dependents libraries
把miraiok包下的env文件夹下的全部dll文件复制到syswow64文件夹里
如果还是不行，自己下载depends22_x64.zip 把生成的dllsdk拖进去，查看你的dll文件依赖，然后在百度下载，复制到你的syswow64文件夹里
ps: c:/windows/syswow64

如果有其他问题，私聊作者qq:1930893235或1930893235@qq.com
