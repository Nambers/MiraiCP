# Change Log

## v2.15.0-RC
* 修复文件列表的问题, #188
* 修复 MiraiCP-native 的问题, 现在理论上应该是能开始用了, 但还没经过完全测试
* 更新 Mirai 版本 -> 2.15.0-M1
* 修复 filter const 的问题 PR #191 by @shuli106
* 增加发布 native 版本的 CI

Internal:
* 修改了一些 MiraiCPString 内部实现
* 修改了 BS 线程池实现

## v2.14.0
* Command注册函数`registerCommand`现在不接受参数，只接受一个模板参数，会调用默认构造函数构造子类对象；command类必须重写基类析构函数
* schedule不会从重载前的插件传递timeout event给重载后的插件
* 采用CRTP定义的Exception具体类型将不会出现在头文件中，因为这造成了大量的编译时间开销（基类仍然在头文件中）。如果需要某种具体的MiraiCP内部异常类型，可以从源文件中找到并复制到你的代码引用位置的前方
* 隐藏`GroupInviteEvent`，`NewFriendRequestEvent`，`MemberJoinRequestEvent`内部接口
* Event回调的handle使用裸指针而非更慢的shared_ptr
* SingleMessage删除了使用指针的`==`重载
* 修复 MemberLeaveEvent 构造问题
* 增加获取全部在线 bot 接口
* 增加获取群荣耀成员接口 (龙王之类的)
* 修复解析 MiraiCode

Internal:
* 修复 Console 没有自己的线程处理输入
* 修复 test 重复加载/cancel 的问题
> MiraiCP-native 还有问题, 目前还跑不了(悲)

## v2.13.2

+ 解决了reload等指令的问题
+ 修改了线程识别方案
+ 增加了 Termux 支持
+ 增加群头衔支持

