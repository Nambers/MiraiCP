# 欢迎来到MiraiCP案例库

* [欢迎来到MiraiCP案例库](#欢迎来到miraicp案例库)
  * [取当前消息里全部照片的下载链接](#取当前消息里全部照片的下载链接)
  * [执行定时任务](#执行定时任务)
  * [各类事件](#各类事件)
    * [群成员退出](#群成员退出)
    * [好友申请](#好友申请)
    * [新成员入群](#新成员入群)
    * [机器人被邀请进群](#机器人被邀请进群)
    * [私聊信息](#私聊信息)
    * [群聊信息](#群聊信息)

所有的自定义代码皆写在`Procession.cpp`下的`onEnable()`里，如:
```C++
#include "pch.h"
unsigned long groupid = 0;
void onEnable() {
	/*插件启动*/
	/*
	注册事件监听-用户自定义
	logger - 日志组件
		logger.Info(string)发送消息级日志
		logger.Warning(string)发送警告级日志
		logger.Error(string)发送错误级日志
	procession 广播源
		procession.registerEvent(lambda) 注册监听
		procession.registerEvent([](GroupMessageEvent param){ \*处理*\});是监听群消息
		procession.registerEvent([](PrivateMessageEvent param){ \*处理*\});是监听群消息
		...
	参数都在param变量里，在lambda块中使用param.xxx来调用
	*/
	procession->registerEvent([](GroupMessageEvent param)->void {
		//在这写你自己处理群消息的代码
		logger->Info("hi");
		param.group.SendMsg(param.sender.at());
		});
	procession->registerEvent([=](SchedulingEvent e) {
		if (e.id == 1) {
			/*do something*/
			Friend(groupid).SendMsg("这是定时任务");
		}
		});
	procession->registerEvent([&](PrivateMessageEvent param)->void {
		//在这写你自己处理私聊消息的代码
		logger->Info("hi");
		SetScheduling(100, 1);
		groupid = param.sender.id;
		param.sender.SendMsg(param.message);
		});
	procession->registerEvent([](GroupInviteEvent param)->bool {
		//处理群邀请，true同意进群,false不同意
		if (param.sender.id == 11111) {
			return ACCEPT;
		}
		return ACCEPT;
		});
	procession->registerEvent([](NewFriendRequestEvent param)->bool {
		//新好友邀请
		logger->Info("新好友申请来自于" + to_string(param.sender.id));
		if (param.message == "hhh") {
			return ACCEPT;
		}
		return ACCEPT;
		});
	procession->registerEvent([](MemberLeaveEvent p) {
		p.group.SendMsg(p.member.nameCard + "离开了本群");
		});
	procession->registerEvent([](MemberJoinEvent param)->void {
		if (param.type == INVITE) {
			//该成员是被邀请进入的，所以有param.invitor，其他类型都没有
		}
		else if (param.type == ACTIVE) {
			//该成员是主动加入的
		}
		logger->Info(to_string(param.member.id) + "加入本群");
		param.group.SendMsg("欢迎" + param.member.nameCard + "加入本群");
		});
}
void onDisable() {
	/*插件结束*/
}
```

##  取当前消息里全部照片的下载链接
```C++
...
	procession->registerEvent([](PrivateMessageEvent param)->void {
	/*示例: 取消息里全部图片的下载地址*/
	vector<string> temp = Image::GetImgIdFromMiraiCode(param.message);
	for (int i = 0; i < temp.size(); i++) {
		logger->Info(temp[i]);
		logger->Info("图片下载地址:" + Image(param.env, temp[i]).queryURL());
	}
	});
...
```
## 执行定时任务
因为env的局部性，所以在c++部分的多线程什么的并不具备反向调用kotlin部分发送消息或其他操作的能力，所以要把延迟调用移到kotlin部分，我封装好了一个接口
```C++
//注，需为全局变量
unsigned long id = 0;
...
	procession->registerEvent([=](SchedulingEvent e) {
		if (e.id == 1) {
			/*do something*/
			Friend(id).SendMsg("这是定时任务");
		}
		});
		//这里的[&]和[=]分别为以引用和值的方式捕获全部可以捕获的变量，前者可以改变量值，相当于指针，后者无法更改，相当于本地一个局部变量并进行值传递，详细见搜索引擎的c++ lambda
	procession->registerEvent([&](PrivateMessageEvent param)->void {
		//在这写你自己处理私聊消息的代码
		logger->Info("hi");
		//延迟100ms发送
		SetScheduling(100, 1);
		id = param.sender.id;
		param.sender.SendMsg(param.message);
		});
...
```
## 各类事件

### 群成员退出
附，无法取到`p.member.nameCard`，因为该成员已经退出本群，无法取到成员实例，正在修复
```C++
	procession->registerEvent([](MemberLeaveEvent p) {
		p.group.SendMsg(p.member.id + "离开了本群");
		});
```

### 好友申请
```C++
	procession->registerEvent([](NewFriendRequestEvent param)->bool {
		//新好友邀请
		logger->Info("新好友申请来自于" + to_string(param.sender.id));
		//附加信息验证
		if (param.message == "hhh") {
			//拒绝
			return REJECT;
		}
		//同意
		return ACCEPT;
		});
```

### 新成员入群
```C++
	procession->registerEvent([](MemberJoinEvent param)->void {
		if (param.type == INVITE) {
			//该成员是被邀请进入的，所以有param.invitor，其他类型都没有
		}
		else if (param.type == ACTIVE) {
			//该成员是主动加入的
		}
		logger->Info(to_string(param.member.id) + "加入本群");
		param.group.SendMsg("欢迎" + param.member.nameCard + "加入本群");
		});
```

### 机器人被邀请进群
```C++
	procession->registerEvent([](GroupInviteEvent param)->bool {
		//处理群邀请
		if (param.sender.id == 11111) {
			return ACCEPT;
		}
		return ACCEPT;
		});
```
### 私聊信息
```C++
	procession->registerEvent([](PrivateMessageEvent param)->void {
		//在这写你自己处理私聊消息的代码
		logger->Info("hi");
		param.sender.SendMsg(param.message);
		});
```

### 群聊信息
```C++
	procession->registerEvent([](GroupMessageEvent param)->void {
		//在这写你自己处理群消息的代码
		logger->Info("hi");
		param.group.SendMsg(param.sender.at());
		});	
```