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
  * [CI Test status](#ci-test-status)
* [使用声明](#使用声明)
* [使用流程](#使用流程)
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
- 支持termux
- 与mirai-core同步发布(无需mah)
- 支持3种使用方法(mcl, MiraiCP-loader 以及 MiraiCP-core)
- [从2.7.0开始]代码集中到一个hpp文件和其他固定依赖文件，每次更新只需要更新那个文件
- 不封装和隐藏底层实现代码，您可以自定义实现代码或通过实现代码提issue/pr

## CI Test status
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

# 使用流程

见[startup.md](doc/startup.md)

**如果有其他问题，欢迎提交 issue 询问和提交 pull request 贡献**

# 更新方式
## 直接拉取
可直接在模板`clone`下来的文件夹中`git stash`, `git fetch`, `git pull`, `git stash pop` 直接从上游拉取代码, 然后再在本项目的`release`中下载最新对应的jar组件和中间件替换旧的

## 手动替换
0. 从[最新release](https://github.com/Nambers/MiraiCP/releases) 中下载loader/plugin的jar文件和中间件并替换

1. 从各自的模板项目中下载 `MiraiCP.hpp`, `MiraiCP.cpp`, 替换原本的文件

# 调试

请参考[调试文档仓库](https://github.com/0ph1uch1/MiraiCP-debug-docs)。

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
<details> 
<summary>C++ dependencies</summary>

| Dependence name                                              | Url                                      | Author             | License                              |
| ------------------------------------------------------------ | ---------------------------------------- | ------------------ | ------------------------------------ |
| JSON for modern C++                                          | <https://github.com/nlohmann/json>       | Niels Lohmann      | MIT                                  |
| UTF8-CPP: UTF-8 with C++ in a Portable Way                   | <https://github.com/nemtrif/utfcpp>      | Nemanja Trifunovic | BSL-1.0                              |
| JNI Headers                                                  | Null                                     | Java               | GNU General Public License version 2 |
| Amalgamate: C++ source file amalgamation                     | <https://github.com/0ph1uch1/amalgamate> | Antares0982        | AGPL-3.0                             |
| PolyM is a very simple C++ message queue intended for inter-thread communication | <https://github.com/khuttun/PolyM>       | khuttun            | MIT                                  |

</details>

<details> 
<summary>Kotlin dependencies</summary>

| Dependence name                                         | Url                                    | Author          | License                        |
|---------------------------------------------------------|----------------------------------------|-----------------|--------------------------------|
| Jline 3                                                 | <https://github.com/jline/jline3>      | Jline team      | BSD-3                          |
| Mirai                                                   | <https://github.com/mamoe/mirai>       | Mamoe team      | AGPL-3.0                       |
| Jansi                                                   | <https://github.com/fusesource/jansi>  | FuseSource team | Apache License 2.0             |
| Mordant: Colorful styling for command-line applications | <https://github.com/ajalt/mordant>     | AJ Alt          | Apache License 2.0             |
| Junit 5                                                 | <https://github.com/junit-team/junit5> | Junit team      | Eclipse Public License - v 2.0 |

</details>

# Stargazers
[![Stargazers repo roster for @Nambers/MiraiCP](https://reporoster.com/stars/Nambers/MiraiCP)](https://github.com/Nambers/MiraiCP/stargazers)
