#include "pch.h"

//全局javavm指针
JavaVM* gvm = nullptr;
//JNI版本
int JNIVersion = 0;
//全局日志指针
Logger* logger = new Logger();
//全局事件指针
Event* procession = new Event();
//全局配置指针
Config* config = new Config();
threadManager* manager = new threadManager();
/*
*正文开始
*/
/*
* 名称:Java_com_example_plugin_CPP_1lib_Verify
* 作用:判断是否连接上本插件，勿改
* 参数:env 必备，job 必备
* 返回值:jstring (用str2jstring把string类型转成jsrting) 发送返回的字符串
*/
JNIEXPORT jstring JNICALL Java_tech_eritquearcus_miraicp_CPP_1lib_Verify(JNIEnv* env, jobject) {
	manager->setEnv(env);
	env->GetJavaVM(&gvm);
	JNIVersion = (int)manager->getEnv()->GetVersion();
	try {
		//初始化日志模块
		logger->init();
		config->Init();
		onEnable();
	}
	catch (MiraiCPException& e) {
		e.raise();
	}
	return Tools::str2jstring(MiraiCPVersion.c_str());//验证机制，返回当前SDK版本
}
/* 插件结束事件*/
JNIEXPORT jobject JNICALL Java_tech_eritquearcus_miraicp_CPP_1lib_PluginDisable
(JNIEnv* env, jobject job) {
	manager->setEnv(env);
	onDisable();
	delete(logger);
	delete(procession);
	delete(config);
	gvm->DestroyJavaVM();
	return job;
}
jstring returnNull() {
	return Tools::str2jstring("MIRAICP_NULL");
}
/*
* 消息解析分流
*/
JNIEXPORT jstring JNICALL Java_tech_eritquearcus_miraicp_CPP_1lib_Event
(JNIEnv* env, jobject, jstring content) {
	manager->setEnv(env);
	Json::Value root = Tools::StringToJson(Tools::jstring2str(content));
	if (root == Json::nullValue) { APIException("JSON文本异常").raise(); }
	try {
		switch (root["type"].asInt()) {
		case 1: {
			//GroupMessage
			procession->broadcast(
                GroupMessageEvent(root["botid"].asLargestUInt(),
                                  Group(root["groupid"].asLargestUInt(), root["botid"].asLargestUInt(), env),
                                  Member(root["senderid"].asLargestUInt(), root["groupid"].asLargestUInt(), root["botid"].asLargestUInt(), env),
                                  root["message"].asCString(),
                                  MessageSource(root["Source"].asCString(), root)
                                  )
			);
			break;
		}
		case 2: {
			//私聊消息
			procession->broadcast(
            PrivateMessageEvent(root["botid"].asLargestUInt(),
                                Friend(root["senderid"].asLargestUInt(), root["botid"].asLargestUInt(), env),
                              root["message"].asCString(),
                              MessageSource(root["Source"].asCString(), root)
            ));
			break;
		}
		case 3:
			//群聊邀请
			procession->broadcast(
			        GroupInviteEvent(
                            root["botid"].asLargestUInt(),
                            root["source"].asCString(),
                            root["invitername"].asCString(),
                            root["inviterid"].asLargestUInt(),
                            root["groupname"].asCString(),
				            root["groupid"].asLargestUInt()
			));
			break;
		case 4:
			//好友
			procession->broadcast(NewFriendRequestEvent(
			        root["source"],
				root["botid"].asLargestUInt()
			));
			break;
		case 5:
			//新成员加入
			procession->broadcast(MemberJoinEvent(
				root["jointype"].asInt(),
				Member(root["memberid"].asLargestUInt(),
					root["groupid"].asLargestUInt(),
					root["botid"].asLargestUInt()),
				Group(root["groupid"].asLargestUInt(), root["botid"].asLargestUInt()),
				Member(
					root["inviterid"].asLargestUInt(),
					root["groupid"].asLargestUInt(),
					root["botid"].asLargestUInt()),
				root["botid"].asLargestUInt()
			));
			break;
		case 6:
			//群成员退出
			procession->broadcast(MemberLeaveEvent(
				root["leavetype"].asInt(),
				Member(root["memberid"].asLargestUInt(),
					root["groupid"].asLargestUInt(),
					root["botid"].asLargestUInt()),
				Group(root["groupid"].asLargestUInt(),
					root["botid"].asLargestUInt()),
				Member(
					root["operatorid"].asLargestUInt(),
					root["groupid"].asLargestUInt(),
					root["botid"].asLargestUInt()),
				root["botid"].asLargestUInt()
			));
			break;
		case 7:
			procession->broadcast(RecallEvent(
				root["Etype"].asInt(),
				root["time"].asInt(),
				root["authorid"].asLargestUInt(),
				root["operatorid"].asLargestUInt(),
				root["ids"].asCString(),
				root["internalids"].asCString(),
				root["botid"].asLargestUInt(),
				root["groupid"].asLargestUInt()
			));
			break;
		case 8:
			procession->broadcast(SchedulingEvent(
				root["message"].asCString()
			));
			break;
		case 9:
			procession->broadcast(BotJoinGroupEvent(
				root["etype"].asInt(),
				Group(root["groupid"].asLargestUInt(), root["botid"].asLargestUInt()),
				(root["etyoe"].asInt() == 2?Member(root["inviterid"].asLargestUInt(),root["groupid"].asLargestUInt(), root["botid"].asLargestUInt()):Member()),
				root["botid"].asLargestUInt()
			));
			break;
		case 10:
			procession->broadcast(GroupTempMessageEvent(
				Group(root["groupid"].asLargestUInt(), root["botid"].asLargestUInt()),
				Member(root["senderid"].asLargestUInt(), root["groupid"].asLargestUInt(), root["botid"].asLargestUInt()),
				root["message"].asCString(),
				MessageSource(root["Source"].asCString()),
				root["botid"].asLargestUInt()
			));
			break;
		}
		return returnNull();
	}catch (MiraiCPException& e) {
		e.raise();
		return returnNull();
	}
	APIException("未知的消息类型,位置C-Handle").raise();
	return Tools::str2jstring("ERROR");
}
