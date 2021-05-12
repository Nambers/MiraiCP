# MiraiCP案例库

* [MiraiCP案例库](#miraicp案例库)
* [注意事项](#注意事项)
  * [多线程](#多线程)
  * [构建对象](#构建对象)
	* [构建群成员](#构建群成员)
	* [构建好友](#构建好友)
	* [构建群](#构建群)
	* [构建并发送聊天记录](#构建并发送聊天记录)
  * [取当前消息里全部照片的下载链接](#取当前消息里全部照片的下载链接)
  * [群文件操作](#群文件操作)
	* [发送群文件](#发送群文件)
	* [取群文件信息](#取群文件信息)
  * [执行定时任务](#执行定时任务)
  * [发送LightAPP-以小程序形式发送卡片](#发送LightAPP以小程序形式发送卡片)
  * [撤回信息](#撤回信息)
  * [各类事件](#各类事件)
    * [群成员退出](#群成员退出)
    * [好友申请](#好友申请)
    * [新成员入群](#新成员入群)
    * [机器人被邀请进群](#机器人被邀请进群)
    * [私聊信息](#私聊信息)
    * [群聊信息](#群聊信息)
    * [撤回信息事件](#撤回信息事件)
  * [群成员操作](#群成员操作)
  	* [踢出群成员](#踢出群成员)

所有的自定义代码皆写在`Procession.cpp`下的`onEnable()`里，如:
```C++
#include "pch.h"
unsigned long groupid = 0;
void onEnable() {
	/*插件启动*/
	/*
	注册事件监听-用户自定义
	logger - 日志组件
		logger->Info(string)发送消息级日志
		logger->Warning(string)发送警告级日志
		logger->Error(string)发送错误级日志
	procession 广播源
		procession->registerEvent(lambda) 注册监听
		procession->registerEvent([](GroupMessageEvent param){ \*处理*\});是监听群消息
		procession->registerEvent([](PrivateMessageEvent param){ \*处理*\});是监听私聊消息
		...
	其中"param"是自定义的变量名，改成p,e什么的都可以
	参数都在param变量里，在lambda块中使用param.xxx来调用
	*/
	procession->registerEvent([](GroupMessageEvent param)->void {
		//在这写你自己处理群消息的代码
		logger->Info("hi");
		param.group.SendMsg(param.sender.at());
		});
}
void onDisable() {
	/*插件结束*/
}
```

# 注意事项
从`v2.4.2`开始，记得自行新建Member对象group对象friend对象和事件刚开始都要调用`init()`方法初始化(为了更好的捕获抛出的异常)
从`v2.5.0`开始，不需要`init()`
## 多线程
需在任务结束时调用manager->detach();
```C++
void func() {
	/*执行操作*/
	Friend(1111).SendMsg("hi");
	
	// 结束线程
	manager->detach();
}
void onEnable() {
	procession->registerEvent([](GroupMessageEvent e) {
		thread th1(func);
		th1.join();
		});
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
# 群文件操作

**qq群文件远程路径(文件路径)以`/`开始，即`/`为根目录以及路径分隔符**

**qq群文件远程路径(文件路径)当前mirai只支持最多一层文件夹，即/xx/xx.xxx**

## 发送群文件
参数有2个，第一个是发送到了qq群文件路径(需要带文件名，文件名和本地文件名可以不是相同的)，第二个是本地文件路径
```C++
	procession->registerEvent([](GroupMessageEvent e) {
		// 发送D:\\ValveUnhandledExceptionFilter.txt本地文件到qq群的 /test.txt 路径
		RemoteFile tmp = e.group.sendFile("/test.txt", "D:\\ValveUnhandledExceptionFilter.txt");
	});
```
## 取群文件信息
有三种方式取
- 根据qq群远程路径(不带文件名)和文件id,文件id可以在上传返回的RemoteFile类型中获得, 会在子目录中查找如果当前目录未找到
- 根据qq群远程路径(带文件名)找,不过由于qq群文件运行同名文件这一特性,返回的文件为群文件中同名文件中随机一个
- 根据qq群远程路径(不带文件名)返回文件列表(包含文件夹),然后再根据取到的列表里分析
```C++
	procession->registerEvent([](GroupMessageEvent e) {
		e.group.SendMsg(e.group.getFile("/", id).name());
		e.group.SendMsg(e.group.getFile("/test.txt").name());
		e.group.SendMsg(e.group.getFileListString("/"));
		});
```
# 构建对象

## 构建群成员
用群号和群成员qq号构建一个群成员对象，失败时会抛出MemberException，可以对群成员对象执行mute, kick, getnamecard，以及sendmsg
```C++
Member(groupid, qqid, botid);
```
## 构建好友
用qq号构建一个好友对象，失败时会抛出FriendException，可以对好友对象sendmsg
```C++
Friend(qqid, botid);
```
## 构建群
用群号构建一个群对象，失败时抛出GroupException，可以对群对象sendmsg，getMemberList,和取群名称
```C++
Group(groupid, botid);
```
## 构建并发送聊天记录
```C++
procession->registerEvent([](GroupMessageEvent e) {
		ForwardMessage(&e.group,
			{
				ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
				ForwardNode(1930893235, "Eritque arcus", "hahaha", -100)
			}).sendTo(&e.group);
		});
```
## 执行定时任务
因为env的局部性，所以在c++部分的多线程什么的并不具备反向调用kotlin部分发送消息或其他操作的能力，所以要把延迟调用移到kotlin部分，在miraiCP中封装成一个事件，即`SchedulingEvent`
```C++
//注，需为全局变量
unsigned long id = 0;
...
	procession->registerEvent([=](SchedulingEvent e) {
		//此处的e.id为自定义id，需在调用定时任务时自定义传入
		if (e.id == 1) {
			/*do something*/
			Friend(id).SendMsg("这是定时任务");
		}
		});
		//这里的[&]和[=]分别为以引用和值的方式捕获全部可以捕获的变量，前者可以改变量值，相当于指针，后者无法更改，相当于本地一个局部变量并进行值传递，详细见搜索引擎的c++ lambda
	procession->registerEvent([&](PrivateMessageEvent param)->void {
		//在这写你自己处理私聊消息的代码
		logger->Info("hi");
		//延迟100ms发送，后面的1为自定义id
		SetScheduling(100, 1);
		id = param.sender.id();
		param.sender.SendMsg(param.message);
		});
...
```
## 发送LightAPP(以小程序形式发送卡片)
通过模板构建发送:
```C++
	procession->registerEvent([](GroupMessageEvent e) {
		//修改里面的属性从而自定义
		LightAppStyle1 a = LightAppStyle1();
		LightAppStyle2 b = LightAppStyle2();
		LightAppStyle3 c = LightAppStyle3();
		e.group.SendMiraiCode(LightApp(a).toMiraiCode());
		e.group.SendMiraiCode(LightApp(b).toMiraiCode());
		e.group.SendMiraiCode(LightApp(c).toMiraiCode());
	});
```
或者通过文本构建
```C++
	procession->registerEvent([](GroupMessageEvent e) {
		//风格1，适合文字展示，不能交互,无大图
		//图标地址，应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
		//小标题
		string titles = "{\"title\":\"hi\", \"value\":\"test3\"}";
		//下面的按钮，但是不能按
		string buttons = "{\"name\":\"Test4\",\"action\":\"plugin\",\"actionData\":\"https://baidu.com\"}";
		string x = "{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[应用]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":"
			"{\"appName\":\"Test1\",\"appType\":4,\"appid\":1109659848,"
			"\"iconUrl\":\""+icon+"\"},"
			"\"data\":["+titles+"],"
			"\"title\":\"Test2\",\"button\":"
			"["+buttons+"],"
			"\"emphasis_keyword\":\"\"}},\"text\":\"\",\"sourceAd\":\"\"}";
		e.group.SendMiraiCode(LightApp(x).toString());
		//风格2，不能交互，有预览图
		//icon图标,应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string icon1 = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
		//预览图(大图),应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string preview1 = icon1;
		string a = "{\"config\":"
			"{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"1.0.0.103\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1110081493\",\"preview\":\""+preview1+"\",\"shareTemplateData\":{},"
			"\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"Test5\",\"title\":\"Test6\","
			"\"host\":{\"uin\":1000000,\"nick\":\"应用消息\"},"
			"\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\""+icon1+"\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
		e.group.SendMiraiCode(LightApp(a).toString());
		//风格3，可以跳转到网址，有预览图
		//跳转链接
		string url = "https://www.baidu.com";
		//icon图标,应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string icon2 = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
		//预览图(大图),应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string preview = icon2;
		string b = "{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\""+preview+"\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"Test1\",\"title\":\"Test3\",\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\""+icon+"\",\"qqdocurl\":\""+url+"\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
		e.group.SendMiraiCode(LightApp(b).toString());
		});
```
## 撤回信息
```C++
	procession->registerEvent([](GroupMessageEvent e) {
		
		try {
			e.messageSource.recall();
			e.group.SendMsg("hi").recall();
		}
		catch (MiraiCPException) {
			logger->Error("错误");
		}
		});
```
## 各类事件

### 群成员退出

```C++
	procession->registerEvent([](MemberLeaveEvent p) {
		p.group.SendMsg(p.member.id() + "离开了本群");
		});
```

### 好友申请
```C++
	procession->registerEvent([](NewFriendRequestEvent param)->bool {
		//新好友邀请
		logger->Info("新好友申请来自于" + to_string(param.sender.id()));
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
		logger->Info(to_string(param.member.id()) + "加入本群");
		param.group.SendMsg("欢迎" + param.member.nameCard() + "加入本群");
		});
```

### 机器人被邀请进群
```C++
	procession->registerEvent([](GroupInviteEvent param)->bool {
		//处理群邀请
		if (param.sender.id() == 11111) {
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
### 撤回信息事件
```C++
	procession->registerEvent([](RecallEvent e) {	
		if (e.type == 2) {
			Group g = Group(e.groupid);
			g.SendMsg(to_string(e.operatorid) + "撤回了" + 
				to_string(e.authorid) + "的一条信息");
			
		}
		});
```
## 群成员操作

### 踢出群成员
```C++
	procession->registerEvent([](GroupMessageEvent e) {
		try {
			Member m = Member(qqid, e.group.id(), e.bot.id());
			m.Kick("this_is_reason");
		}
		catch (BotException err) {
			//权限不足
			logger->Error(err.what());
		}
		catch (MemberException err) {
			if (err.type == 1) {
				//找不到群
			}
			if (err.type == 2) {
				//找不到群成员
			}
		}
		});
```
