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
	procession->registerEvent([](NewFriendRequestEvent e) {
		e.accept();
		});
	procession->registerEvent([](PrivateMessageEvent e) {
		std::thread func1(func, e.sender.id);
		e.sender.SendMsg(e.message);
		func1.detach();
		// 线程应该在lambda中决定要detach还是join, 否则会报错
		});
	procession->registerEvent([](GroupTempMessageEvent e) {
		e.sender.SendMsg("hi");
		});
}
void onDisable() {
	/*插件结束,正常退出才会调用*/
}