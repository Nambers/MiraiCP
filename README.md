# 欢迎来到MiraiCP
![Github](https://img.shields.io/badge/Author-Nambers-blue) ![GitHub](https://img.shields.io/github/license/Nambers/MiraiCP) ![GitHub repo size](https://img.shields.io/github/repo-size/Nambers/MiraiCP) ![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/Nambers/MiraiCP) ![GitHub last commit](https://img.shields.io/github/last-commit/Nambers/MiraiCP)

MiraiCP是一个[Mirai](https://github.com/mamoe/mirai) 的C++语言的社区SDK，基于Mirai-console插件模板开发

## 使用流程:

### 1 下载启动器(mcl)
下载地址[笔者整合包](https://github.com/Nambers/MiraiEXE) 或 [官方](https://github.com/iTXTech/mirai-console-loader/)

### 2 了解项目

文件结构:

kotlin_mirai: 配套kotlin插件

mirai-demo: C++SDK

### 3 使用方法:

#### 3.1 使用release包
1. 下载release文件
2. 压缩包根目录下有个.jar文件留着下面有用
3. 打开`mirai-demo`文件夹下的.sln文件从而打开vs的c++项目，或者用其他方式打开位于mirai-dome目录下的c++项目
4. 在`procession.cpp`里自定义你自己的代码
5. 按照(`x64 release`)这个配置生成.dll文件
6. 看`mirai-demo/x64/Release/`这个路径下的mirai-demo.dll文件，留着下面有用
7. 打开上面下载的mcl文件夹
8. 把`.jar`文件拷贝进mcl的plugin文件夹下
9. 运行一次mcl，然后不管报错，不要登录，直接退出
10. 打开mcl目录下的`data/org.example.miraiCP`路径，把上面的.dll文件拷进来
11. 正常运行mcl
#### 3.2 使用源代码
1. 下载源代码文件
2. 里面把kotlin_mirai用IDE打开
3. 运行gradle里面的buildplugin这个任务
4. 在`/build/mirai/`下获得.jar文件
5. 以下步骤同3.1


如果有其他问题，欢迎提交issue和提交PR贡献

目前支持的事件[events.md](https://github.com/Nambers/MiraiCP/blob/master/doc/events.md)

项目设计流程[intro.md](https://github.com/Nambers/MiraiCP/blob/master/doc/intro.md)

进阶教程(在补了在补了[:wink:])

附:[kotlin版开发模板](https://github.com/Nambers/mirai_kotlin_example)

### TODO
- [ ] 修复`v1.0.1`中频繁调用报错问题
- [ ] 增加更多事件
- [ ] 写文档
