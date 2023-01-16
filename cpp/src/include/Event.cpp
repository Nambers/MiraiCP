// Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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

    GroupMessageEvent::GroupMessageEvent(BaseEventData j) : BotEvent(j.botId),
                                                            group(j.subject->id, j.botId),
                                                            sender(j.object->id, j.object->groupId, j.botId),
                                                            message(MessageChain::deserializationFromMessageJson(
                                                                    json::parse(j.eventData["message"].get<std::string>()))
                                                                            .plus(MessageSource::deserializeFromString(
                                                                                    j.eventData["source"].get<std::string>()))) {
    }

    MessageChain GroupMessageEvent::nextMessage(long time, bool halt) const {
        json j{{"contact", this->group.toJson()},
               {"time",    time},
               {"halt",    halt}};
        std::string r = KtOperation::ktOperation(KtOperation::NextMsg, j);
        if (r == "E1")
            throw TimeOutException("取下一条信息超时", MIRAICP_EXCEPTION_WHERE);
        json re = json::parse(r);
        return MessageChain::deserializationFromMessageJson(re["message"]).plus(
                MessageSource::deserializeFromString(re["messageSource"]));
    }

    MessageChain GroupMessageEvent::senderNextMessage(long time, bool halt) const {
        json j{{"contact", this->sender.toJson()},
               {"time",    time},
               {"halt",    halt}};
        std::string r = KtOperation::ktOperation(KtOperation::NextMsg, j);
        if (r == "E1")
            throw TimeOutException("取下一条信息超时", MIRAICP_EXCEPTION_WHERE);
        json re = json::parse(r);
        return MessageChain::deserializationFromMessageJson(
                json::parse(re["message"].get<std::string>())).plus(
                MessageSource::deserializeFromString(re["messageSource"]));
    }

    PrivateMessageEvent::PrivateMessageEvent(BaseEventData j) : BotEvent(j.botId),
                                                                 sender(j.subject->id, j.subject->botId),
                                                                message(MessageChain::deserializationFromMessageJson(
                                                                        json::parse(j.eventData["message"].get<std::string>()))
                                                                                .plus(MessageSource::deserializeFromString(
                                                                                        j.eventData["source"].get<std::string>()))) {
    }

    MessageChain PrivateMessageEvent::nextMessage(long time, bool halt) const {
        json j{{"contact", this->sender.toJson()},
               {"time",    time},
               {"halt",    halt}};
        std::string r = KtOperation::ktOperation(KtOperation::NextMsg, j);
        if (r == "E1")
            throw TimeOutException("取下一条信息超时", MIRAICP_EXCEPTION_WHERE);
        json re = json::parse(r);
        return MessageChain::deserializationFromMessageJson(json::parse(re["message"].get<std::string>()))
                .plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    GroupInviteEvent::GroupInviteEvent(BaseEventData j) : BotEvent(j.botId),
                                                          source(Tools::json_stringmover(j.eventData, "request")),
                                                          inviterNick(
                                                                  Tools::json_stringmover(j.eventData, "invitorNick")),
                                                          groupName(Tools::json_stringmover(j.eventData, "groupName")),
                                                          group(j.subject->id, j.subject->botId),
                                                          inviter(j.object->id, j.object->botId),
                                                          requestEventId(j.eventData["requestEventId"]) {
    }

    void GroupInviteEvent::operation0(const std::string &source, QQID botid, bool accept) {
        nlohmann::json j{{"source", source},
                         {"sign",   accept},
                         {"botId",  botid}};
        std::string re = KtOperation::ktOperation(KtOperation::Gioperation, j);
        if (re == "E") Logger::logger.error("群聊邀请事件同意失败(可能因为重复处理),id:" + source);
    }

    NewFriendRequestEvent::NewFriendRequestEvent(BaseEventData j) : BotEvent(j.botId),
                                                                     source(Tools::json_stringmover(j.eventData, "request")),
                                                                     from(j.object->id, j.object->botId),
                                                                     fromGroup(j.subject == std::nullopt ? std::nullopt : std::optional(Group(j.subject->id, j.subject->botId))),
                                                                     nick(Tools::json_stringmover(j.eventData, "requesterNick")),
                                                                     message(Tools::json_stringmover(j.eventData, "message")),
                                                                     requestEventId(j.eventData["requestEventId"]) {

    }

    void NewFriendRequestEvent::operation0(const std::string &source, QQID botId, bool accept, bool ban) {
        nlohmann::json j{{"source", source},
                         {"sign",   accept},
                         {"botId",  botId},
                         {"ban",    ban}};
        std::string re = KtOperation::ktOperation(KtOperation::Nfroperation, j);
        if (re == "E") Logger::logger.error("好友申请事件同意失败(可能因为重复处理),id:" + source);
    }

    MemberJoinEvent::MemberJoinEvent(BaseEventData j) : BotEvent(j.botId),
                                                        type(joinType(j.eventData["eventType"].get<int>())),
                                                        member(j.object->id, j.subject->groupId, j.botId),
                                                        group(j.subject->id, j.subject->botId),
                                                        inviter(j.object == std::nullopt ? std::nullopt : std::optional(
                                                                Member(j.object->id, j.object->groupId,
                                                                       j.object->botId))) {
    }

    MemberLeaveEvent::MemberLeaveEvent(BaseEventData j) : BotEvent(j.botId),
                                                           member(j.object->id, j.object->groupId, j.object->botId),
                                                           group(j.subject->id, j.subject->botId),
                                                           operater(!j.eventData.contains("operator") ? std::nullopt : std::optional(Member(j.eventData["operator"]["id"], j.eventData["operator"]["groupId"], j.eventData["operator"]["botId"]))),
                                                           type(j.eventData["eventType"]){
    }

    FriendRecallEvent::FriendRecallEvent(BaseEventData j) : BotEvent(j.botId),
                                                            time(j.eventData["messageTime"]),
                                                            author(j.eventData["author"]["id"], j.eventData["author"]["botId"]),
                                                            operater(j.object->id, j.object->botId),
                                                            ids(j.eventData["messageIds"].dump()),
                                                            internalIds(j.eventData["messageInternalIds"].dump()) {
    }
    MemberRecallEvent::MemberRecallEvent(BaseEventData j) : BotEvent(j.subject->botId),
                                                            time(j.eventData["messageTime"]),
                                                            author(j.eventData["author"]["id"], j.eventData["author"]["groupId"], j.eventData["author"]["botId"]),
                                                            operater(j.object->id, j.object->groupId, j.object->botId),
                                                            ids(j.eventData["messageIds"].dump()),
                                                            internalIds(j.eventData["messageInternalIds"].dump()) {
    }

    BotJoinGroupEvent::BotJoinGroupEvent(BaseEventData j) : BotEvent(j.botId),
                                                            group(j.subject->id, j.subject->botId),
                                                            inviter(j.eventData.contains("inviter") ? std::optional(
                                                                    Member(j.eventData["inviter"]["id"],
                                                                           j.eventData["inviter"]["groupId"],
                                                                           j.eventData["inviter"]["botId"]))
                                                                                                    : std::nullopt),
                                                            type(j.eventData["eventType"]) {
    }

    GroupTempMessageEvent::GroupTempMessageEvent(BaseEventData j) : BotEvent(j.botId),
                                                                     group(j.subject->id, j.subject->botId),
                                                                     sender(j.object->id, j.object->groupId, j.object->botId),
                                                                     message(MessageChain::deserializationFromMessageJson(
                                                                             json::parse(j.eventData["message"].get<std::string>()))
                                                                                     .plus(MessageSource::deserializeFromString(
                                                                                             j.eventData["source"]))) {
    }

    NudgeEvent::NudgeEvent(BaseEventData j) : BotEvent(j.botId),
                                              from(j.object->toContactPointer()),
                                              target(BaseEventData::BuiltInContact(
                                                      j.eventData["target"]).toContactPointer()),
                                              subject(j.subject->toContactPointer()) {
    }

    BotLeaveEvent::BotLeaveEvent(BaseEventData j) : BotEvent(j.botId),
                                                     group(j.subject->id, j.subject->botId),
                                                     type(j.eventData["eventType"]),
                                                     operater(j.object->id, j.object->groupId,j.object->botId){
    }

    MemberJoinRequestEvent::MemberJoinRequestEvent(BaseEventData j) : BotEvent(j.botId),
                                                                      source(Tools::json_jsonmover(j.eventData,
                                                                                                   "requestData")),
                                                                      group(j.subject->id, j.subject->botId),
                                                                      inviter(j.eventData.contains("inviter")
                                                                              ? std::optional(
                                                                                      Member(Tools::json_jsonmover(
                                                                                              j.eventData, "inviter")))
                                                                              : std::nullopt),
                                                                      from(j.object->id, j.object->groupId,
                                                                           j.object->botId),
                                                                      fromNick(Tools::json_jsonmover(j.eventData,
                                                                                                     "fromNick")),
                                                                      message(Tools::json_jsonmover(j.eventData,
                                                                                                    "message")) {
    }

    void MemberJoinRequestEvent::operate(std::string_view s, QQID botid, bool sign, const std::string &msg) {
        nlohmann::json j{{"source", s},
                         {"botId",  botid},
                         {"sign",   sign},
                         {"msg",    msg}};
        KtOperation::ktOperation(KtOperation::MemberJoinRequest, j);
    }

    MessagePreSendEvent::MessagePreSendEvent(BaseEventData j) : BotEvent(j.botId),
                                                                target(j.subject->toContactPointer()),
                                                                message(MessageChain::deserializationFromMessageJson(
                                                                        nlohmann::json::parse(
                                                                                Tools::json_stringmover(j.eventData, "message")))) {
    }

    void Event::incomingEvent(BaseEventData j, int type) {
        switch (type) {
            case eventTypes::GroupMessageEvent: {
                Event::broadcast(GroupMessageEvent(std::move(j)));
                break;
            }
            case eventTypes::FriendMessageEvent: {
                Event::broadcast(PrivateMessageEvent(std::move(j)));
                break;
            }
            case eventTypes::GroupTempMessageEvent: {
                Event::broadcast(GroupTempMessageEvent(std::move(j)));
                break;
            }
//          case IMessageEvent::MessageEventType::StrangerMessageEvent: {
//              // todo Implement StrangerMessageEvent (ea)
//          }
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
            case eventTypes::FriendRecallEvent: {
                Event::broadcast(RecallEvent::FriendRecallEvent(std::move(j)));
                break;
            }
            case eventTypes::MemberRecallEvent: {
                Event::broadcast(RecallEvent::MemberRecallEvent(std::move(j)));
                break;
            }
            case eventTypes::BotJoinGroupEvent: {
                Event::broadcast(BotJoinGroupEvent(std::move(j)));
                break;
            }
            case eventTypes::TimeOutEvent: {
                Event::broadcast(TimeOutEvent(Tools::json_stringmover(j.eventData, "msg")));
                break;
            }
            case eventTypes::BotOnlineEvent: {
                Event::broadcast(BotOnlineEvent(j.botId));
                break;
            }
            case eventTypes::NudgeEvent: {
                Event::broadcast(NudgeEvent(std::move(j)));
                break;
            }
            case eventTypes::BotLeaveEvent: {
                Event::broadcast(BotLeaveEvent(std::move(j)));
                break;
            }
            case eventTypes::MemberJoinRequestEvent: {
                Event::broadcast(MemberJoinRequestEvent(std::move(j)));
                break;
            }
            case eventTypes::MessagePreSendEvent: {
                Event::broadcast(MessagePreSendEvent(std::move(j)));
                break;
            }
            case eventTypes::Command: {
                // command
                CommandManager::commandManager[j.eventData["bindId"]]->onCommand(
                        j.eventData.contains("contact") ? Contact::deserializeToPointer(Tools::json_jsonmover(j.eventData, "contact")) : nullptr,
                        Bot(j.botId),
                        MessageChain::deserializationFromMessageJson(
                                j.eventData.contains("message") ? Tools::json_jsonmover(j.eventData, "message") : ""));
                break;
            }
            default: {
                throw APIException("Unreachable code", MIRAICP_EXCEPTION_WHERE);
            }
        }
    }

    BaseEventData::BaseEventData(nlohmann::json j) {
        this->botId = 0;
        if (j.contains("subject")) {
            this->botId = j["subject"]["botId"].get<QQID>();
            this->subject = BuiltInContact(Tools::json_jsonmover(j, "subject"));

        }
        if (j.contains("object")) {
            this->botId = j["object"]["botId"].get<QQID>();
            this->object = BuiltInContact(Tools::json_jsonmover(j, "object"));
        }
        this->eventData = Tools::json_jsonmover(j, "eventData");
    }

    BaseEventData::BuiltInContact::BuiltInContact(nlohmann::json in_json) {
        if (in_json.empty()) {
            return;
        }
        this->botId = in_json["botId"];
        this->id = in_json["id"];
        if (in_json.contains("groupId"))
            this->groupId = in_json["groupId"];
        this->type = ContactType(in_json["type"]);
    }

    std::shared_ptr<Contact> BaseEventData::BuiltInContact::toContactPointer() {
        switch (this->type) {
            case ContactType::TypeFriend:
                return std::make_shared<Friend>(this->id, this->botId);
            case ContactType::TypeGroup:
                return std::make_shared<Group>(this->id, this->botId);
            case ContactType::TypeMember:
                return std::make_shared<Member>(this->id, this->groupId, this->botId);
            case ContactType::TypeBot:
                // bot
                return std::make_shared<Bot>(this->id);
            case ContactType::TypeStranger:
                // todo Implement Stranger (ea)
            case ContactType::TypeAnonymousMember:
                // todo anonymous member
            default:
                throw APIException("Type of builtInContact doesn't match or implement", MIRAICP_EXCEPTION_WHERE);
        }
    }
} // namespace MiraiCP
