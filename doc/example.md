# MiraiCP案例库

* [MiraiCP案例库](#miraicp案例库)
* [注意事项](#注意事项)
  * [取当前消息里全部照片的下载链接](#取当前消息里全部照片的下载链接)
  * [执行定时任务](#执行定时任务)
  * [发送LightAPP(以小程序形式发送卡片)](#发送LightAPP(以小程序形式发送卡片))
  * [各类事件](#各类事件)
    * [群成员退出](#群成员退出)
    * [好友申请](#好友申请)
    * [新成员入群](#新成员入群)
    * [机器人被邀请进群](#机器人被邀请进群)
    * [私聊信息](#私聊信息)
    * [群聊信息](#群聊信息)
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
		param.init();
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
##  取当前消息里全部照片的下载链接
```C++
...
	procession->registerEvent([](PrivateMessageEvent param)->void {
	/*示例: 取消息里全部图片的下载地址*/
	param.init();
	vector<string> temp = Image::GetImgIdFromMiraiCode(param.message);
	for (int i = 0; i < temp.size(); i++) {
		logger->Info(temp[i]);
		logger->Info("图片下载地址:" + Image(param.env, temp[i]).queryURL());
	}
	});
...
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
		param.init();
		logger->Info("hi");
		//延迟100ms发送，后面的1为自定义id
		SetScheduling(100, 1);
		id = param.sender.id;
		param.sender.SendMsg(param.message);
		});
...
```
## 发送LightAPP(以小程序形式发送卡片)
```C++
	procession->registerEvent([](GroupMessageEvent e) {
		e.init();
		//风格1，适合文字暂时，不能交互
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
		//风格3，可以跳转到网址
		//跳转链接
		string url = "https://www.baiduc.com";
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
## 各类事件

### 群成员退出

```C++
	procession->registerEvent([](MemberLeaveEvent p) {
		p.init();
		p.group.SendMsg(p.member.id + "离开了本群");
		});
```

### 好友申请
```C++
	procession->registerEvent([](NewFriendRequestEvent param)->bool {
		//新好友邀请
		param.init();
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
		param.init();
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
		param.init();
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
		param.init();
		logger->Info("hi");
		param.sender.SendMsg(param.message);
		});
```

### 群聊信息
```C++
	procession->registerEvent([](GroupMessageEvent param)->void {
		//在这写你自己处理群消息的代码
		param.init();
		logger->Info("hi");
		param.group.SendMsg(param.sender.at());
		});	
```
## 群成员操作
### 踢出群成员
```C++
	procession->registerEvent([](GroupMessageEvent e) {
		e.init();
		try {
			Member m = Member(qqid, e.group.id);
			m.init();
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