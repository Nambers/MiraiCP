// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or any later version(in your opinion).
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "utils.h"
#include "Command.h"
#include "Config.h"
#include "Event.h"
#include "Exception.h"
#include "ThreadManager.h"
#include "Tools.h"

// 开始对接JNI接口代码

/*
* 名称:Java_com_example_plugin_CPP_1lib_Verify
* 作用:判断是否连接上本插件，勿改
* 参数:env 必备，job 必备
* 返回值:jstring (用str2jstring把string类型转成jsrting) 发送返回的字符串
*/
using json = nlohmann::json;
namespace MiraiCP::JNIApi {
    // env != null, call from jni
    JNIEXPORT jstring Verify(JNIEnv *env, jobject, jstring id) {
        using namespace MiraiCP;
        ThreadManager::setEnv(env);
        MiraiCP::ThreadManager::JNIVersion = env->GetVersion();
        try {
            //初始化日志模块
            Config::construct();
            Logger::logger.init();
            enrollPlugin();
            // plugin == nullptr 无插件实例加载
            if (CPPPlugin::plugin != nullptr) {
                CPPPlugin::pluginLogger = new PluginLogger(&Logger::logger);
                CPPPlugin::plugin->onEnable();
            }
        } catch (const MiraiCPExceptionBase &e) {
            e.raise();
        }
        json j = (CPPPlugin::plugin != nullptr) ? CPPPlugin::plugin->config.serialize() : json::parse("{}");
        j["MiraiCPversion"] = MiraiCPVersion;
        Config::pluginId = std::stoi(Tools::jstring2str(id));
        return Tools::str2jstring(j.dump().c_str());
        //验证机制，返回当前SDK版本
    }

    /*
 * 插件结束事件
 * env != null, call from jni
 */
    JNIEXPORT jobject PluginDisable(JNIEnv *env, jobject job) {
        using namespace MiraiCP;
        ThreadManager::setEnv(env);
        try {
            CPPPlugin::plugin->onDisable();
        } catch (const MiraiCPExceptionBase &e) {
            e.raise();
        }
        CPPPlugin::plugin = nullptr;
        return job;
    }

    /*返回空值*/
    JNIEXPORT jstring returnNull() {
        return MiraiCP::Tools::str2jstring("MIRAICP_NULL");
    }

