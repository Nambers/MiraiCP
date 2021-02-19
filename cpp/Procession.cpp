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
	procession->registerEvent([](GroupInviteEvent e) {
		try {
			e.init();
		}catch (MiraiCPException error) {
			/*捕获基类只能获取是否有异常发生，要用细分的异常类捕获才能知道发生了什么异常*/
			logger->Error(error.what());
		}
		//错误示范: 因为当前事件还未入群，所以发送消息会报错
		e.group.SendMsg("邀请");
		return false;
		});

}
void onDisable() {
	/*插件结束,正常退出才会调用*/
}