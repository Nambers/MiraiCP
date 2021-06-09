#include "pch.h"
using json = nlohmann::json;
using string = std::string;

///全局javavm指针
JavaVM* gvm = nullptr;
///JNI版本
int JNIVersion = 0;
///全局日志指针
Logger* logger = new Logger();
///全局事件指针
Event* procession = new Event();
///全局配置指针
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
JNIEXPORT jstring JNICALL Java_tech_eritquearcus_miraicp_shared_CPP_1lib_Verify(JNIEnv* env, jobject) {
	manager->setEnv(env);
	env->GetJavaVM(&gvm);
	JNIVersion = (int)env->GetVersion();
	try {
		//初始化日志模块
        config->Init();
		logger->init();
		onEnable();
	}
	catch (MiraiCPException& e) {
		e.raise();
	}
	return Tools::str2jstring(MiraiCPVersion.c_str());//验证机制，返回当前SDK版本
}
/* 插件结束事件*/
JNIEXPORT jobject JNICALL Java_tech_eritquearcus_miraicp_shared_CPP_1lib_PluginDisable
(JNIEnv* env, jobject job) {
	manager->setEnv(env);
	onDisable();
	delete (logger);
	delete (procession);
	delete (config);
	gvm->DestroyJavaVM();
	return job;
}

jstring returnNull() {
	return Tools::str2jstring("MIRAICP_NULL");
}
/*
* 消息解析分流
*/
JNIEXPORT jstring JNICALL Java_tech_eritquearcus_miraicp_shared_CPP_1lib_Event
(JNIEnv* env, jobject, jstring content) {
	manager->setEnv(env);
	string tmp = Tools::jstring2str(content, env);
	json j;
	try {
		j = json::parse(tmp);
	}
	catch (json::parse_error& e) {
		APIException("格式化json错误, JNIEXPORT jstring JNICALL Java_tech_eritquearcus_miraicp_CPP_1lib_Event").raise();
		logger->Error("For debug:" + j.dump());
		logger->Error(e.what());
		return returnNull();
	}
	try {
		switch ((int)j["type"]) {
            case 1: {
                //GroupMessage
                procession->broadcast<GroupMessageEvent>(
                        GroupMessageEvent(j["group"]["botid"],
                                          Group::deserializationFromJson(j["group"]),
                                          Member::deserializationFromJson(j["member"]),
                                          j["message"],
                                          MessageSource::deserializeFromString(j["source"])
                        )
                );
                break;
            }
            case 2: {
                //私聊消息
                procession->broadcast<PrivateMessageEvent>(
                        PrivateMessageEvent(j["friend"]["botid"],
                                            Friend::deserializationFromJson(j["friend"]),
                                            j["message"],
                                            MessageSource::deserializeFromString(j["source"])
                        ));
                break;
            }
            case 3:
                //群聊邀请
                procession->broadcast<GroupInviteEvent>(
                        GroupInviteEvent(
                                j["botid"],
                                j["source"],
                                j["source"]["inviternick"],
                                j["source"]["inviterid"],
                                j["source"]["groupname"],
                                j["source"]["groupid"]
                        ));
                break;
            case 4:
                //好友
                procession->broadcast<NewFriendRequestEvent>(
                        NewFriendRequestEvent(
                                j["source"]["botid"],
                                j["source"].dump(),
                                j["source"]["fromid"],
                                j["source"]["fromgroupid"],
                                j["source"]["fromnick"],
                                j["source"]["message"]
                        ));
                break;
            case 5:
                //新成员加入
                procession->broadcast<MemberJoinEvent>(
                        MemberJoinEvent(
                                j["group"]["botid"],
                                j["jointype"],
                                Member::deserializationFromJson(j["member"]),
                                Group::deserializationFromJson(j["group"]),
                                j["inviterid"]
                        ));
                break;
            case 6:
                //群成员退出
                procession->broadcast<MemberLeaveEvent>(MemberLeaveEvent(
                        j["group"]["botid"],
                        j["leavetype"],
                        j["memberid"],
                        Group::deserializationFromJson(j["group"]),
                        j["operatorid"]
                ));
                break;
            case 7:
                procession->broadcast<RecallEvent>(RecallEvent(
                        j["botid"],
                        j["etype"],
                        j["time"],
                        j["authorid"],
                        j["operatorid"],
                        j["ids"],
                        j["internalids"],
                        j["groupid"]
                ));
                break;
            case 9:
                procession->broadcast<BotJoinGroupEvent>(BotJoinGroupEvent(
                        j["group"]["botid"],
                        j["etype"],
                        Group::deserializationFromJson(j["group"]),
                        j["inviterid"]
                ));
                break;
            case 10:
                procession->broadcast<GroupTempMessageEvent>(GroupTempMessageEvent(
                        j["group"]["botid"],
                        Group::deserializationFromJson(j["group"]),
                        Member::deserializationFromJson(j["member"]),
                        j["message"],
                        MessageSource::deserializeFromString(j["source"])
                ));
                break;
            default:
                throw APIException("Unreachable code");
        }
	}
	catch (json::type_error& e) {
		logger->Error("json格式化异常,位置C-Handle");
		logger->Error(j.dump());
		logger->Error(e.what());
		return Tools::str2jstring("ERROR");
	}catch(MiraiCPException &e){
	    e.raise();
	}
	return returnNull();
}