    /*
* 消息解析分流
 * env != null, call from jni
*/
    JNIEXPORT jstring Event(JNIEnv *env, jobject, jstring content) {
        using namespace MiraiCP;
        ThreadManager::setEnv(env);
        std::string tmp = Tools::jstring2str(content, env);
        json j;
        try {
            j = json::parse(tmp);
        } catch (json::parse_error &e) {
            APIException("格式化json错误", MIRAICP_EXCEPTION_WHERE).raise();
            Logger::logger.error("For debug:" + j.dump());
            Logger::logger.error(e.what(), false);
            return returnNull();
        }
        int type = j["type"].get<int>();

        if (eventTypes(type) != eventTypes::Command && Event::processor.noRegistered(type)) return returnNull();
        try {
            switch (eventTypes(type)) {
                case eventTypes::GroupMessageEvent: {
                    //GroupMessage
                    Event::processor.broadcast<GroupMessageEvent>(
                            GroupMessageEvent(j["group"]["botid"],
                                              Group(Group::deserialize(j["group"])),
                                              Member(Member::deserialize(j["member"])),
                                              MessageChain::deserializationFromMessageSourceJson(json::parse(j["source"].get<std::string>()))
                                                      .plus(MessageSource::deserializeFromString(j["source"].get<std::string>()))));
                    break;
                }
                case eventTypes::PrivateMessageEvent: {
                    //私聊消息
                    Event::processor.broadcast<PrivateMessageEvent>(
                            PrivateMessageEvent(j["friend"]["botid"],
                                                Friend(Friend::deserialize(j["friend"])),
                                                MessageChain::deserializationFromMessageSourceJson(json::parse(j["source"].get<std::string>()))
                                                        .plus(MessageSource::deserializeFromString(j["source"].get<std::string>()))));
                    break;
                }
                case eventTypes::GroupInviteEvent:
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
                case eventTypes::NewFriendRequestEvent:
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
                case eventTypes::MemberJoinEvent:
                    //新成员加入
                    Event::processor.broadcast<MemberJoinEvent>(
                            MemberJoinEvent(
                                    j["group"]["botid"],
                                    j["jointype"],
                                    Member(Member::deserialize(j["member"])),
                                    Group(Group::deserialize(j["group"])),
                                    j["inviterid"]));
                    break;
                case eventTypes::MemberLeaveEvent:
                    //群成员退出
                    Event::processor.broadcast<MemberLeaveEvent>(MemberLeaveEvent(
                            j["group"]["botid"],
                            j["leavetype"],
                            j["memberid"],
                            Group(Group::deserialize(j["group"])),
                            j["operatorid"]));
                    break;
                case eventTypes::RecallEvent:
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
                case eventTypes::BotJoinGroupEvent:
                    Event::processor.broadcast<BotJoinGroupEvent>(BotJoinGroupEvent(
                            j["group"]["botid"],
                            j["etype"],
                            Group(Group::deserialize(j["group"])),
                            j["inviterid"]));
                    break;
                case eventTypes::GroupTempMessageEvent:
                    Event::processor.broadcast<GroupTempMessageEvent>(GroupTempMessageEvent(
                            j["group"]["botid"],
                            Group(Group::deserialize(j["group"])),
                            Member(Member::deserialize(j["member"])),
                            MessageChain::deserializationFromMessageSourceJson(json::parse(j["message"].get<std::string>()))
                                    .plus(MessageSource::deserializeFromString(j["source"]))));
                    break;
                case eventTypes::TimeOutEvent:
                    Event::processor.broadcast(TimeOutEvent(j["msg"]));
                    break;
                case eventTypes::BotOnlineEvent:
                    Event::processor.broadcast(BotOnlineEvent(j["botid"]));
                    break;
                case eventTypes::NudgeEvent:
                    Event::processor.broadcast(NudgeEvent(Contact::deserialize(j["from"]),
                                                          Contact::deserialize(j["target"]),
                                                          Contact::deserialize(j["subject"]),
                                                          j["botid"]));
                    break;
                case eventTypes::BotLeaveEvent:
                    Event::processor.broadcast(BotLeaveEvent(j["groupid"], j["botid"]));
                    break;
                case eventTypes::MemberJoinRequestEvent: {
                    std::optional<Group> a;
                    std::optional<Member> b;
                    Contact temp = Contact::deserialize(j["group"]);
                    if (temp.id() == 0)
                        a = std::nullopt;
                    else
                        a = Group(temp);
                    temp = Contact::deserialize(j["inviter"]);
                    if (temp.id() == 0)
                        b = std::nullopt;
                    else
                        b = Member(temp);
                    Event::processor.broadcast(MemberJoinRequestEvent(a, b, temp.botid(), j["requester"], j["requestData"]));
                    break;
                }
                case eventTypes::MessagePreSendEvent: {
                    Event::processor.broadcast(MessagePreSendEvent(Contact::deserialize(j["target"]), MessageChain::deserializationFromMessageSourceJson(j["message"].get<std::string>(), false), j["botid"]));
                    break;
                }
                case eventTypes::Command: {
                    // command
                    std::optional<Contact> c = std::nullopt;
                    if (j.contains("contact")) c = Contact::deserialize(j["contact"]);
                    CommandManager::commandManager[j["bindId"]]->onCommand(c, Bot(j["botid"]), MessageChain::deserializationFromMessageSourceJson((j.contains("message") ? j["message"].get<std::string>() : ""), false));
                    break;
                }
                default:
                    throw APIException("Unreachable code", MIRAICP_EXCEPTION_WHERE);
            }
        } catch (json::type_error &e) {
            Logger::logger.error("json格式化异常,位置C-Handle");
            Logger::logger.error(e.what());
            Logger::logger.error("info:", tmp);
            return Tools::str2jstring("ERROR");
        } catch (const MiraiCPExceptionBase &e) {
            e.raise();
            return Tools::str2jstring("ERROR");
        } catch (const std::exception &e) {
            // 这里如果不catch全部exception就会带崩jvm
            Logger::logger.error(e.what());
            Logger::logger.error("info:", tmp);
            return Tools::str2jstring("ERROR");
        }
        return returnNull();
    }
    // env != null
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
    JNINativeMethod method_table[]{
            {(char *) "Verify", (char *) "(Ljava/lang/String;)Ljava/lang/String;", (jstring *) Verify},
            {(char *) "Event", (char *) "(Ljava/lang/String;)Ljava/lang/String;", (jstring *) Event},
            {(char *) "PluginDisable", (char *) "()Ljava/lang/Void;", (jobject *) PluginDisable}};
} // namespace MiraiCP::JNIApi
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    assert(env != nullptr);
    MiraiCP::ThreadManager::gvm = vm;
    // 注册native方法
    if (!MiraiCP::JNIApi::registerMethods(env, "tech/eritquearcus/miraicp/shared/CPPLib", MiraiCP::JNIApi::method_table, 3)) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

//结束对接JNI接口代码
