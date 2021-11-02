#include "utils.h"
#include "Event.h"
#include "Exception.h"

// 开始对接JNI接口代码

/*
* 名称:Java_com_example_plugin_CPP_1lib_Verify
* 作用:判断是否连接上本插件，勿改
* 参数:env 必备，job 必备
* 返回值:jstring (用str2jstring把string类型转成jsrting) 发送返回的字符串
*/
JNIEXPORT jstring
Verify(JNIEnv *env, jobject) {
    using namespace MiraiCP;
    ThreadManager::setEnv(env);
    MiraiCP::ThreadManager::JNIVersion = env->GetVersion();
    try {
        //初始化日志模块
        Config::construct();
        Logger::logger.init();
        enrollPlugin();
        if (CPPPlugin::plugin == nullptr) {
            Logger::logger.error("无插件实例加载");
        } else {
            CPPPlugin::pluginLogger = new PluginLogger(&Logger::logger);
            CPPPlugin::plugin->onEnable();
        }
    } catch (MiraiCPException &e) {
        e.raise();
    }
    json j = CPPPlugin::plugin->config.serialize();
    j["MiraiCPversion"] = MiraiCPVersion;
    return Tools::str2jstring(j.dump().c_str());
    //验证机制，返回当前SDK版本
}

/* 插件结束事件*/
JNIEXPORT jobject
PluginDisable(JNIEnv *env, jobject job) {
    using namespace MiraiCP;
    ThreadManager::setEnv(env);
    CPPPlugin::plugin->onDisable();
    CPPPlugin::plugin = nullptr;
    return job;
}

/*返回空值*/
JNIEXPORT jstring

returnNull() {
    return MiraiCP::Tools::str2jstring("MIRAICP_NULL");
}

