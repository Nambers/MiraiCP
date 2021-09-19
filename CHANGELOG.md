# Change Log

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
