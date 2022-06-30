<div align="center">
	
# **MiraiCP**

[API文档](https://eritque-arcus.tech/MiraiCP/) · [下载](https://github.com/Nambers/MiraiCP/releases) · [demo](https://github.com/Nambers/MiraiCP-template)

	
[![License](https://img.shields.io/github/license/Nambers/MiraiCP)](https://github.com/Nambers/MiraiCP/blob/master/LICENSE)  [![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/Nambers/MiraiCP?include_prereleases)](https://github.com/Nambers/MiraiCP/releases/) [![GitHub last commit](https://img.shields.io/github/last-commit/Nambers/MiraiCP/dev)](https://github.com/Nambers/MiraiCP/commit/master)
![image](https://user-images.githubusercontent.com/35139537/143440804-e7385a03-b3b6-4d0a-b4c0-2595ea4b35ba.png)
	
</div>


>[中文](https://github.com/Nambers/MiraiCP/blob/master/README.md)
>
>[English](https://github.com/Nambers/MiraiCP/blob/master/README_en.md)

* [<strong>MiraiCP</strong>](#miraicp)
* [关于MiraiCP](#关于miraicp)
  * [特性](#特性)
  * [test](#test)
* [使用声明](#使用声明)
* [使用流程:](#使用流程)
  * [1 配置环境](#1-配置环境)
  * [2 注意事项](#2-注意事项)
  * [3 使用方法:](#3-使用方法)
    * [3\.0 选择 MiraiCP 插件启动方式](#30-选择-miraicp-插件启动方式)
    * [3\.1 编译动态链接库插件](#31-编译动态链接库插件)
    * [3\.2\.1 启动 MiraiCP\-plugin](#321-启动-miraicp-plugin)
    * [3\.2\.2 启动 MiraiCP\-loader](#322-启动-miraicp-loader)
    * [3\.2\.3 启动 MiraiCP\-core](#323-启动-miraicp-core)
    * [3\.3 下载中间件](#33-下载中间件)
* [更新方式](#更新方式)
  * [直接拉取](#直接拉取)
  * [手动替换](#手动替换)
* [交流方式](#交流方式)
* [TODO](#todo)
* [许可](#许可)
* [依赖](#依赖)
* [Stargazers](#stargazers)

> **Tips~**
> 如有意向一起开发本项目，请联系我邮箱 (￣▽￣)"

# 关于MiraiCP


**[API文档(包含示例)](https://eritque-arcus.tech/MiraiCP/)** · [文档库](doc) · [demo](https://github.com/Nambers/MiraiCP-template)

<details>
<summary>支持的事件</summary>

这些内容可以在[Config.kt](https://github.com/Nambers/MiraiCP/blob/dev/kotlin/shared/src/main/kotlin/Config.kt#L180)看到
  
| 事件名称     | 函数名称              |
|-------------|-----------------------|
| 机器人上线   | BotOnlineEvent        |
| 群聊消息     | GroupMessageEvent     |
| 私聊消息     | PrivateMessageEvent   |
| 好友申请     | NewFriendRequestEvent |
| 群聊邀请     | GroupInviteEvent      |
| 新群成员申请 | MemberJoinRequestEvent |
| 新群成员加入 | MemberJoinEvent       |
| 群成员离开   | MemberLeaveEvent      |
| 机器人加入   | BotJoinEvent          |
| 机器人离开   | BotLeaveEvent         |
| 消息撤回     | RecallEvent           |
| 群临时会话   | GroupTempMessageEvent |
| 定时事件执行 | SchedulingEvent       |
| 戳一戳       | NugdeEvent           |

</details>

## 特性
- C++ 17
- 使用cmake
- 使用jni与kt交互
- c++ utf8编码
- 与mirai-core同步发布(无需mah)
- 支持3种使用方法(mcl, MiraiCP-loader 以及 MiraiCP-core)
- [从2.7.0开始]代码集中到一个hpp文件和其他固定依赖文件，每次更新只需要更新那个文件
- 不封装和隐藏底层实现代码，您可以自定义实现代码或通过实现代码提issue/pr

## test
| CI Name | status |
|:---:|:---|
| C++ in windows(mingw&msvc) & linux(g++) | ![C++ Test](https://github.com/Nambers/MiraiCP/actions/workflows/cppBuilds.yml/badge.svg)|
| kotlin | ![kotlin Test](https://github.com/Nambers/MiraiCP/actions/workflows/gradle.yml/badge.svg)|

# 使用声明

0. MiraiCP是一个[Mirai](https://github.com/mamoe/mirai) 的C++语言的社区SDK，基于Mirai-console和Mirai-core插件模板开发

1. 本项目仅供学习参考，禁止用于任何商业用途(根据Mirai的AGPLv3许可协议开源)。

2. 本项目不含有任何旨在破坏用户计算机数据和获取用户隐私的恶意代码，不含有任何跟踪、监视用户计算机功能代码，不会收集任何用户个人信息，不会泄露用户隐私。

3. 本项目不提供任何具体功能实现，仅仅只是对项目mirai-console和mirai-core(详见[mirai仓库](https://github.com/mamoe/mirai))的二次封装。

4. 任何单位或个人认为本项目可能涉嫌侵权，应及时提出反馈，本项目将会第一时间对违规内容给予删除等相关处理。

# 使用流程:

## 1 配置环境
mirai需要java环境 **>=11**

## 2 注意事项
- 从v2.6.3-RC开始，使用utf8作为编码, vs需要加/utf8编译参数，见[微软文档](https://docs.microsoft.com/zh-cn/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8?view=msvc-160&viewFallbackFrom=vs-2017), 而在CmakeLists里就表现为
```
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
```
- MiraiCP版本规则: 从2.2.0开始 *(2021/1/31)*,版本前两位为Mirai的版本，后一位为SDK更新迭代版本
- `deprecated` 方法一般只保存3个版本
- 编译出的动态链接库**不具备跨平台性**, 如在 windows 下编译的库在 linux 上无法使用, 要在和目标同系统下编译或者用 cmake 交叉编译

## 3 使用方法:

### 3.0 选择 MiraiCP 插件启动方式
0. 选择启动方式
    + MiraiCP-plugin 使用 Mirai-Console-Loader 进行启动, 适用于需要同时使用其他 Mirai 社区中插件的情况
    + MiraiCP-loader 使用 MiraiCP loader 组件进行启动, 适用于只使用 MiraiCP 插件的项目, 无需下载 mcl
    + MiraiCP-core 直接启动 Mirai
1. 进行[下一步](#31-编译动态链接库插件)

### 3.1 编译动态链接库插件
0. 从[MiraiCP-template](https://github.com/Nambers/MiraiCP-template) 中下载 适用于 MiraiCP-plugin 和 MiraiCP-loader 的插件模板, 或者从[MiraiCP-core-demo](https://github.com/Nambers/MiraiCP-core-demo) 中下载适用于 MiraiCP-core 的插件模板
1. 如果是 **MiraiCP-core** 项目, 跳转至[启动 MiraiCP-core](#323-启动-miraicp-core)
2. 在`main.cpp`中编写, 并编译出动态链接库(通常后缀为 `.dll`/`.so`)
3. 进行下一步,  启动和配置 MiraiCP, 跳转至[启动 MiraiCP-plugin](#321-启动-miraicp-plugin)或[启动 MiraiCP-loader](#322-启动-miraicp-loader)

### 3.2.1 启动 MiraiCP-plugin
0. 确保本地已经下载 Mirai-Console-Loader 并可以启动, 如果没有, 跳转至[Mirai-Console-Loader](https://github.com/iTXTech/mirai-console-loader)
1. 在 mcl 目录下输入 `mcl --update-package io.github.nambers:MiraiCP-plugin --channel nightly --type plugin` 让 mcl 自动下载, 或者从本项目 `release` 中下载最新的 `MiraiCP-plugin-<version>.mirai.jar` 放置到 mcl 下的 plugin 文件夹
2. 在 mcl 下的 `data/tech.eritquearcus.miraicp` 里更改配置文件,
    大概长这样:
    里面的 path 就是[3.1节](#31-编译动态链接库插件)编译出来的
    ```json
    {
        "pluginConfig":[{
            "path":"\\cmake-build-debug\\MiraiCP.dll"
        }]
    }
    ```
    详细配置见 [doc](doc/config.md#2-plugin-%E7%AB%AF)
3. 进行下一步, [下载中间件](#33-下载中间件)

### 3.2.2 启动 MiraiCP-loader
0. 确保本地有 jdk 环境
1. 在本项目 `release` 中下载最新的 `MiraiCP-loader-<version>.jar`
2. 在该 jar 同目录下创建 `config.json` 作为配置文件
3. 编写json:
    ```json
    {
      "accounts": [{
        "id": qqid,
        "passwords": "passwords密码",
        "protocol":  "pad",
        "heatBeat": "STAT_HB",
        "md5": false,
        "autoLogin": true
      }],
      "cppPaths": [
      {"path": "dll路径"}
      ]
    }
    ```
    详细见[config.md文档](doc/config.md#1-loader-%E7%AB%AF)
4. 进行下一步, [下载中间件](#33-下载中间件)

### 3.2.3 启动 MiraiCP-core
0. 确保本地有 jdk 环境
1. 从本项目 release 中下载 `MiraiCP-core-<version>.jar`, 放置到一个你知道的目录
2. 在`main.cpp`中把上面 jar 的目录写入(`load`方法内)
3. 直接运行 `main` 方法

### 3.3 下载中间件
0. 确保已经完成了上面的下载启动器, MiraiCP-core 可跳过本阶段
1. 从最新的 release 中下载 `libLoader-<version>` 对应自己操作系统的中间件
名字对照表见下表格
    | Platform Name | Libloader name          |
    | ------------- | ----------------------- |
    | Windows       | libloader-\<version\>.dll |
    | Linux         | libloader-\<version\>.so  |
    如果上面没有你的版本, 可以发 issue 以获取支持, 或者自行编译 libLoader
2. 把 libloader 放在 3.2节 下载的启动器的同目录下, 或者在配置文件里的 `advanceConfig` 下的 `libLoaderPath` 配置项写入 libLoader 的绝对路径**包含扩展名**
3. 使用 `java -jar MiraiCP-loader-<version>.jar` 启动 MiraiCP-loader 或 启动 mcl 以启动 MiraiCP-plugin



**如果有其他问题，欢迎提交issue和提交PR贡献**

# 更新方式
## 直接拉取
可直接在模板`clone`下来的文件夹中`git stash`, `git fetch`, `git pull`, `git stash pop` 直接从上游拉取代码, 然后再在本项目的`release`中下载最新对应的jar组件和中间件替换旧的

## 手动替换
0. 从[最新release](https://github.com/Nambers/MiraiCP/releases) 中下载loader/plugin/core的jar文件和中间件并替换

1. 从各自的模板项目中下载 `MiraiCP.hpp`, `MiraiCP.cpp`, 以及如果是`MiraiCP-core`项目的`core.h`, `core.cpp`, 替换原本的文件

# 交流方式

论坛贴: [miraicp](https://mirai.mamoe.net/topic/532/miraicp-c-sdk)

github issue: [miraicp issue](https://github.com/Nambers/MiraiCP/issues)

qq群: 1044565129

# TODO
~~查看[本项目的milestones里程碑](https://github.com/Nambers/MiraiCP/milestones)~~

查看 [issues](https://github.com/Nambers/MiraiCP/issues)

# 许可
```
Copyright (C) 2020-2022 Eritque arcus and contributors.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or any later version(in your opinion).

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```

# 依赖
| Dependence name                            | Url                                 | Author             | License                              |
|--------------------------------------------|-------------------------------------|--------------------|--------------------------------------|
| JSON for modern C++                        | <https://github.com/nlohmann/json>  | Niels Lohmann      | MIT                                  |
| UTF8-CPP: UTF-8 with C++ in a Portable Way | <https://github.com/nemtrif/utfcpp> | Nemanja Trifunovic | BSL-1.0                              |
| JNI Headers                                | Null                                | Java               | GNU General Public License version 2 |
# Stargazers
[![Stargazers repo roster for @Nambers/MiraiCP](https://reporoster.com/stars/Nambers/MiraiCP)](https://github.com/Nambers/MiraiCP/stargazers)
