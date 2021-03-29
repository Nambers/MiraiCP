#include "pch.h"

//全局jnienv指针
JNIEnv* genv = nullptr;
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
/*
*正文开始
*/
/*
* 名称:Java_com_example_plugin_CPP_1lib_Verify
* 作用:判断是否连接上本插件，勿改
* 参数:env 必备，job 必备
* 返回值:jstring (用str2jstring把string类型转成jsrting) 发送返回的字符串
*/
JNIEXPORT jstring JNICALL Java_org_example_mirai_plugin_CPP_1lib_Verify(JNIEnv* env, jobject) {
	genv = env;
	JNIVersion = (int)genv->GetVersion();
	env->GetJavaVM(&gvm);
	try {
		//初始化日志模块
		logger->init();
		config->Init();
		onEnable();
	}
	catch (MiraiCPException e) {
		e.raise();
	}
	return tools.str2jstring("v2.5.0");//验证机制，返回当前SDK版本
}
/* 插件结束事件*/
JNIEXPORT jobject JNICALL Java_org_example_mirai_plugin_CPP_1lib_PluginDisable
(JNIEnv* env, jobject job) {
	genv = env;
	onDisable();
	delete(logger);
	delete(procession);
	delete(config);
	return job;
}
/*
* 消息解析分流
*/
JNIEXPORT jstring JNICALL Java_org_example_mirai_plugin_CPP_1lib_Event
(JNIEnv* env, jobject, jstring content) {
	genv = env;
	std::string Rcontent = tools.jstring2str(content);
	const auto rawJsonLength = static_cast<int>(Rcontent.length());
	Json::String err;
	Json::Value root;
	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(Rcontent.c_str(), Rcontent.c_str() + rawJsonLength, &root,
		&err)) {
		//error
		logger->Error("JSON reader error");
		APIException("JSON reader error").raise();
	}
	try {
		switch (root["type"].asInt()) {
		case 1: {
			//GroupMessage
			procession->broadcast(GroupMessageEvent(
				Group(root["groupid"].asLargestUInt()),
				Member(root["senderid"].asLargestUInt(), root["groupid"].asLargestUInt()),
				root["message"].asCString(),
				MessageSource(root["Source"].asCString())
			)
			);
			return tools.str2jstring("MIRAICP_NULL");
		}
		case 2: {
			//私聊消息
			procession->broadcast(PrivateMessageEvent(
				Friend(root["senderid"].asLargestUInt()),
				root["message"].asCString(),
				MessageSource(root["Source"].asCString())
			)
			);
			return tools.str2jstring("MIRAICP_NULL");
		}
		case 3:
			//群聊邀请
			return tools.str2jstring(procession->broadcast(GroupInviteEvent(
				Group(root["groupid"].asLargestUInt()),
				Friend(root["invitorid"].asLargestUInt()))).c_str());
		case 4:
			//好友
			return tools.str2jstring(procession->broadcast(NewFriendRequestEvent(
				Friend(root["friendid"].asLargestUInt()),
				root["message"].asCString())).c_str());
		case 5:
			//新成员加入
			procession->broadcast(MemberJoinEvent(
				root["jointype"].asInt(),
				Member(root["memberid"].asLargestUInt(),
					root["groupid"].asLargestUInt()),
				Group(root["groupid"].asLargestUInt()),
				Member(
					root["invitorid"].asLargestUInt(),
					root["groupid"].asLargestUInt())
			));
			return tools.str2jstring("MIRAICP_NULL");
		case 6:
			//群成员退出
			procession->broadcast(MemberLeaveEvent(
				root["leavetype"].asInt(),
				Member(root["memberid"].asLargestUInt(),
					root["groupid"].asLargestUInt()),
				Group(root["groupid"].asLargestUInt()),
				Member(
					root["operatorid"].asLargestUInt(),
					root["groupid"].asLargestUInt())
			));
			return tools.str2jstring("MIRAICP_NULL");
		case 7:
			procession->broadcast(RecallEvent(
				root["Etype"].asInt(),
				root["time"].asInt(),
				root["authorid"].asLargestUInt(),
				root["operatorid"].asLargestUInt(),
				root["ids"].asCString(),
				root["internalids"].asCString(),
				root["groupid"].asLargestUInt()
			));
			return tools.str2jstring("MIRAICP_NULL");
		case 8:
			procession->broadcast(SchedulingEvent(
				root["message"].asCString()));
			return tools.str2jstring("MIRAICP_NULL");
		}
	}catch (MiraiCPException& e) {
		e.raise();
		return tools.str2jstring("MIRAICP_NULL");
	}
	logger->Error("unknown type");
	APIException("未知的消息类型").raise();
	return tools.str2jstring("ERROR");
}
