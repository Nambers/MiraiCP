#include "pch.h"

void func() {
	//重新赋值genv以确保组件可用
	getEnv();

	/*执行操作*/
	Friend(1111).SendMsg("hi");

	//在jvm结束此线程，env失效
	gvm->DetachCurrentThread();
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
		thread th1(func);
		th1.join();
		});
	

}
void onDisable() {
	/*插件结束,正常退出才会调用*/
}