#include "pch.h"
void onEnable() {
	/*插件启动*/
}
void onDisable() {
	/*插件结束*/
}
void EventRegister() {
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

	procession->registerEvent([](GroupMessageEvent param)->void {
		//在这写你自己处理群消息的代码
		logger->Info("hi");
		param.group.SendMsg(param.sender.at());
		});
	procession->registerEvent([](PrivateMessageEvent param)->void {
		//在这写你自己处理私聊消息的代码
		logger->Info("hi");
		param.sender.SendMsg(param.message);
		});
	procession->registerEvent([](GroupInviteEvent param)->bool {
		//处理群邀请，true同意进群,false不同意
		if (param.sender.id == 11111) {
			return ACCEPT;
		}
		return ACCEPT;
		});
	procession->registerEvent([](NewFriendRequestEvent param)->bool {
		//新好友邀请
		logger->Info("新好友申请来自于" + to_string(param.sender.id));
		if (param.message == "hhh") {
			return ACCEPT;
		}
		return ACCEPT;
		});
	procession->registerEvent([](MemberJoinEvent param)->void {
		if (param.type == INVITE) {
			//该成员是被邀请进入的，所以有param.invitor，其他类型都没有
		}
		else if (param.type == ACTIVE) {
			//该成员是主动加入的
		}
		logger->Info(to_string(param.member.id) + "加入本群");
		param.group.SendMsg("欢迎" + param.member.nameCard + "加入本群");
		});
}