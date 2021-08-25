# 欢迎来到MiraiCP
[![License](https://img.shields.io/github/license/Nambers/MiraiCP)](https://github.com/Nambers/MiraiCP/blob/master/LICENSE)  [![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/Nambers/MiraiCP?include_prereleases)](https://github.com/Nambers/MiraiCP/releases/) [![GitHub last commit](https://img.shields.io/github/last-commit/Nambers/MiraiCP)](https://github.com/Nambers/MiraiCP/commit/master) ![Lines of code](https://img.shields.io/tokei/lines/github/Nambers/Miraicp)

>[中文](https://github.com/Nambers/MiraiCP/blob/master/README.md)
>
>[English](https://github.com/Nambers/MiraiCP/blob/master/README_en.md)

* [欢迎来到MiraiCP](#欢迎来到miraicp)
* [使用声明](#使用声明)
* [关于MiraiCP](#关于miraicp)
  * [特性](#特性)
* [使用流程:](#使用流程)
  * [1 配置环境](#1-配置环境)
  * [2 注意事项](#2-注意事项)
  * [3 使用方法:](#3-使用方法)
    * [3\.1 如何使用MiraiCP SDK?](#31-如何使用miraicp-sdk)
      * [3\.1\.1 在demo里写](#311-在demo里写)
      * [3\.1\.2 在代码中加入MiraiCP sdk依赖](#312-在代码中加入miraicp-sdk依赖)
    * [3\.2 启动SDK](#32-启动sdk)
      * [3\.2\.1 使用loader直接使用](#321-使用loader直接使用)
      * [3\.2\.2 使用plugin作为mcl的插件使用](#322-使用plugin作为mcl的插件使用)
* [更新方式](#更新方式)
* [交流方式](#交流方式)
* [TODO](#todo)
* [许可](#许可)
* [依赖](#依赖)
* [Stargazers](#stargazers)

> **Tips~**
> 如有意向一起开发本项目，请联系我邮箱(`1930893235@qq.com`) (￣▽￣)"
# 使用声明

0. MiraiCP是一个[Mirai](https://github.com/mamoe/mirai) 的C++语言的社区SDK，基于Mirai-console和Mirai-core插件模板开发

1. 本项目仅供学习参考，禁止用于任何商业用途(根据Mirai的AGPLv3许可协议开源)。

2. 本项目不含有任何旨在破坏用户计算机数据和获取用户隐私的恶意代码，不含有任何跟踪、监视用户计算机功能代码，不会收集任何用户个人信息，不会泄露用户隐私。

3. 本项目不提供任何具体功能实现，仅仅只是对项目mirai-console和mirai-core(详见[mirai仓库](https://github.com/mamoe/mirai))的二次封装。

4. 任何单位或个人认为本项目可能涉嫌侵权，应及时提出反馈，本项目将会第一时间对违规内容给予删除等相关处理。

# 关于MiraiCP

> 从v2.6.3-RC开始，使用utf8作为编码
> 
> vs需要加/utf8编译参数，见[微软文档](https://docs.microsoft.com/zh-cn/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8?view=msvc-160&viewFallbackFrom=vs-2017)
> 
> cmake方式已经加了以utf8编译不用改

<details>
<summary>支持的事件</summary>

这些内容可以在[Config.kt](https://github.com/Nambers/MiraiCP/blob/master/loader/src/main/kotlin/Config.kt)看到
  
| 事件名称     | 函数名称              |
|-------------|-----------------------|
| 机器人上线   | BotOnlineEvent        |
| 群聊消息     | GroupMessageEvent     |
| 私聊消息     | PrivateMessageEvent   |
| 好友申请     | NewFriendRequestEvent |
| 群聊邀请     | GroupInviteEvent      |
| 新群成员加入 | MemberJoinEvent       |
| 群成员离开   | MemberLeaveEvent      |
| 消息撤回     | RecallEvent           |
| 群临时会话   | GroupTempMessageEvent |
| 定时事件执行 | SchedulingEvent       |

</details>

**[在线API文档(包含示例)](https://eritque-arcus.tech/MiraiCP/html/)**

代码示例 [example.md](https://github.com/Nambers/MiraiCP/blob/master/doc/example.md)

本项目设计流程结构 [intro.md](https://github.com/Nambers/MiraiCP/blob/master/doc/intro.md)

Mirai支持的qq表情(对应miraicode的face)对应序号 [faces.md](https://github.com/Nambers/MiraiCP/blob/master/doc/faces.md)

## 特性
- C++ 20
- 使用cmake
- 使用jni与kt交互
- c++ utf8编码
- 与mirai-core同步发布(无需mah)
- 支持2种使用方法
- [从2.7.0开始]代码集中到一个hpp文件和其他固定依赖文件，每次更新只需要更新那个文件
- 不封装和隐藏底层实现代码，您可以自定义实现代码或通过实现代码提issue/pr

# 使用流程:

## 1 配置环境
mirai需要java环境 **>=11**

## 2 注意事项

> MiraiCP版本规则: 从2.2.0开始 *(2021/1/31)*,版本前两位为Mirai的版本，后一位为SDK更新迭代版本

1. 目前只确定win下可用，其他操作系统未测试，理论上liunx应该可用，生成so文件替换dll文件即可

## 3 使用方法:

### 3.1 如何使用MiraiCP SDK?
#### 3.1.1 在demo里写
从[最新release](https://github.com/Nambers/MiraiCP/releases) 中下载MiraiCP_CPP.zip文件夹并解压
在demo.cpp中编写代码
#### 3.1.2 在代码中加入MiraiCP sdk依赖
从[最新release](https://github.com/Nambers/MiraiCP/releases) 中下载MiraiCP_CPP.zip文件夹并解压, 仅保留include文件夹
在你的cmakelist中构建一个动态链接库，target name必须是MiraiCP，然后在下面demo.cpp就主程序在的文件，然后加你依赖的文件，然后引入include作为子文件夹:
```cmake
add_library(MiraiCP SHARED demo.cpp)
# 增加MiraiCP依赖
add_subdirectory(include)
```
然后在你需要使用miraiCP sdk的文件`#include "MiraiCP.hpp"` 和 `using namespace MiraiCP;` 即可
### 3.2 启动SDK
#### 3.2.1 使用loader直接使用
适用于只在机器人上使用MiraiCP SDK插件

0. 从[最新release](https://github.com/Nambers/MiraiCP/releases) 中下载MiraiCP-loader-*.jar(*号为版本)

1. 在同目录下创建config.json作为配置文件

2. 编写json:

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
  "cppPath": "dll路径"
}
```
其中

- accounts可以有多个机器人账户，用逗号分隔
- id为qq号
- passwords为qq密码，可为明文密码或者32位md5密码(大小写都可以)
- protocol为可选配置项，配置内容为登录协议，可选值为:pad/phone/watch (默认为phone)
- heatBeat也为可选配置项，配置内容为心跳策略，可选值为: STAT_HB/REGISTER/NONE (默认为state_hb)
- md5 如果qq密码为md5格式，填true，如果不是可以不填或者填false
- autoLogin 如果要自动登录该账户就填true，否则不填或者填false，如果不为true要手动登录
- cppPath为miraicp sdk生成的dll的路径，一般在sdk的cmake-build-debug文件夹下

3. 使用java -jar MiraiCP-loader-*.jar 启动loader(*号为版本)

#### 3.2.2 使用plugin作为mcl的插件使用
本种方法适用于还需要加载别的mirai-console插件

##### 3.2.2.1 使用mcl自动下载
在mcl同级目录输入
```
mcl --update-package io.github.nambers:MiraiCP-plugin --channel nightly --type plugin
```
安装, 然后在mcl的路径下的data\miraiCP\路径中创建miraicp.txt里填写sdk生成的dll的路径，或者把sdk生成的dll复制进去(如果没有data\miraiCP路径需要启动一次mcl)

##### 3.2.2.2 手动下载

0. 首先下载启动器(mcl), 下载地址 -> [官方](https://github.com/iTXTech/mirai-console-loader/)

1. 从[最新release](https://github.com/Nambers/MiraiCP/releases) 中下载MiraiCP-plugin-*.mirai.jar(*号为版本)

2. 把该mirai.jar放到mcl的plugin路径下(如果没有plugin路径需要启动一次mcl)

3. 在mcl的路径下的data\miraiCP\路径中创建miraicp.txt里填写sdk生成的dll的路径，或者把sdk生成的dll复制进去(如果没有data\miraiCP路径需要启动一次mcl)

4. 启动mcl


**如果有其他问题，欢迎提交issue和提交PR贡献**

# 更新方式
0. 从[最新release](https://github.com/Nambers/MiraiCP/releases) 中下载MiraiCP.hpp和loader或plugin jar包

1. 替换include/下的miraiCP.hpp

2. 替换你的loader或plugin jar包

# 交流方式

论坛贴: [miraicp](https://mirai.mamoe.net/topic/532/miraicp-c-sdk)

github issue: [miraicp issue](https://github.com/Nambers/MiraiCP/issues)

qq群: 1044565129

邮件: 1930893235@qq.com

# TODO
查看[本项目的milestones里程碑](https://github.com/Nambers/MiraiCP/milestones)

# 许可
```
Copyright (C) 2020-2021 Eritque arcus and contributors.

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
[JSON for modern C++](https://github.com/nlohmann/json) - MIT licence

[UTF8-CPP: UTF-8 with C++ in a Portable Way](https://github.com/nemtrif/utfcpp) - Boost Software License 1.0

# Stargazers
[![Stargazers repo roster for @Nambers/MiraiCP](https://reporoster.com/stars/Nambers/MiraiCP)](https://github.com/Nambers/MiraiCP/stargazers)
