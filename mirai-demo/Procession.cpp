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
		procession.registerEvent(lambda) 开始监听事件，可以重复监听
		procession.registerEvent([](GroupMessageEvent param){ \*处理*\});是监听群消息
		procession.registerEvent([](PrivateMessageEvent param){ \*处理*\});是监听群消息
		...
	参数都在param变量里，在lambda块中使用param.xxx来调用
	*/

	procession->registerEvent([](GroupMessageEvent param)->void {
		//在这写你自己处理群消息的代码
		logger->Info("hi");
		param.group.SendMsg(param.sender.at()+param.message);
		});
	procession->registerEvent([](PrivateMessageEvent param)->void {
		//在这写你自己处理私聊消息的代码
		logger->Info("hi");
		param.sender.SendMsg(param.message);
		vector<string> temp = Image::GetImgIdFromMiraiCode(param.message);
		for (int i = 0; i < temp.size(); i++) {
			logger->Info(temp[i]);
			logger->Info("图片下载地址:" + Image(param.env, temp[i]).queryURL());
		}
		});
	procession->registerEvent([](GroupInviteEvent param)->bool{
		//处理群邀请，true同意进群,false不同意
		if (param.sender.id == 11111) {
			return true;
		}
		return true;
		});
	procession->registerEvent([](NewFriendRequestEvent param)->bool {
		//新好友邀请
		logger->Info("新好友申请来自于" + to_string(param.sender.id));
		if (param.message == "hhh") {
			return false;
		}
		return true;
		});
}