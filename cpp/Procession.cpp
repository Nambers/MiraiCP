#include "pch.h"
void func(unsigned long long i) {
	//执行操作
	Friend(i).SendMsg("hi");
	manager->detach();
}

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
	参数都在param变量里，在lambda块中使用param.xxx来调用
	*/
	// 邀请事件
	// 好友申请
	procession->registerEvent([](NewFriendRequestEvent e) {
		e.accept();
		Friend(e.senderid).SendMsg("HI");
		});
	// 邀请加群
	procession->registerEvent([](GroupInviteEvent e) {
		e.accept();
		logger->Info("x");
		Group(e.groupid).SendMsg("被" + e.inviterNick + "邀请进入" + e.groupName);
		});
	// 消息事件
	// 监听私聊
	Event::NodeHandle handle = procession->registerEvent([](PrivateMessageEvent e) {
		std::thread func1(func, e.sender.id());
		e.sender.SendMsg(e.message);
		// 测试取图片
		std::vector <std::string> temp = Image::GetImgIdsFromMiraiCode(e.message);
		func1.detach();
		// 多线程测试,线程应该在lambda中决定要detach还是join, 否则会报错
		for (std::string a : temp) {
			e.sender.SendMsg(a);
		}
		Image tmp = e.sender.uploadImg("C:\\Users\\19308\\Desktop\\a.jpg");
		e.sender.SendMsg(tmp.toMiraiCode());
		e.sender.SendMiraiCode(tmp.toMiraiCode());
		});
	// 监听群信息
	procession->registerEvent([=](GroupMessageEvent e) {
		// 发送文本信息
		e.group.SendMsg("HI");
		// 发送MiraiCode信息，At
		e.group.SendMiraiCode(At(e.sender));
		// 撤回测试
		e.group.SendMsg("撤回测试").recall();
		// 发送xml卡片测试,可以用new传miraicodeable指针进去，也可以用.toMiraiCode()
		// e.group.SendMiraiCode(new LightApp(LightAppStyle1()));
		// e.group.SendMiraiCode(LightApp(LightAppStyle2()).toMiraiCode());
		// e.group.SendMiraiCode(new LightApp(LightAppStyle3()));
		// ForwardMessage(&e.group,
		//	{
		//		ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
		//		ForwardNode(1930893235, "Eritque arcus", "hahaha", -100)
		//	}).sendTo(&e.group);
		// 关闭上面的私聊消息监听器
		// handle.stop();
		// 当前bot属性
		e.sender.SendMsg(e.bot.nick());
		e.sender.SendMsg(e.bot.FriendListToString());
		e.sender.SendMsg(e.bot.GroupListToString());
		});
	// 监听群临时会话
	procession->registerEvent([](GroupTempMessageEvent e) {
		e.sender.SendMsg("hi");
		});
	// 群事件
	procession->registerEvent([](MemberJoinEvent e) {
		e.group.SendMiraiCode(At(e.group.getOwner()) + std::to_string(e.member.id()) + "加入了群聊");
		});
	procession->registerEvent([](MemberLeaveEvent e) {
		e.group.SendMiraiCode(At(e.group.getOwner()) + std::to_string(e.member.id()) + "退出了群聊");
		});
}
void onDisable() {
	/*插件结束,正常退出才会调用*/
}