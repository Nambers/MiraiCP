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

#include "Event.h"
#include "Command.h"
#include "Exception.h"
#include "KtOperation.h"
#include "Tools.h"

namespace MiraiCP {
    using json = nlohmann::json;

    Event Event::processor;

    void GroupInviteEvent::operation0(const std::string &source, QQID botid, bool accept) {
        nlohmann::json j{{"text", source}, {"accept", accept}, {"botid", botid}};
        std::string re = KtOperation::ktOperation(KtOperation::Gioperation, std::move(j));
        if (re == "E") Logger::logger.error("群聊邀请事件同意失败(可能因为重复处理),id:" + source);
    }

    GroupInviteEvent::GroupInviteEvent(nlohmann::json j) : BotEvent(j["source"]["botid"]),
                                                           source(Tools::json_stringmover(j, "request")),
                                                           inviterNick(Tools::json_stringmover(j["source"], "inviternick")),
                                                           groupName(Tools::json_stringmover(j["source"], "groupname")),
                                                           groupid(j["source"]["groupid"]),
                                                           inviterid(j["source"]["inviterid"]) {
    }

    MessageChain PrivateMessageEvent::nextMessage(long time, bool halt) const {
        json j{{"contactSource", this->sender.toString()}, {"time", time}, {"halt", halt}};
        std::string r = KtOperation::ktOperation(KtOperation::NextMsg, std::move(j));
        if (r == "E1")
            throw TimeOutException("取下一条信息超时", MIRAICP_EXCEPTION_WHERE);
        json re = json::parse(r);
        return MessageChain::deserializationFromMessageSourceJson(json::parse(re["messageSource"].get<std::string>())).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    PrivateMessageEvent::PrivateMessageEvent(nlohmann::json j) : BotEvent(j["friend"]["botid"]),
                                                                 sender(Contact::deserialize<Friend>(Tools::json_jsonmover(j, "friend"))),
                                                                 message(MessageChain::deserializationFromMessageSourceJson(
                                                                                 json::parse(j["source"].get<std::string>()))
                                                                                 .plus(MessageSource::deserializeFromString(
                                                                                         j["source"].get<std::string>()))) {
    }

    MessageChain GroupMessageEvent::nextMessage(long time, bool halt) const {
        json j{{"contactSource", this->group.toString()}, {"time", time}, {"halt", halt}};
        std::string r = KtOperation::ktOperation(KtOperation::NextMsg, std::move(j));
        if (r == "E1")
            throw TimeOutException("取下一条信息超时", MIRAICP_EXCEPTION_WHERE);
        json re = json::parse(r);
        return MessageChain::deserializationFromMessageSourceJson(json::parse(re["messageSource"].get<std::string>())).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    MessageChain GroupMessageEvent::senderNextMessage(long time, bool halt) const {
        json j{{"contactSource", this->sender.toString()}, {"time", time}, {"halt", halt}};
        std::string r = KtOperation::ktOperation(KtOperation::NextMsg, std::move(j));
        if (r == "E1")
            throw TimeOutException("取下一条信息超时", MIRAICP_EXCEPTION_WHERE);
        json re = json::parse(r);
        return MessageChain::deserializationFromMessageSourceJson(json::parse(re["messageSource"].get<std::string>())).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    GroupMessageEvent::GroupMessageEvent(nlohmann::json j) : BotEvent(j["group"]["botid"]),
                                                             group(Contact::deserialize<Group>(Tools::json_jsonmover(j, "group"))),
                                                             sender(Tools::json_jsonmover(j, "member")),
                                                             message(MessageChain::deserializationFromMessageSourceJson(
                                                                             json::parse(j["source"].get<std::string>()))
                                                                             .plus(MessageSource::deserializeFromString(
                                                                                     j["source"].get<std::string>()))) {
    }

    void NewFriendRequestEvent::operation0(const std::string &source, QQID botid, bool accept, bool ban) {
        nlohmann::json j{{"text", source}, {"accept", accept}, {"botid", botid}, {"ban", ban}};
        std::string re = KtOperation::ktOperation(KtOperation::Nfroperation, std::move(j));
        if (re == "E") Logger::logger.error("好友申请事件同意失败(可能因为重复处理),id:" + source);
    }
    NewFriendRequestEvent::NewFriendRequestEvent(nlohmann::json j) : BotEvent(j["source"]["botid"]),
                                                                     source(Tools::json_stringmover(j, "request")),
                                                                     fromid(j["source"]["fromid"]),
                                                                     fromgroupid(j["source"]["fromgroupid"]),
                                                                     nick(Tools::json_stringmover(j["source"], "fromnick")),
                                                                     message(Tools::json_stringmover(j["source"], "message")) {
    }

    void MemberJoinRequestEvent::operate(std::string_view s, QQID botid, bool sign, const std::string &msg) {
        nlohmann::json j{{"source", s}, {"botid", botid}, {"sign", sign}, {"msg", msg}};
        KtOperation::ktOperation(KtOperation::MemberJoinRequest, std::move(j));
    }

    void Event::incomingEvent(json j, int type) {
        switch (type) {
            case eventTypes::GroupMessageEvent: {
                //GroupMessage
                Event::broadcast(GroupMessageEvent(std::move(j)));
                break;
            }
            case eventTypes::PrivateMessageEvent: {
                //私聊消息
                Event::broadcast(PrivateMessageEvent(std::move(j)));
                break;
            }
            case eventTypes::GroupInviteEvent: {
                //群聊邀请
                Event::broadcast(GroupInviteEvent(std::move(j)));
                break;
            }
            case eventTypes::NewFriendRequestEvent: {
                //好友
                Event::broadcast(NewFriendRequestEvent(std::move(j)));
                break;
            }
            case eventTypes::MemberJoinEvent: { //新成员加入
                Event::broadcast(MemberJoinEvent(std::move(j)));
                break;
            }
            case eventTypes::MemberLeaveEvent: {
                //群成员退出
                Event::broadcast(MemberLeaveEvent(std::move(j)));
                break;
            }
            case eventTypes::RecallEvent: {
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
            }
            case eventTypes::BotJoinGroupEvent: {
                Event::broadcast<BotJoinGroupEvent>(BotJoinGroupEvent(
                        j["group"]["botid"],
                        Contact::deserialize<Group>(j["group"]),
                        j["inviterid"],
                        j["etype"]));
                break;
            }
                ////////////////
            case eventTypes::GroupTempMessageEvent: {
                Event::broadcast<GroupTempMessageEvent>(GroupTempMessageEvent(
                        j["group"]["botid"],
                        Contact::deserialize<Group>(j["group"]),
                        Contact::deserialize<Member>(j["member"]),
                        MessageChain::deserializationFromMessageSourceJson(json::parse(j["message"].get<std::string>()))
                                .plus(MessageSource::deserializeFromString(j["source"]))));
                break;
            }
            case eventTypes::TimeOutEvent: {
                Event::broadcast(TimeOutEvent(j["msg"].get<std::string>()));
                break;
            }
            case eventTypes::BotOnlineEvent: {
                Event::broadcast(BotOnlineEvent(j["botid"].get<QQID>()));
                break;
            }
            case eventTypes::NudgeEvent: {
                Event::broadcast(NudgeEvent(Contact::deserializeToPointer(j["from"]),
                                            Contact::deserializeToPointer(j["target"]),
                                            Contact::deserializeToPointer(j["subject"]),
                                            j["botid"]));
                break;
            }
            case eventTypes::BotLeaveEvent: {
                Event::broadcast(BotLeaveEvent(j["groupid"], j["botid"], j["type"], j["operatorid"]));
                break;
            }
            case eventTypes::MemberJoinRequestEvent: {
                Group a = Contact::deserialize<Group>(j["group"]);
                std::optional<Member> b = std::nullopt;
                //                if (j["group"]["id"] != 0)
                //                    a.emplace(Contact::deserialize<Group>(j["group"]));
                if (j["inviter"]["id"] != 0)
                    b.emplace(Contact::deserialize<Member>(j["inviter"]));
                Event::broadcast(MemberJoinRequestEvent(a, b, a.botid(), j["requester"], j["requestData"]));
                break;
            }
            case eventTypes::MessagePreSendEvent: {
                Event::broadcast(MessagePreSendEvent(Contact::deserializeToPointer(j["target"]), MessageChain::deserializationFromMessageSourceJson(j["message"].get<std::string>(), false), j["botid"]));
                break;
            }
            case eventTypes::Command: {
                // command
                std::shared_ptr<Contact> c;
                if (j.contains("contact")) c = Contact::deserializeToPointer(j["contact"]);
                CommandManager::commandManager[j["bindId"]]->onCommand(std::move(c), Bot(j["botid"]), MessageChain::deserializationFromMessageSourceJson((j.contains("message") ? j["message"].get<std::string>() : ""), false));
                break;
            }
            default: {
                throw APIException("Unreachable code", MIRAICP_EXCEPTION_WHERE);
            }
        }
    }

    MemberJoinEvent::MemberJoinEvent(nlohmann::json j) : BotEvent(j["group"]["botid"]),
                                                         type(joinType(j["jointype"].get<int>())),
                                                         member(Contact::deserialize<Member>(Tools::json_jsonmover(j, "member"))),
                                                         group(Contact::deserialize<Group>(Tools::json_jsonmover(j, "group"))),
                                                         inviterid(j["inviterid"]) {
    }

    MemberLeaveEvent::MemberLeaveEvent(nlohmann::json j) : BotEvent(j["group"]["botid"]),
                                                           memberid(j["memberid"]),
                                                           group(Contact::deserialize<Group>(Tools::json_jsonmover(j, "group"))),
                                                           operaterid(j["operatorid"]),
                                                           type(j["leavetype"]) {
    }
} // namespace MiraiCP
