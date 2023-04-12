## Step 1 构建项目

> 本指南默认您有一定的 C++, Cmake 和 git cli 基础
### i 下载demo
> 直接从修改demo开始是一个极好的方法

```git clone https://github.com/Nambers/MiraiCP-template```
### ii 打开 demo 项目
自行搜索, 

或者参考这篇[readme](https://github.com/Nambers/MiraiCP-template/blob/main/README.md)
### iii 认识 demo
```
root
│  .gitignore
│  CMakeLists.txt # 你要注意的文件 1, 接下来称为 `cmake文件`
│  LICENSE # 开源协议
│  README.md # 教你怎么打开 cmake 项目的 markdown
│
├─single_include
│  ├─3rd_include # MiraiCP 依赖的第三方库
│  │
│  └─MiraiCP # MiraiCP 本体依赖
│          MiraiCP.cpp
│          MiraiCP.hpp
│
└─src
        main.cpp # 你要注意的文件, 接下来称为 `main.cpp`
```
在 `main.cpp` 中， `onEnable` 会在一开始（或者被重新启用）的时候调用一次, `onDisable` 会在结束（或者被禁用）的时候调用一次.
### iv 基础配置 - 插件
打开 `main.cpp`
#### 更改插件信息
找到
```
const PluginConfig CPPPlugin::config{
        "Plugin id",          // 插件id
        "Plugin name",        // 插件名称
        "Version",            // 插件版本
        "Author name",        // 插件作者
        "Plugin description",  // 可选：插件描述
        "Publish time"        // 可选：日期
};
```
把里面的信息改为你*喜欢的*

id最好不超过16个ASCII字符。避免空格和中文字符、特殊字符（非ASCII），然后务必和其他插件区分开

`可选` 意味着你可以不设置这个参数

#### \[可选\]更改类名
你可以把
```
// 插件实例
class Main : public CPPPlugin {
```
的 `Main` 改成任意你*喜欢的*, 只要在下面的
```
// 绑定当前插件实例
void MiraiCP::enrollPlugin() {
  MiraiCP::enrollPlugin<Main>();
}
```
里把 `Main` 改成这个类名就ok
### v 基础配置 - cmake
#### cmake 版本 和 C++ 标准
```
# 最低cmake版本
cmake_minimum_required(VERSION 3.14)
# C++ 标准：17+
set(CMAKE_CXX_STANDARD 17)
```
#### \[可选\] 项目名称
```
# 声明C++项目
project(MiraiCPPlugin LANGUAGES CXX)
```
可以把 `MiraiCPPlugin` 替换成你任何*喜欢的*, 然后把这个文件里剩下的全部 `MiraiCPPlugin` 都要替换成你自己的
#### \[可选\] 增加新源文件
你可以在项目目录下新建一个文件(或者可以在任何地方以及任何文件名, 不过有点太混沌邪恶了), 然后在
```
# 添加动态库编译目标
add_library(
        MiraiCPPlugin
        SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/single_include/MiraiCP/MiraiCP.cpp # MiraiCP 源文件
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${PLUGIN_SOURCE_PATH}
)
```
里面添加, 比如新建了一个 `main2.cpp` 在 `src` 目录下, 也就是 `main.cpp` 同目录， 需要修改以上代码片段到
```
add_library(
        MiraiCPPlugin
        SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/single_include/MiraiCP/MiraiCP.cpp # MiraiCP 源文件
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main2.cpp
        ${PLUGIN_SOURCE_PATH}
)
```
> 顺序应该是无所谓的
### vi 你可能在找?
#### 监听事件
监听事件指: 接受qq bot的输入, 常用的监听事件有: 群消息事件, 私聊消息事件.

如果你想监听一个事件, 首先先打开 [在线API文档](https://eritque-arcus.tech/MiraiCP)

然后在左侧侧边栏里按路径打开 **MiraiCP -> 文件 -> 文件列表 -> Event.h**，找到你想监听的事件

如果没找到，你可以反思下你的要求是不是有点离谱。如果你觉得没问题，可以开个 issue 让维护人员支持。

找到事件后，打开 `main.cpp`，找到 `onEnable`。
```
 // 入口函数
void onEnable() override {
    // 请在此处监听
    Event::registerEvent<事件类名>([](事件类名 e){
        //do job
    });
}
```
比如, 监听群消息事件:
```
 // 入口函数
void onEnable() override {
    // 请在此处监听
    Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent e){
        Logger::logger.info(":)");
    });
}
```
#### 发送消息
假如你想在收到群消息事件(也就是 bot 的某个群接受到了信息)后发送这条信息回去
```
Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent e) {
            e.group.sendMessage(e.message);
        });
```
#### 其他更多例子
见 [API doc](https://eritque-arcus.tech/MiraiCP-docs/html/examples.html)
### vii 编译
自行搜索，IDE 里的编译按钮，或者如果你喜欢 cli:
```
cd build
cmake ..
cmake --build . --target MiraiCPPlugin
```

至此, 构建项目完成
## Step 2 配置运行环境

> 重要提醒：如果目标部署的系统是Linux，请使用OpenJDK，不要使用Oracle JDK！Oracle JDK有一个很严重的bug（详情见[issue142](https://github.com/Nambers/MiraiCP/issues/142)），可能导致你的插件无法按照预期运行

### i 选择启动方式
| 名称 | 特性 | 下一步 |
| --- | --- | --- |
| MiraiCP-plugin | 使用 Mirai-Console-Loader 进行启动, 适用于需要同时使用其他 Mirai 社区中插件的情况 | [ii MiraiCP Plugin](#ii-miraicp-plugin) |
| MiraiCP-loader | 使用 MiraiCP loader 组件进行启动, 适用于只使用 MiraiCP 插件的项目, 无需下载 mcl | [iii MiraiCP Loader](#iii-miraicp-loader) |
| MiraiCP-loader-native | 使用 MiraiCP loader native 组件进行启动, 适用于只使用 MiraiCP 插件的项目, 无需下载 mcl, 配置 java 和 下载中间件 | [v MiraiCP Loader Native](#v-miraicp-loader-native) |

### ii MiraiCP Plugin
> 需要java环境 **>=11**, 自行下载 jdk 11或更高版本（推荐下openJdk）

1. 首先, 自行下载 [Mirai-Console-Loader](https://github.com/iTXTech/mirai-console-loader) 并配置
2. 然后在 mcl 目录下输入 `mcl --update-package io.github.nambers:MiraiCP-plugin --channel nightly --type plugin` 让 mcl 自动下载, 或者从本项目 `release` 中下载最新的
3. `MiraiCP-plugin-<version>.mirai2.jar` 放置到 mcl 下的 plugin 文件夹

[下一步-下载中间件](#iv-中间件下载)
### iii MiraiCP Loader
> 需要java环境 **>=11**, 自行下载 jdk 11或更高版本(推荐下openJdk)

1. 在本项目 `release` 中下载最新的 `MiraiCP-loader-<version>.jar`

2. 在该 jar 同目录下创建 `config.json` 作为配置文件

[下一步-下载中间件](#iv-中间件下载)

### iv 中间件下载
从最新的 release 中下载 `libLoader-<version>` 对应自己操作系统的中间件`libLoader`. `libLoader`是用于管理插件运行时动态链接的工具，提供热重载，线程池，内存访问异常处理等功能。`libLoader`的详细工作原理见[文档](doc/libloader.md)

名字和平台对照表见下表格  
| Platform Name | Libloader name          |
| ------------- | ----------------------- |
| Windows       | libloader-\<version\>.dll |
| Linux         | libloader-\<version\>.so  |  
如果上面没有你的平台, 可以发 issue 以获取支持, 或者自行编译 libLoader  
或者，从源码构建，以Linux为例(详细可以参考`/scripts`下的编译脚本):  

```bash
git clone https://github.com/Nambers/MiraiCP --depth 1
cd MiraiCP/cpp
mkdir build
cd build
cmake -DGOOGLE_TEST=OFF .. 
make Loader
```

至此，配置环境部分暂时告一段落，可直接前往 *Step 3 配置运行环境*  
### v MiraiCP Loader Native
> 运行 Native 版本可能需要一定的知识
> 该版本还没经过全面测试
> -- 4/12/2023
#### 下载
在 GitHub Releases 里面下载 LoaderNativeLinux 或者 LoaderNativeWindows, 然后本地解压.
#### 环境配置
因为 Mirai 依赖于 Openssl 和 curl 运行, 所以启动 native 前要安装这两个库
##### Linux
依赖预览:
```
libLoaderNative.so => ./libLoaderNative.so
        libresolv.so.2 => /lib/x86_64-linux-gnu/libresolv.so.2
        libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6
        libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0
        libutil.so.1 => /lib/x86_64-linux-gnu/libutil.so.1
        libcrypt.so.1 => /lib/x86_64-linux-gnu/libcrypt.so.1
        librt.so.1 => /lib/x86_64-linux-gnu/librt.so.1
        libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1
        libcrypto.so.1.1 => not found
        libssl.so.1.1 => not found
        libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2
        libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6
        /lib64/ld-linux-x86-64.so.2
```
可以先使用 `LD_LIBRARY_PATH=./ ldd MiraiCP-loader-<version>.kexe` 来检查哪些环境缺失了.  
一般来说应该只需要额外安装一次 Openssl (比如用 `sudo apt install libssl-dev` 在 ubuntu).  
环境配置这块可能需要使用者自己查询资料配好.  
> 如果不加 `LD_LIBRARY_PATH=./` 跑 ldd 显示 `libLoaderNative.so => not found` 是正常的
> 我们同时把 `libcrypt.so.1` 一起打包了, 以防 Arch 下找不到
##### Windows
依赖预览:
```
Image has the following dependencies:
    ADVAPI32.dll
    bcrypt.dll
    KERNEL32.dll
    msvcrt.dll
    WS2_32.dll
    libLoaderNative.dll
    libcrypto-3-x64.dll
    libcurl.dll
```  
可以用 VS developer command prompt 里面的 `dumpbin /DEPENDENTS MiraiCP-loader-<version>.exe` 查看依赖.  
通常的做法是用 vcpkg 安装 openssl 和 curl.  
安装命令类似于 ` vcpkg install openssl:x64-windows curl[core,ssl]:x64-windows`.  
#### 启动配置
native 的启动配置和 loader 完全一致, 两者的配置文件 `config.json` 可以完全互通.  
但是与 loader 不同的是, native 不需要配置 libLoaderPath 也就是中间件路径, 他会以动态库依赖的方式载入, 而不是 loader 的动态载入.  
所以如果 native 找不到中间件他会直接无法执行.  
> native 的配置文件路径与 loader 相同, 都是在可执行文件的同目录下.  
#### 启动
在 linux 下执行 `runMiraiCP.sh` 脚本, 该脚本会自动检查依赖和启动 (如果自己启动注意要加 `LD_LIBRARY_PATH=./`).  
在 windows 下直接运行可执行文件.  

## Step 3 配置运行环境
### 对于 MiraiCP Plugin 启动方法
1. 先运行一遍 mcl 然后退出
2. 在 mcl 下的 `data/tech.eritquearcus.miraicp` 里更改配置文件,
    里面的 `path` 就是`Step 1`编译出来的动态链接库的地址, `libLoaderPath` 是下载的中间件的地址
    
    ```json
    {
        "advanceConfig": {
            "libLoaderPath": "\\cmake-build-debug\\libLoader.dll"
        },
        "pluginConfig":[{
            "path":"\\cmake-build-debug\\MiraiCP.dll"
        }]
    }
    ```
    详细配置见 [doc](config.md#2-plugin-%E7%AB%AF)
3. 运行 mcl
### 对于 MiraiCP Loader 启动方法
1. 在 `Step 2` 下载的 loader jar 同目录下创建 `config.json` 作为配置文件（或者在任何一个地方创建，然后调用的时候把地址作为第一个命令行参数）
2. 编写json： `libLoaderPath` 就是`Step 2`下载的中间件， `path` 是 `Step 1` 编译出的 dll 地址。
    ```json
    {
        "advanceConfig": {
            "libLoaderPath": "/cmake-build-debug/libLoader.dll"
        },
      "accounts": [{
        "id": qqid,
        "passwords": "passwords",
        "md5": false,
        "autoLogin": true
      }],
      "cppPaths": [
      {"path": "/cmake-build-debug/MiraiCP.dll"}
      ]
    }
    ```
    详细见[config.md文档](config.md#1-loader-%E7%AB%AF)
3. 使用 `java -jar MiraiCP-loader-<version>.jar` 启动 MiraiCP-loader

**如果有其他问题，欢迎提交issue和提交PR贡献**