# MiraiCP

[![License](https://img.shields.io/github/license/Nambers/MiraiCP)](https://github.com/Nambers/MiraiCP/blob/master/LICENSE)  [![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/Nambers/MiraiCP?include_prereleases)](https://github.com/Nambers/MiraiCP/releases/) [![GitHub last commit](https://img.shields.io/github/last-commit/Nambers/MiraiCP)](https://github.com/Nambers/MiraiCP/commit/master)

>[中文](https://github.com/Nambers/MiraiCP/blob/master/README.md)
>
>[English](https://github.com/Nambers/MiraiCP/blob/master/README_en.md)

* [Announcement](#Announcement)
* [About](#About)
  * [Features](#Features)
  * [test](#test)
* [How to Use](#How to Use)
  * [1. Environment](#1. Environment)
  * [2. Notes](#2. Notes)
  * [3. Guidance](#3. Guidance)
    * [3.1 How to use miraiCP SDK](#3.1 How to use miraiCP SDK)
      * [3.1.1 Write code in demo](#3.1.1 Write code in demo)
      * [3.1.2 Add miraiCP SDK dependency to your code](#3.1.2 Add miraiCP SDK dependency to your code)
    * [3.2 Start SDK](#3.2 Start SDK)
      * [3.2.1 Use it directly in loader](#3.2.1 Use it directly in loader)
      * [3.2.2 Use as an plugin of mirai console loader](#3.2.2 Use as an plugin of mirai console loader)
* [How to Upgrade](#How to Upgrade)
* [Community](#Community)
* [TODO](#TODO)
* [License](#License)
* [Dependencies](#Dependencies)
* [Stargazers](#Stargazers)


> **Tips~**
> If you are willing to join to develop this project, send email to me (`1930893235@qq.com`) (￣▽￣)"
# Announcement
please note:

0. MiraiCP is a C++ community SDK of [Mirai](https://github.com/mamoe/mirai), based on the mirai-console and the plugin template of mirai-core.

1. This project is only available for study and communication only, and cannot be used for commercial activities (open source according to AGPLv3 license agreement).

2. This project does not contain any malicious code designed to destroy user computer data and obtain user's privacy or any code tracking or monitoring user's computer, does not collect any personal information, and will not disclose user privacy.

3. This project does not provide any specific function implementation, is just the encapsulated package of the project mirai-console and mirai-core (see [mirai](https://github.com/mamoe/mirai)).

4. Any unit or individual who believes that this project may be suspected of infringement should promptly provide feedback or issue. We will delete the illegal content and handle other necessary issues related immediately.

# About

> Using utf8 encoding since version 2.6.3-RC
>
> `/utf8` parameter is needed when using Visual Studio, see [Microsoft doc](https://docs.microsoft.com/zh-cn/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8?view=msvc-160&viewFallbackFrom=vs-2017). Not needed when using cmake

<details>
<summary>Supporting events</summary>

You can see all these content in[Config.kt](https://github.com/Nambers/MiraiCP/blob/master/loader/src/main/kotlin/Config.kt)

| Event description                                            | method name           |
|--------------------------------------------------------------|-----------------------|
| Bot Online Event                                          | BotOnlineEvent    |
| Group Message Event                                          | GroupMessageEvent     |
| Friend Message Event                                         | PrivateMessageEvent   |
| New Friend Request                                           | NewFriendRequestEvent |
| New Group Invite                                             | GroupInviteEvent      |
| New group member join                                        | MemberJoinEvent       |
| group member leave                                           | MemberLeaveEvent      |
| Message recall                                               | RecallEvent           |
| Group temporary Message Event				       | GroupTempMessageEvent |
| Timed out event(when the time of a registered time task out) | SchedulingEvent       |
|Nugde Event					|NugdeEvent|

</details>

**[Online API documentation](https://eritque-arcus.tech/MiraiCP/html/)**

[Docs](doc)

## Features

* C++ 17
* Use `cmake`
* Use jni to communicate with kotlin
* C++ utf8 encoding
* Release with mirai-core concurrently
* Support two ways of using
* Codes are assembled into a `.hpp` and a `.cpp`  file. Each time there is a new release, user only need to renew these files.

## test

|          CI Name           | status                                                       |
| :------------------------: | :----------------------------------------------------------- |
|    C++ in linux(mingw)     | ![C++ Test](https://github.com/Nambers/MiraiCP/actions/workflows/cpp_linux.yml/badge.svg) |
| C++ in windows(mingw&msvc) | ![C++ Test](https://github.com/Nambers/MiraiCP/actions/workflows/cpp_windows.yml/badge.svg) |
|           kotlin           | ![kotlin Test](https://github.com/Nambers/MiraiCP/actions/workflows/gradle.yml/badge.svg) |

# How to Use

## 1. Environment
Mirai needs java version >= 11.

## 2. Notes

> verison rule of MiraiCP: since v2.2.0 *(2021/1/31)*, the first two characters of the version represent the version of Mirai used, and the last character represents the iterative version of this SDK update

1. Till now, this project is tested on Windows and Linux. For Linux user, you need to `export JAVA_PATH=/path/to/your/java/library`, and the filename of plugin you compiled will ends with `.so`, not `.dll`.


## 3. Guidance
### 3.1 How to use miraiCP SDK

#### 3.1.1 Write code in demo

Download MiraiCP_CPP.zip from [the latest release](https://github.com/Nambers/MiraiCP/releases). Unzip it and write code in `demo.cpp`.

#### 3.1.2 Add miraiCP SDK dependency to your code

Download MiraiCP_CPP.zip from [the latest release](https://github.com/Nambers/MiraiCP/releases). Create dynamic link library in cmakelist, where target name must be MiraiCP, and add everything you need. You need to include the directory `include` as well.

```cmake
add_library(MiraiCP SHARED include/miraiCP.cpp demo.cpp)
# add miraiCPdependence
add_subdirectory(include)
```

Add `#include "MiraiCP.hpp"` and `using namespace MiraiCP;` to your codes need miraiCP.

### 3.2 Start SDK

#### 3.2.1 Use it directly in loader

This is only suitable for using miraiCP plugins only.

0. Download MiraiCP-loader-*.jar from [the latest release](https://github.com/Nambers/MiraiCP/releases) (\* is the version name)

1. Create `config.json` and write:

   ```json
   {
     "accounts": [{
       "id": qqid,
       "passwords": "password",
       "protocol":  "pad",
       "heatBeat": "STAT_HB",
       "md5": false,
       "autoLogin": true
     }],
     "cppPath": ".dll or .so path"
   }
   ```

   For the details, see [config.md](doc/config.md#1-loader-%E7%AB%AF)

2. Start up:

   ```bash
   java -jar MiraiCP-loader*.jar
   ```

#### 3.2.2 Use as an plugin of mirai console loader

download [mcl](https://github.com/iTXTech/mirai-console-loader/)

##### 3.2.2.1 Download MiraiCP automatically using mcl

```bash
./mcl --update-package io.github.nambers:MiraiCP-plugin --channel nightly --type plugin
```

Then create and write config in `data/miraiCP/miraicp.json`:

```json
{
	"pluginConfig":[{
		"path":"/path/to/miraiCP/plugin/plugin.dll/or/plugin.so"
	}]
}
```

##### 3.2.2.2 Download MiraiCP manually

0. Download MiraiCP-plugin-*.mirai.jar from [the latest release](https://github.com/Nambers/MiraiCP/releases) (\* is the version name)


1. Put the `.jar` file to the path of plugins in mcl (you need to start mcl once if there is no path of plugins)
2. Create and write `data/miraiCP/miraicp.json`, and see [3.2.2.1](#3221-Download-MiraiCP-automatically-using-mcl)
3. Start mcl

**If there is any other problem, issues and PR are welcome.**

# How to Upgrade

0. download from [the latest release](https://github.com/Nambers/MiraiCP/releases)

2. Replace the `miraiCP.hpp` and `miraiCP.cpp` with new one
3. Replace the MiraiCP-loader-\*.jar or the MiraiCP-plugin-\*.mirai.jar

# Community

Forum: [miraicp](https://mirai.mamoe.net/topic/532/miraicp-c-sdk)

GitHub issue: [miraicp issue](https://github.com/Nambers/MiraiCP/issues)

QQ group: [1044565129](https://jq.qq.com/?k=59OVyP3d)

Email: 1930893235@qq.com

# TODO

See [the milestones of this project](https://github.com/Nambers/MiraiCP/milestones)

# License
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

# Dependencies
[JSON for modern C++](https://github.com/nlohmann/json) - MIT license

[UTF8-CPP: UTF-8 with C++ in a Portable Way](https://github.com/nemtrif/utfcpp) - Boost Software License 1.0

`JNI headers`, in the directory of JDK

# Stargazers
[![Stargazers repo roster for @Nambers/MiraiCP](https://reporoster.com/stars/Nambers/MiraiCP)](https://github.com/Nambers/MiraiCP/stargazers)
