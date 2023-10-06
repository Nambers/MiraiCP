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
#include "Events/BotJoinGroupEvent.h"
#include "Events/BotLeaveEvent.h"
#include "Events/BotOnlineEvent.h"
#include "Events/FriendRecallEvent.h"
#include "Events/GroupInviteEvent.h"
#include "Events/GroupMessageEvent.h"
#include "Events/GroupTempMessageEvent.h"
#include "Events/MemberJoinEvent.h"
#include "Events/MemberJoinRequestEvent.h"
#include "Events/MemberLeaveEvent.h"
#include "Events/MemberRecallEvent.h"
#include "Events/MessagePreSendEvent.h"
#include "Events/NewFriendRequestEvent.h"
#include "Events/NudgeEvent.h"
#include "Events/PrivateMessageEvent.h"
#include "Events/TimeOutEvent.h"
#include "Exception.h"
#include "JsonTools.h"
#include "KtOperation.h"
#include "Tools.h"
#include <atomic>


namespace MiraiCP {
    namespace CommandManager {
        extern std::unordered_map<std::string, std::unique_ptr<IRawCommand>> commandList;
    };

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
               {"time", time},
               {"halt", halt}};
        std::string r = KtOperation::ktOperation(KtOperation::NextMsg, j);
        if (r == "E1")
            throw TimeOutException("取下一条信息超时", MIRAICP_EXCEPTION_WHERE);
        json re = json::parse(r);
        return MessageChain::deserializationFromMessageJson(nlohmann::json::parse(json_stringmover(re, "message"))).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    MessageChain GroupMessageEvent::senderNextMessage(long time, bool halt) const {
        json j{{"contact", this->sender.toJson()},
               {"time", time},
               {"halt", halt}};
        std::string r = KtOperation::ktOperation(KtOperation::NextMsg, j);
        if (r == "E1")
            throw TimeOutException("取下一条信息超时", MIRAICP_EXCEPTION_WHERE);
        json re = json::parse(r);
        return MessageChain::deserializationFromMessageJson(
                       json::parse(re["message"].get<std::string>()))
                .plus(
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
               {"time", time},
               {"halt", halt}};
        std::string r = KtOperation::ktOperation(KtOperation::NextMsg, j);
        if (r == "E1")
            throw TimeOutException("取下一条信息超时", MIRAICP_EXCEPTION_WHERE);
        json re = json::parse(r);
        return MessageChain::deserializationFromMessageJson(json::parse(re["message"].get<std::string>()))
                .plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    GroupInviteEvent::GroupInviteEvent(BaseEventData j) : BotEvent(j.botId),
                                                          source(json_stringmover(j.eventData, "request")),
                                                          inviterNick(
                                                                  json_stringmover(j.eventData, "invitorNick")),
                                                          groupName(json_stringmover(j.eventData, "groupName")),
                                                          group(j.subject->id, j.subject->botId),
                                                          inviter(j.object->id, j.object->botId),
                                                          requestEventId(j.eventData["requestEventId"]) {
    }

    void groupInviteInternal(const std::string &source, QQID botid, bool accept) {
        nlohmann::json j{{"source", source},
                         {"sign", accept},
                         {"botId", botid}};
        std::string re = KtOperation::ktOperation(KtOperation::Gioperation, j);
        if (re == "E") Logger::logger.error("群聊邀请事件同意失败(可能因为重复处理),id:" + source);
    }

    void GroupInviteEvent::reject() {
        groupInviteInternal(source, bot.id(), false);
    }

    void GroupInviteEvent::accept() {
        groupInviteInternal(source, bot.id(), true);
    }

    NewFriendRequestEvent::NewFriendRequestEvent(BaseEventData j) : BotEvent(j.botId),
                                                                    source(json_stringmover(j.eventData, "request")),
                                                                    from(j.object->id, j.object->botId),
                                                                    fromGroup(j.subject == std::nullopt ? std::nullopt : std::optional(Group(j.subject->id, j.subject->botId))),
                                                                    nick(json_stringmover(j.eventData, "requesterNick")),
                                                                    message(json_stringmover(j.eventData, "message")),
                                                                    requestEventId(j.eventData["requestEventId"]) {
    }

    void newFriendRequestInternal(const std::string &source, QQID botId, bool accept, bool ban = false) {
        nlohmann::json j{{"source", source},
                         {"sign", accept},
                         {"botId", botId},
                         {"ban", ban}};
        std::string re = KtOperation::ktOperation(KtOperation::Nfroperation, j);
        if (re == "E") Logger::logger.error("好友申请事件同意失败(可能因为重复处理),id:" + source);
    }

    void NewFriendRequestEvent::reject(bool ban) {
        newFriendRequestInternal(source, bot.id(), false, ban);
    }

    void NewFriendRequestEvent::accept() {
        newFriendRequestInternal(source, bot.id(), true);
    }

    MemberJoinEvent::MemberJoinEvent(BaseEventData j) : BotEvent(j.botId),
                                                        type(joinType(j.eventData["eventType"].get<int>())),
                                                        member(j.object->id, j.object->groupId, j.object->botId),
                                                        group(j.subject->id, j.subject->botId),
                                                        inviter(!j.eventData.contains("invitor") ? std::nullopt : std::optional(Member(j.eventData["invitor"]["id"], j.eventData["invitor"]["groupId"], j.eventData["invitor"]["botId"]))) {
    }

    MemberLeaveEvent::MemberLeaveEvent(BaseEventData j) : BotEvent(j.botId),
                                                          member(j.object->id, j.object->groupId, j.object->botId),
                                                          group(j.subject->id, j.subject->botId),
                                                          operater(!j.eventData.contains("operator") ? std::nullopt : std::optional(Member(j.eventData["operator"]["id"], j.eventData["operator"]["groupId"], j.eventData["operator"]["botId"]))),
                                                          type(j.eventData["eventType"]) {
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
                                                             j.eventData["target"])
                                                             .toContactPointer()),
                                              subject(j.subject->toContactPointer()) {
    }

    BotLeaveEvent::BotLeaveEvent(BaseEventData j) : BotEvent(j.botId),
                                                    group(j.subject->id, j.subject->botId),
                                                    type(j.eventData["eventType"]),
                                                    operater(j.object->id, j.object->groupId, j.object->botId) {
    }

    MemberJoinRequestEvent::MemberJoinRequestEvent(BaseEventData j) : BotEvent(j.botId),
                                                                      source(json_jsonmover(j.eventData,
                                                                                            "requestData")),
                                                                      group(j.subject->id, j.subject->botId),
                                                                      inviter(j.eventData.contains("inviter")
                                                                                      ? std::optional(
                                                                                                Member(json_jsonmover(
                                                                                                        j.eventData, "inviter")))
                                                                                      : std::nullopt),
                                                                      from(j.object->id, j.object->groupId,
                                                                           j.object->botId),
                                                                      fromNick(json_jsonmover(j.eventData,
                                                                                              "fromNick")),
                                                                      message(json_jsonmover(j.eventData,
                                                                                             "message")) {
    }

    /**
     * @brief 底层通过MemberJoinRequest
     * @param s 序列化后的文本
     */
    void memberJoinRequestInternal(std::string_view s, QQID botid, bool sign, std::string msg = "") {
        nlohmann::json j{{"source", s},
                         {"botId", botid},
                         {"sign", sign},
                         {"msg", std::move(msg)}};
        KtOperation::ktOperation(KtOperation::MemberJoinRequest, j);
    }

    void MemberJoinRequestEvent::accept() {
        memberJoinRequestInternal(source, bot.id(), true);
    }

    void MemberJoinRequestEvent::reject(std::string msg) {
        memberJoinRequestInternal(source, bot.id(), false, std::move(msg));
    }

    MessagePreSendEvent::MessagePreSendEvent(BaseEventData j) : BotEvent(j.botId),
                                                                target(j.subject->toContactPointer()),
                                                                message(MessageChain::deserializationFromMessageJson(
                                                                        nlohmann::json::parse(
                                                                                json_stringmover(j.eventData, "message")))) {
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
                Event::broadcast(TimeOutEvent(json_stringmover(j.eventData, "msg")));
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
                if (CommandManager::commandList.find(j.eventData["primaryName"]) != CommandManager::commandList.end()) {
                    CommandManager::commandList[j.eventData["primaryName"]]->onCommand(
                            j.eventData.contains("contact") ? Contact::deserializeToPointer(json_jsonmover(j.eventData, "contact")) : nullptr,
                            Bot(j.botId),
                            MessageChain::deserializationFromMessageJson(json::parse(json_stringmover(j.eventData, "message"))));
                }
                break;
            }
            default: {
                throw APIException("Unreachable code", MIRAICP_EXCEPTION_WHERE);
            }
        }
    }

    Event::Event() : _all_events_(int(eventTypes::Types::count)) {}

    bool Event::internalNoRegistered(int index) const {
        std::shared_lock lk(eventsMtx);
        return _all_events_[index].empty();
    }

    void Event::internalClear() noexcept {
        std::unique_lock lk(eventsMtx);
        for (auto &a: _all_events_) a.clear();
    }

    void Event::internalBroadcast(MiraiCPEvent *event, size_t eventId) const {
        std::shared_lock lk(eventsMtx);
        for (auto &&[k, v]: _all_events_[eventId]) {
            for (auto &&a: v) {
                if (a->run(event)) return;
            }
        }
    }

    NodeHandle *Event::internalRegister(std::function<bool(MiraiCPEvent *)> callback, size_t where, Event::priority_level level) {
        std::unique_lock lk(eventsMtx);
        auto &row = _all_events_[where][level];
        row.emplace_back(std::make_unique<eventNode>(std::move(callback)));
        return row[row.size() - 1]->getHandle();
    }

    BaseEventData::BaseEventData(nlohmann::json j) {
        this->botId = 0;
        if (j.contains("subject")) {
            this->botId = j["subject"]["botId"].get<QQID>();
            this->subject = BuiltInContact(json_jsonmover(j, "subject"));
        }
        if (j.contains("object")) {
            this->botId = j["object"]["botId"].get<QQID>();
            this->object = BuiltInContact(json_jsonmover(j, "object"));
        }
        this->eventData = json_jsonmover(j, "eventData");
    }

    std::string BaseEventData::toString() const {
        std::stringstream ss;
        ss << "{"
           << "botId=" << botId << ", subject=" << subject->toString() << ", object=" << object->toString()
           << ", eventData=" << eventData.dump() << "}";
        return ss.str();
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

    std::string BaseEventData::BuiltInContact::toString() const {
        std::stringstream ss;
        ss << "{"
           << "botId=" << botId << ", id=" << id << ", groupId=" << groupId << ", type=" << type << "}";
        return ss.str();
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

    struct NodeHandle::NodeHandleInternal {
        std::atomic<bool> flag;
    };

    NodeHandle::NodeHandle(bool a) : handle(new NodeHandleInternal{a}) {
    }

    bool NodeHandle::isEnable() const {
        return handle->flag.load();
    }

    void NodeHandle::stop() {
        handle->flag.store(false);
    }

    void NodeHandle::resume() {
        handle->flag.store(true);
    }

    NodeHandle::~NodeHandle() {
        delete handle;
    }

} // namespace MiraiCP
