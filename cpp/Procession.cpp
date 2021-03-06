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
	参数都在param变量里，在lambda块中使用param.xxx来调用
	*/
	procession->registerEvent([](GroupMessageEvent e) {
		e.init();
		e.group.SendMiraiCode(MiraiCode("a") + &e.group.uploadImg("C:\\Users\\19308\\Desktop\\aa.jpg") + "c");
		e.group.SendMiraiCode(MiraiCode("a").plus(&e.group.uploadImg("C:\\Users\\19308\\Desktop\\aa.jpg")).plus("c"));
		});
	

}
void onDisable() {
	/*插件结束,正常退出才会调用*/
}