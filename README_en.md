# Welcome to MiraiCP
![License](https://img.shields.io/github/license/Nambers/MiraiCP)  ![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/Nambers/MiraiCP) ![GitHub last commit](https://img.shields.io/github/last-commit/Nambers/MiraiCP)

>[中文](https://github.com/Nambers/MiraiCP/blob/master/README.md)
>
>[English](https://github.com/Nambers/MiraiCP/blob/master/README_en.md)

* [Announcement](#Announcement)
* [About this project](#About)
  * [Start](#Start)
    * [1 Environment](#1-Environment)
    * [2 Notes](#2-Note)
    * [3 Using guide:](#3-Using-Guide)
      * [3\.1 using miraiCP-Plugin plugin with mcl](#31-using-MiraiCP-Plugin-plugin-with-mcl)
      * [3\.2 using MiraiCP-Loader](#32-using-MiraiCP-Loader)
* [Upgrade guide](#upgrade-guide)
* [TODO](#todo)
* [license](#license)
> **Tips~**
> If you are willing to join to develop this project, send email to me(`1930893235@qq.com`) (￣▽￣)"
# Announcement
please note:

0. MiraiCP is a C++ community SDK of [Mirai](https://github.com/mamoe/mirai) which use `JNI` technique to connect the mirai-console or mirai-core.

1. This project just using for studying only, and cannot use for commerce(open source according to AGPLv3 license agreement).

2. This project does not contain any malicious code designed to destroy user computer data and obtain user privacy, does not contain any tracking or monitoring user computer function code, does not collect any user personal information, and does not disclose user privacy.

3. This project does not provide any specific function implementation, only the encapsulating package of the project mirai-console and mirai-core (refer [mirai](https://github.com/mamoe/mirai)).

4. Any unit or individual who believes that this project may be suspected of infringement should promptly provide feedback or issue. This project will promptly delete the illegal content and other related treatments.

# About

<details>
<summary>Supporting events</summary>

You can see all this content in[Config.kt](https://github.com/Nambers/MiraiCP/blob/master/loader/src/main/kotlin/Config.kt)
  
| Event description                                            | method name           |
|--------------------------------------------------------------|-----------------------|
| Group Message Event                                          | GroupMessageEvent     |
| Friend Message Event                                         | PrivateMessageEvent   |
| New Friend Request                                           | NewFriendRequestEvent |
| New Group Invite                                             | GroupInviteEvent      |
| New group member join                                        | MemberJoinEvent       |
| group member leave                                           | MemberLeaveEvent      |
| Message recall                                               | RecallEvent           |
| Timed out event(when the time of a registered time task out) | SchedulingEvent       |

</details>

**Example code [example.md](https://github.com/Nambers/MiraiCP/blob/master/doc/example.md)**

Design flow chart of this project [intro.md](https://github.com/Nambers/MiraiCP/blob/master/doc/intro.md)

The qq face that mirai supporting [faces.md](https://github.com/Nambers/MiraiCP/blob/master/doc/faces.md)

# Start

## 1. Environment
mirai need openjdk instead of Oracle jdk as the runtime environment, such as download the openjdk15/openjdk11.

## 2. Note

> verison rule of MiraiCP: since v2.2.0 *(2021/1/31)*, the first two characters of the version represent the version of Mirai used, and the last character represents the iterative version of this SDK update

1. 因为JNIEnv是局部变量，不了解jni请勿在SDK中使用多线程，如果要使用多线程的话，要保存全局`JavaVM`变量然后在多线程开头使用`genv = JavaVM.GetEnv()`覆盖全局Env，否则mirai部分可能无法成功调用，目前已添加定时计划的实现[代码详见](https://github.com/Nambers/MiraiCP/blob/master/doc/example.md#%E6%89%A7%E8%A1%8C%E5%AE%9A%E6%97%B6%E4%BB%BB%E5%8A%A1)

2. Till now, this project is certainly can use under windowse environment, other enviornments are not tested, and theoretically, this project can be used in linux enviornment(just generate .so file in C++ IDE instead of .dll file).

3. If the C++ IDE report a error about the `jni.h` is missing, add `cpp/include` dir into the library folder settings of C++ IDE(such as `Project->Properties->C++->General`),cpp/include folder inculdes some dependent libraries of the project.

## 3. using guide
There are two ways to use this project(choose one in your opinion):
### 3.1 Using MiraiCP-Plugin plugin with mcl
<details>
<summary>Show</summary>
	
0. download the official plugin loader(mcl), url -> [my backup](https://github.com/Nambers/MiraiEXE) or [official url](https://github.com/iTXTech/mirai-console-loader/)
1. download the MiraiCP-Plugin.7z of the release and unzip
2. copy the `.jar` file in the root of the package
3. open the .sln file in the `cpp` folder to open the C++ project
4. write your own code in `procession.cpp`
5. generate the .dll file
6. copy the .dll file
7. open the mcl folder(download in step 1)
8. paste the `.jar`(in the step 2) into the `plugin` folder in the mcl
9. run the mcl once, ignoring the repoting error in mcl and do not sign in(to generate the data folder in mcl).
10. open the `data/miraiCP` path(the name may change) and paste the .dll file(in the step 6)
	**OR**paste the .dll file in any path and create a file named `miraicp.txt` in `data/miraiCP` folder with the path of your .dll file path
11. run the mcl

</details>

### 3.2 Using MiraiCP-Loader

<details>
<summary>Show</summary>
	
1. download MiraiCP-Loader.7z in the release of this project
2. open the .sln file in `cpp` folder to open the C++ project
3. in `procession.cpp` file write your code
4. generate .dll file
5. copy the `.dll` file in `cpp/x64/Release/` or `cpp/x64/Debug`
6. change the code in `run.bat` in format \[qq number, password, path of .dll(in step 5 and absolute path or relative path can be used),1 or 0(optional, represent the loader will check of new version or not)\], such as `java -jar MiraiCP-Loader.jar 111 aaa D:\temp\mirai-demo.dll 1`
7. run the run.bat

</details>

**If there are other problems, you are welcome to submit issue or pullrequest in this project.**

# Upgrade guide
1. download new release
2. copy the new`.mirai.jar` plugin file or `.jar` loader file over the older one
3. Copy all the individual files in the `cpp` folder from the new release (json and include folders are not required), mainly `pch.h` (precompiled header file), `pch.cpp` (dll entry point), `tools.h` (various events and object class declarations), `tools.cpp` (implementation of declarations in tools.h), and `constants.h` (constant table) overwrite the older ones.
# TODO
see [the milestones of this project](https://github.com/Nambers/MiraiCP/milestones)

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
