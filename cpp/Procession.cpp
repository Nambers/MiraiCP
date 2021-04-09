#include "pch.h"
void func(unsigned long long i) {
	manager->start();
	try {
		//执行操作
		Friend(i).SendMsg("hi");
	}
	catch (MiraiCPException& e) {	
		logger->Error(e.what());
	}
	manager->detach();
}

void func2(unsigned long long i) {
	manager->start();
	try {
		//执行操作
		Group(i).SendMsg("hi");
	}
	catch (MiraiCPException& e) {
		logger->Error(e.what());
	}
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
	procession->registerEvent([](GroupMessageEvent e) {
		/*std::thread t = std::thread(func, e.group.getOwner().id);
		std::thread t1 = std::thread(func2, e.group.id);
		t.join();
		t1.join();*/
		});
	procession->registerEvent([](NewFriendRequestEvent e) {
		e.accept();
		});
	procession->registerEvent([](PrivateMessageEvent e) {
		e.sender.SendMsg(e.message);
		});
}
void onDisable() {
	/*插件结束,正常退出才会调用*/
}