/*
* 消息解析分流
*/
JNIEXPORT jstring
Event(JNIEnv *env, jobject, jstring content) {
    using namespace MiraiCP;
    ThreadManager::setEnv(env);
    std::string tmp = Tools::jstring2str(content, env);
    json j;
    try {
        j = json::parse(tmp);
    } catch (json::parse_error &e) {
        APIException("格式化json错误").raise();
        Logger::logger.error("For debug:" + j.dump());
        Logger::logger.error(e.what(), false);
        return returnNull();
    }
    try {
        switch ((int) j["type"]) {
            case 1: {
                //GroupMessage
                Event::processor.broadcast<GroupMessageEvent>(
                        GroupMessageEvent(j["group"]["botid"],
                                          Group(Group::deserializationFromJson(j["group"])),
                                          Member(Member::deserializationFromJson(j["member"])),
                                          MessageChain::deserializationFromMiraiCode(j["message"].get<std::string>())
                                                  .plus(MessageSource::deserializeFromString(j["source"]))));
                break;
            }
            case 2: {
                //私聊消息
                Event::processor.broadcast<PrivateMessageEvent>(
                        PrivateMessageEvent(j["friend"]["botid"],
                                            Friend(Friend::deserializationFromJson(j["friend"])),
                                            MessageChain::deserializationFromMiraiCode(j["message"])
                                                    .plus(MessageSource::deserializeFromString(j["source"]))));
                break;
            }
            case 3:
                //群聊邀请
                Event::processor.broadcast<GroupInviteEvent>(
                        GroupInviteEvent(
                                j["source"]["botid"],
                                j["request"],
                                j["source"]["inviternick"],
                                j["source"]["inviterid"],
                                j["source"]["groupname"],
                                j["source"]["groupid"]));
                break;
            case 4:
                //好友
                Event::processor.broadcast<NewFriendRequestEvent>(
                        NewFriendRequestEvent(
                                j["source"]["botid"],
                                j["request"],
                                j["source"]["fromid"],
                                j["source"]["fromgroupid"],
                                j["source"]["fromnick"],
                                j["source"]["message"]));
                break;
            case 5:
                //新成员加入
                Event::processor.broadcast<MemberJoinEvent>(
                        MemberJoinEvent(
                                j["group"]["botid"],
                                j["jointype"],
                                Member(Member::deserializationFromJson(j["member"])),
                                Group(Group::deserializationFromJson(j["group"])),
                                j["inviterid"]));
                break;
            case 6:
                //群成员退出
                Event::processor.broadcast<MemberLeaveEvent>(MemberLeaveEvent(
                        j["group"]["botid"],
                        j["leavetype"],
                        j["memberid"],
                        Group(Group::deserializationFromJson(j["group"])),
                        j["operatorid"]));
                break;
            case 7:
                Event::processor.broadcast<RecallEvent>(RecallEvent(
                        j["botid"],
                        j["etype"],
                        j["time"],
                        j["authorid"],
                        j["operatorid"],
                        j["ids"],
                        j["internalids"],
                        j["groupid"]));
                break;
            case 9:
                Event::processor.broadcast<BotJoinGroupEvent>(BotJoinGroupEvent(
                        j["group"]["botid"],
                        j["etype"],
                        Group(Group::deserializationFromJson(j["group"])),
                        j["inviterid"]));
                break;
            case 10:
                Event::processor.broadcast<GroupTempMessageEvent>(GroupTempMessageEvent(
                        j["group"]["botid"],
                        Group(Group::deserializationFromJson(j["group"])),
                        Member(Member::deserializationFromJson(j["member"])),
                        MessageChain::deserializationFromMiraiCode(j["message"])
                                .plus(MessageSource::deserializeFromString(j["source"]))));
                break;
            case 11:
                Event::processor.broadcast<BotOnlineEvent>(BotOnlineEvent(j["botid"]));
                break;
            case 12:
                Event::processor.broadcast<TimeOutEvent>(TimeOutEvent(j["msg"]));
                break;
            case 13:
                Event::processor.broadcast<NudgeEvent>(NudgeEvent(Contact::deserializationFromJson(j["from"]),
                                                                  Contact::deserializationFromJson(j["target"]),
                                                                  j["botid"]));
                break;
            case 14:
                Event::processor.broadcast(BotLeaveEvent(j["groupid"], j["botid"]));
                break;
            case 15: {
                std::optional<Group> a;
                std::optional<Member> b;
                Contact temp = Contact::deserializationFromJson(j["group"]);
                if (temp.id() == 0)
                    a = std::nullopt;
                else
                    a = Group(temp);
                temp = Contact::deserializationFromJson(j["inviter"]);
                if (temp.id() == 0)
                    b = std::nullopt;
                else
                    b = Member(temp);
                Event::processor.broadcast(MemberJoinRequestEvent(a, b, temp.botid(), j["requestData"]));
                break;
            }
            default:
                MiraiCPThrow(APIException("Unreachable code"));
        }
    } catch (json::type_error &e) {
        Logger::logger.error("json格式化异常,位置C-Handle");
        Logger::logger.error(e.what(), false);
        return Tools::str2jstring("ERROR");
    } catch (MiraiCPException &e) {
        Logger::logger.error("MiraiCP error:" + e.what());
        return Tools::str2jstring("ERROR");
    } catch (std::exception &e) {
        // 这里如果不catch全部exception就会带崩jvm
        Logger::logger.error(e.what());
        return Tools::str2jstring("ERROR");
    }
    return returnNull();
}

int registerMethods(JNIEnv *env, const char *className, JNINativeMethod *gMethods, int numMethods) {
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        return JNI_FALSE;
    }
    //注册native方法
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

JNINativeMethod method_table[] = {
        {(char *) "Verify", (char *) "()Ljava/lang/String;", (jstring *) Verify},
        {(char *) "Event", (char *) "(Ljava/lang/String;)Ljava/lang/String;", (jstring *) Event},
        {(char *) "PluginDisable", (char *) "()Ljava/lang/Void;", (jobject *) PluginDisable}};

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    assert(env != nullptr);
    MiraiCP::ThreadManager::gvm = vm;
    // 注册native方法
    if (!registerMethods(env, "tech/eritquearcus/miraicp/shared/CPP_lib", method_table, 3)) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

//结束对接JNI接口代码
