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
	procession->registerEvent([](NewFriendRequestEvent e) {
		e.accept();
		});
	procession->registerEvent([](GroupInviteEvent e) {
		e.accept();
		});
	// 消息事件
	procession->registerEvent([](PrivateMessageEvent e) {
		std::thread func1(func, e.sender.id);
		e.sender.SendMsg(e.message);
		std::vector <std::string> temp = Image::GetImgIdsFromMiraiCode(e.message);
		func1.detach();
		// 线程应该在lambda中决定要detach还是join, 否则会报错
		for (std::string a : temp) {
			e.sender.SendMsg(a);
		}
		});
	procession->registerEvent([](GroupMessageEvent e) {
		e.group.SendMsg("HI");
		e.group.SendMiraiCode(At(e.sender));
		e.group.SendMsg("撤回测试").recall();
		// 可以用new传miraicodeable指针进去，也可以用.toMiraiCode()
		e.group.SendMiraiCode(new LightApp(LightAppStyle1()));
		e.group.SendMiraiCode(LightApp(LightAppStyle2()).toMiraiCode());
		e.group.SendMiraiCode(new LightApp(LightAppStyle3()));
		ForwardMessage(&e.group,
			{
				ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
				ForwardNode(1930893235, "Eritque arcus", "hahaha", -100)
			}).sendTo(&e.group);
		});
	procession->registerEvent([](GroupTempMessageEvent e) {
		e.sender.SendMsg("hi");
		});
	// 群事件
	procession->registerEvent([](MemberJoinEvent e) {
		e.group.SendMiraiCode(At(e.group.getOwner()) + std::to_string(e.member.id) + "加入了群聊");
		});
	procession->registerEvent([](MemberLeaveEvent e) {
		e.group.SendMiraiCode(At(e.group.getOwner()) + std::to_string(e.member.id) + "退出了群聊");
		});
}
void onDisable() {
	/*插件结束,正常退出才会调用*/
}