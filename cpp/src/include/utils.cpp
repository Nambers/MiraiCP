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
#include "KtOperation.h"
#include "Event.h"
#include "Exception.h"
#include "Tools.h"
#include "loaderApi.h"

// 开始对接JNI接口代码

using json = nlohmann::json;

namespace LibLoader::LoaderApi {
    const interface_funcs *get_loader_apis();

    void set_loader_apis(const LibLoader::LoaderApi::interface_funcs *apis);

    void reset_loader_apis();
} // namespace LibLoader::LoaderApi

extern "C" {
/// 插件开启入口
void FUNC_ENTRANCE(const LibLoader::LoaderApi::interface_funcs &funcs) {
    static_assert(std::is_same_v<decltype(&FUNC_ENTRANCE), LibLoader::plugin_entrance_func_ptr>);
    using namespace MiraiCP;

    LibLoader::LoaderApi::set_loader_apis(&funcs);

    try {
        enrollPlugin();
        // plugin == nullptr 无插件实例加载
        if (CPPPlugin::plugin != nullptr)
            CPPPlugin::plugin->onEnable();
    } catch (const MiraiCPExceptionBase &e) {
        e.raise();
    }
}


/// 插件结束(也可能是暂时的disable)
void FUNC_EXIT() {
    static_assert(std::is_same_v<decltype(&FUNC_EXIT), LibLoader::plugin_func_ptr>);

    LibLoader::LoaderApi::reset_loader_apis();

    if (MiraiCP::CPPPlugin::plugin == nullptr) return;
    using namespace MiraiCP;
    try {
        CPPPlugin::plugin->onDisable();
    } catch (const MiraiCPExceptionBase &e) {
        e.raise();
    }
    CPPPlugin::plugin = nullptr;
}


/// 消息解析分流
/// env != null, call from jni
void FUNC_EVENT(std::string content) {
    static_assert(std::is_same_v<decltype(&FUNC_EVENT), LibLoader::plugin_event_func_ptr>);

    using namespace MiraiCP;
    json j;
    try {
        j = json::parse(content);
    } catch (json::parse_error &e) {
        APIException("格式化json错误", MIRAICP_EXCEPTION_WHERE).raise();
        Logger::logger.error("For debug:" + j.dump());
        Logger::logger.error(e.what(), false);
        return;
    }
    int type = j["type"].get<int>();

    if (eventTypes(type) != eventTypes::Command && Event::noRegistered(type)) return;
    try {
        switch (eventTypes(type)) {
            case eventTypes::GroupMessageEvent: {
                //GroupMessage
                Event::broadcast<GroupMessageEvent>(
                        GroupMessageEvent(j["group"]["botid"],
                                          Group(Group::deserialize(j["group"])),
                                          Member(Member::deserialize(j["member"])),
                                          MessageChain::deserializationFromMessageSourceJson(json::parse(j["source"].get<std::string>()))
                                                  .plus(MessageSource::deserializeFromString(j["source"].get<std::string>()))));
                break;
            }
            case eventTypes::PrivateMessageEvent: {
                //私聊消息
                Event::broadcast<PrivateMessageEvent>(
                        PrivateMessageEvent(j["friend"]["botid"],
                                            Friend(Friend::deserialize(j["friend"])),
                                            MessageChain::deserializationFromMessageSourceJson(json::parse(j["source"].get<std::string>()))
                                                    .plus(MessageSource::deserializeFromString(j["source"].get<std::string>()))));
                break;
            }
            case eventTypes::GroupInviteEvent:
                //群聊邀请
                Event::broadcast<GroupInviteEvent>(
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
                Event::broadcast<NewFriendRequestEvent>(
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
                Event::broadcast<MemberJoinEvent>(
                        MemberJoinEvent(
                                j["group"]["botid"],
                                j["jointype"],
                                Member(Member::deserialize(j["member"])),
                                Group(Group::deserialize(j["group"])),
                                j["inviterid"]));
                break;
            case eventTypes::MemberLeaveEvent:
                //群成员退出
                Event::broadcast<MemberLeaveEvent>(MemberLeaveEvent(
                        j["group"]["botid"],
                        j["leavetype"],
                        j["memberid"],
                        Group(Group::deserialize(j["group"])),
                        j["operatorid"]));
                break;
            case eventTypes::RecallEvent:
                Event::broadcast<RecallEvent>(RecallEvent(
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
                Event::broadcast<BotJoinGroupEvent>(BotJoinGroupEvent(
                        j["group"]["botid"],
                        j["etype"],
                        Group(Group::deserialize(j["group"])),
                        j["inviterid"]));
                break;
            case eventTypes::GroupTempMessageEvent:
                Event::broadcast<GroupTempMessageEvent>(GroupTempMessageEvent(
                        j["group"]["botid"],
                        Group(Group::deserialize(j["group"])),
                        Member(Member::deserialize(j["member"])),
                        MessageChain::deserializationFromMessageSourceJson(json::parse(j["message"].get<std::string>()))
                                .plus(MessageSource::deserializeFromString(j["source"]))));
                break;
            case eventTypes::TimeOutEvent:
                Event::broadcast(TimeOutEvent(j["msg"]));
                break;
            case eventTypes::BotOnlineEvent:
                Event::broadcast(BotOnlineEvent(j["botid"]));
                break;
            case eventTypes::NudgeEvent:
                Event::broadcast(NudgeEvent(Contact::deserialize(j["from"]),
                                            Contact::deserialize(j["target"]),
                                            Contact::deserialize(j["subject"]),
                                            j["botid"]));
                break;
            case eventTypes::BotLeaveEvent:
                Event::broadcast(BotLeaveEvent(j["groupid"], j["botid"]));
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
                Event::broadcast(MemberJoinRequestEvent(a, b, temp.botid(), j["requester"], j["requestData"]));
                break;
            }
            case eventTypes::MessagePreSendEvent: {
                Event::broadcast(MessagePreSendEvent(Contact::deserialize(j["target"]), MessageChain::deserializationFromMessageSourceJson(j["message"].get<std::string>(), false), j["botid"]));
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
        Logger::logger.error("info:", content);
    } catch (MiraiCPExceptionBase &e) {
        Event::broadcast<MiraiCPExceptionEvent>(MiraiCPExceptionEvent(&e));
        e.raise();
    } catch (const std::exception &e) {
        // 这里如果不catch全部exception就会带崩jvm
        Logger::logger.error(e.what());
        Logger::logger.error("info:", content);
    }
}

MiraiCP::PluginConfig PLUGIN_INFO() {
    if (MiraiCP::CPPPlugin::plugin == nullptr)
        return {"null"};
    return MiraiCP::CPPPlugin::plugin->config;
}
}
//结束对接JNI接口代码
