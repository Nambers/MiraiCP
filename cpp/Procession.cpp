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
	procession->registerEvent([](MemberLeaveEvent e) {
		try {
			//由于当前成员退出了群聊，所以会抛出MemberException因为找不到对象
			e.init();
			e.member.Mute(5);
		}
		catch (MemberException error) {
			e.group.SendMsg("失败:" + error.what());
		}
		});
}
void onDisable() {
	/*插件结束,正常退出才会调用*/
}