Internal:
+ 修复命令的一些小问题
+ Kotlin -> v1.8.0
+ Gradle -> v7.6
+ mordant -> v2.0.0-beta11
> libLoaderNative 改为动态链接库, MiraiCP-native 能跑了但是还有些小问题 (画饼.jpg

## v2.13.1
+ 修复已知问题:
	+ 消息解析
	+ 修复一些事件解析
+ 可能包含少许 API 不兼容, 比如属性改名
+ 不同版本的 libLoader 加载不同版本的插件可能导致加载问题, [PR#154])(https://github.com/Nambers/MiraiCP/issues/154) 引入
+ 完善一些事件信息
+ 性能优化
+ libLoader 引入线程池, 可以使用 pushtask 方法调用
+ **在 Thread local 语境中(包括但不仅限于事件监听 lambda)存的静态数据不可靠**
+ Mirai 版本到 2.14.0-RC

Internal:
+ 优化内部实现
+ 内部重构
+ 增加使用 MockTest 的单元测试
+ TimeOut schedule 变成非阻塞
+ 插件与线程无绑定关系

+ 由于 kotlin/native LLVM 后端 GCC 版本不兼容, Native Loader 暂时还无法使用

## v2.13.0-alpha-3

* 修复上一个版本已知问题：
  * Group类updateSetting，现在改为传入一个setting object（API change）
  * 修复arm架构Linux下不能正常找到插件符号的问题[#145](https://github.com/Nambers/MiraiCP/issues/145)
  * 修复了MiraiCode无法发送的问题
  * 版本号全局变量构造UB

> 本次为alpha-dev版, 可能出现一些问题

## v2.13.0-alpha-2
+ 修复上一个版本已知问题:
  + 修复 Member 类 impl

> 本次为alpha-dev版, 可能出现一些问题

## v2.13.0-alpha
> 也不知道为什么就囤了 ~200 commits. ToT  

+ 修复上一个版本已知问题：
  + 插件部分全局变量无法正常构造 [#126](https://github.com/Nambers/MiraiCP/issues/126)
  + 加载时抛出json异常 [#137](https://github.com/Nambers/MiraiCP/issues/137)
+ 加入线程池支持，现在可以在插件中向libloader提交多线程任务
+ 更改了部分接口与内部实现(重构 Contact)
+ 重定向 cerr 和 cout 输出到 logger
+ **删除 MiraiCP-core**

Internal:
+ 增加 cpp 内 unit test (google test)
+ UnhandledExceptionFilter & sigsev catch: 加入对内存访问异常的处理机制，现在单个插件产生致命错误时不会使整个程序崩溃（仅测试于Linux, Windows）
+ 结构优化

> 注意：此版本与之前版本的插件无法兼容，请重新编译插件  
> 本次为alpha-dev版, 可能出现一些问题

## v2.12.0-RC2
+ 修复上一个发行版已知问题:
	+ libLoader 移植性
	+ 不同编译器定义差别
	+ win 下复制插件无法覆盖
	+ MSVC 下符号导出问题
+ 新增 loadTestCI

> **本 release 包含的 MiraiCP-core 组件无法使用, 先用v2.11的**

## v2.12.0-RC
+ 引进 LibLoader 中间件
+ 在 MiraiCP-plugin 以及 MiraiCP-loader 去掉 jni 依赖
+ 支持同时加载多个插件
+ 支持热加载
+ 每个插件对应一个线程

API Change:
+ 插件的信息配置从构造函数移了出去
+ 有关 jni 的全部接口被移除
+ 所有关于 jni 的参数都无需再传入

> **本 release 包含的 MiraiCP-core 组件无法使用, 先用上一个的**
>
> PSS: 本次为 dev 版本，可能包含 bug 错误

## v2.11.0
+ 新增`registerBlockingEvent`，可以用于阻塞后续回调函数；`registerEvent`可以设定优先级
+ 修复`disbale`在`plugin == null`的时候的问题
+ 提升一部分`deprecated` warning 级方法到 error 级
+ 修复`MessageChain::filter`的一些问题
+ `Event`类使用单例模式，现在可以直接用`Event::registerEvent`, 不需要经过`Event::processor`
+ 修复`NodeHandle`, `registerEvent`返回的handle可以正确控制回调是否启用
+ 修复`nextMessage`的一些问题
+ 新增`MessageEvent`, 提供了一组`Friend`, `Member`, `Group`的接口
+ `exceptionEvent`现在只会在MiraiCP异常没有被捕获时才会广播
+ 重构`ForwardedMessage`
    + 修复`ForwardedMessage`的一些问题
    + 新增`ForwardedMessageDisplayStrategy`, 转发的消息可以自定义格式, 见[mirai 文档](https://github.com/mamoe/mirai/blob/dev/mirai-core-api/src/commonMain/kotlin/message/data/ForwardMessage.kt#:~:text=*%20%23%23-,%E6%98%BE%E7%A4%BA%E6%96%B9%E6%A1%88,-*)
+ `messageChain`现在继承了`std::vector<Message>`并弃用`vector<Message>::vector()`;现在可以将`messageChain`看作`std::vector<Message>`使用
+ 升级mirai版本到v2.11.0-RC

## v2.11.0-M3

+ 修复 v2.11.0-M2 loader jar文件上传不完整

## v2.11.0-M2
+ 新增 MiraiCP-core 组件设计, 使得可以直接在 C++ 中启动
+ 修复 memberJoinRequestEvent 没有申请人id
+ 修复 loader 断开连接后不重连问题

## v2.11.0-M1
+ 内部结构优化
+ 修复一些错误码错误
+ 修复异常不能被捕获
+ 支持更改群成员群名片
+ 修复 plugin 读取插件路径的问题
+ 修复getMember不传入botid的问题

## v2.10.0

+ 支持 `MusicShare`
+ 支持 `FlashImage`
+ 修复 `Event` 类中的问题, 删除自 `v2.8.1` deprecated 的方法
+ 更新 mirai 版本到 2.10.0

## v2.9.1

+ mirai版本更新到v2.9.2
+ 内部优化 #106
+ 支持转发转发信息
+ 支持MessagePreSendEvent(但是还不支持中断或更改发送)
+ 支持注册命令(loader只支持控制台命令, plugin是对接mirai的命令系统的rawCommand)

## v2.9.0-M2

在`v2.9.0-M1`基础上修复loader版本的log4j远程执行代码漏洞(log4j -> `v2.15.0`)

> `v2.9.0-M1`的plugin版本和版本<`v2.9.0-M1`不受漏洞影响

## v2.9.0-M1

+ 取消插件后缀检查
+ 使用mirai v2.9.0-M1
+ 支持`Image.isUploaded`
+ 优化`Image`的内部属性
+ 更新一些依赖

> 该版本为预发行版, 可能包含一些问题

## v2.8.0

+ 优化 `SingleMessage` 设计
+ 优化 `Event` 设计
+ 修复 `nextMessage` 的一些问题
+ 修复 `deserializationFromMiraiCode` 的问题 #101
+ 引入 `deserializationFromMessageSource`
+ 支持 `Face`, `MarketFace`, `Forwarded Message` 和 `OnlineForwarded Message`
+ 分离代码到 `h` 和 `cpp` 文件
+ 重新设计 `Exception`
+ Mirai 版本升级到 v2.8.0

> C++ 部分代码从本版本起移动到<https://github.com/Nambers/MiraiCP-template>

## v2.8.0-M2

+ 支持MessageChain和大部分SingleMessage #87
+ 支持解析coming audio, FileMessage, 和 quote (ForwardMessage要等mirai支持) #90
+ 修复戳一戳事件的问题和增加`target`对象(#92 @Zipper-1)
+ 支持`BotLeaveEvent`(机器人退出群聊: 踢出/退出/解散) 和 `MemberJoinRequetEvent`(新群成员申请) #91
+ 分散实现代码到`MiraiCP.cpp`以解决重复定义问题 #96
+ 移动`quoteAndSendMessage`从`MessageSource`和`MessageChain`类，移动到`Contact`类并deprecate
+ 重新设计`Event`类 #95, #99

> 该版本为开发版, 可能包含一些问题
> 需要下载压缩包替换原本全部cmake文件, miraicp.hpp和加入新增加的miraicp.cpp文件

## v2.8.0-M1

+ 升级Mirai到v2.8.0-M1
+ 修复Linux下路径问题
+ 修复可能的loader缓存文件创建不成功的问题(路径简化到单层路径)
+ 修改一些配置项, 增加`advanceConfig.maxThread`最大线程缓存池大小
+ 用`EventRequest`替代原先的`@suppress`的 constructor (#20)
+ 修复 #85 的一系列问题
+ 修复匿名群成员的一些问题 (#86)
+ 代码格式优化
+ MiraiCode parser
+ MessageChain和SingleMessage (#87)
+ 把一些全局指针替换成单例, 造成一些方法的`deprecated`, 按照提示更改就行了

break changes:
+ 删除`At`方法, 用`At`类代替
+ 删除`Context`类, 该类看起来没什么用
+ 删除`Message`类, 应该用`MessageChain`类代替

> 该版本为开发版, 可能包含一些问题

## v2.7.1-dev-1

+ 增加重载dll指令
+ 去掉windows.h依赖
+ 分离c++ test部分
+ 解决 #80 的问题
+ 获取群公告列表独立出来
+ 取消threadNum设置
+ 增加CI build test
+ C++ 部分优化一点代码结构, 消除命名歧义

> 该版本为开发版, 可能包含一些问题

## v2.7.1-patch-1

+ mirai版本升级到v2.7.1-dev-1
+ 解决 #79 里的问题
+ 解决 #77 的kick问题

> 此版本主要修复上一个版本，没有什么新内容

## v2.7.0

+ mirai版本升级到v2.7.0
+ 修复因为发送频率过快导致服务器无应答直接抛出TimeOutEvent #76
+ 增加发送消息retry机制 #76
+ 增加测试框架
+ 支持dll加载依赖的dll
+ 增加nextMessage支持
+ 增加戳一戳和戳一戳事件支持
+ 让loader先登录再加载插件，以在onEnable取到bot
+ 简化不必要的cmake代码
+ 增加和删除一些命令
+ plugin支持命令
+ BotIsBeingMutedException
+ loader端和plugin端配置文件格式更改
	+ 	plugin端的配置文件从miraiCP.txt 要重命名为miraiCP.json, 然后内容为以下格式:
		```
		{
			"pluginConfig":[{
				"path":"\\cmake-build-debug\\MiraiCP.dll",
				"dependencies":[]
			}]
		}
		```
		dependencies项是dll的依赖，如果没有就不填，上面的是dll路径
	+ 	loader配置文件文件名不变，格式为:
		```
		{
		  "accounts": [{
			"id": ,
			"passwords": ""
		  }],
		  "cppPaths":[
			{
			  "path":"\\cmake-build-debug\\MiraiCP.dll",
			  "dependence": []
			}
		  ]
		}
		```


## v2.7-RC-dev5

+ 修复2.7-RC-dev4中plugin端对cpp加载不正确([url](https://mirai.mamoe.net/topic/488/%E5%B0%8F%E7%99%BD%E6%B1%82%E6%95%91-mirai_cp%E6%8F%92%E4%BB%B6%E5%9C%A8mcl%E4%B8%8A%E6%94%B6%E5%88%B0%E6%B6%88%E6%81%AF%E5%B0%B1%E6%8A%A5%E9%94%99))
+ 修复取群成员列表中id溢出int
+ 重新支持定时任务
+ 新的LightAppStyle4

> 该版本主要为修复上一个版本

## v2.7-RC-dev4

+ 支持把plugin部分发布到maven, 以增加mcl支持
+ mirai依赖更新到v2.7-RC-dev-3
+ loader支持(-g)参数创建(覆盖)config.json模板
+ 修复匿名聊天造成的找不到群成员造成的异常退出

> 该版本主要为了发布到maven，较于上一版没什么改动

## v2.7-RC-dev3

+ 优化cmake组合方式(所以推荐一并更新全部的Cmake文件)
+ loader增加jline3命令输入框
 - 支持运行时登录
 - 支持调用onDisable退出
+ loader支持md5作为密码(32位md5 大小写都可以)
+ 更新mirai版本到v2.7-RC-dev1 修复管理员识别问题
+ 支持群公告操作
+ 支持上下文
+ 添加ServiceMessage支持(UrlSharer)
+ 修复一些MiraiCode的小问题
+ 修复Mirai端取消mute(0)导致的不能unMute

> 该版本为预览版的开发版(，包含一些可能出现问题的特性

## v2.7-RC-dev2 [deleted and merge into dev3]

+ 修复Mirai端取消mute(0)导致的不能unMute
+ 修复loader一开始启动没有颜色

> 该版本为v2.7-RC-dev的补丁

## v2.7-RC-dev [deleted and merge into dev3]

+ 优化cmake组合方式(所以推荐一并更新全部的Cmake文件)
+ loader增加jline3命令输入框
 - 支持运行时登录
 - 支持调用onDisable退出
+ loader支持md5作为密码(32位md5 大小写都可以)
+ 更新mirai版本到v2.7-RC-dev1 修复管理员识别问题
+ 支持群公告操作
+ 支持上下文
+ 添加ServiceMessage支持(UrlSharer)
+ 修复一些MiraiCode的小问题

> 该版本为预览版的开发版(，包含一些可能出现问题的特性

## v2.7-Beta

+ 重构框架cmake组成方式
 - 代码集中到一个hpp文件，更新更为方便
 - 增加namespace隔离可能冲突的语义
 - cmake包含源码子项目更容易导入sdk
 - 更多插件信息，更多logger

> 该版本为预览版，包含可能出现问题的特性

## v-NONE
