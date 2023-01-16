// Copyright (c) 2022 - 2023. Eritque arcus and contributors.
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

// MiraiCP依赖文件(只需要引入这一个)
#include "MiraiCP.hpp"
#include "MockTests.h"
using namespace MiraiCP;
using namespace MockTests;

const PluginConfig CPPPlugin::config{
        "idMockTest",          // 插件id
        "test",         // 插件名称
        "v1.0",         // 插件版本
        "a",            // 插件作者
        "测试中文描述", // 可选：插件描述
        // 可选：日期
};

class CustomCommand: public IRawCommand {
public:
    Config config() override {
        return {"commandPrefix", {"commandPrefix1"}};
    }

    void onCommand(std::shared_ptr<Contact> ptr, const Bot &bot, const MessageChain &chain) override {
        testEnd("CustomCommand");
    }
};

class Main : public CPPPlugin {
public:
    Main() : CPPPlugin() {}
    ~Main() override = default;

public:
    void onEnable() override {
        Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent a) {
            if(a.message[0]->content == "event"){
                testEnd("GroupMessageEventTest");
                return;
            }
            if(a.message[0]->content == "message"){
                a.group.sendMessage(a.message);
                testEnd("groupSendMessageTest");
                return;
            }
            if(a.message[0]->content == "nudge"){
                a.sender.sendNudge();
                testEnd("memberNudgeTest");
                return;
            }
            if(a.message[0]->content == "mute"){
                a.sender.mute(999);
                testEnd("memberMuteTest");
                return;
            }
            if(a.message[0]->content == "upgrade"){
                a.sender.modifyAdmin(true);
                testEnd("memberUpgradeTest");
                return;
            }
            if(a.message[0]->content == "kick"){
                a.sender.kick("X");
                testEnd("memberKickTest");
                return;
            }
            if(a.message[0]->content == "recall"){
                a.message.source->recall();
                testEnd("messageRecallTest");
                return;
            }
            if(a.message[0]->content == "refresh"){
                refresh(a);
                testEnd("refreshInfoTest");
                return;
            }
            if(a.message[0]->content == "timer"){
                schedule(1, "This is a message");
                return;
            }
            if(a.message[0]->content == "command"){
                CommandManager::commandManager.registerCommand(CustomCommand());
                testEnd("registerCommandTest");
                return;
            }
            if(a.message[0]->content == "image"){
                Logger::logger.info(absolute(std::filesystem::path("./src/jvmTest/resources/img.png")).string());
                auto img = a.group.uploadImg(absolute(std::filesystem::path("./src/jvmTest/resources/img.png")).string());
                a.group.sendMessage(img);
                testEnd("uploadImageTest");
                return;
            }
            Message::messageSerialization(a.message);
            testEnd("groupMessageEventMessageTest");
        });
        Event::registerEvent<NudgeEvent>([](const NudgeEvent& a) {
            Logger::logger.info("nudge:" + std::to_string(a.target->id()));
            testEnd("NudgeEventTest");
        });
        Event::registerEvent<GroupTempMessageEvent>([](GroupTempMessageEvent a) {
            testEnd("GroupMessageEventTest");
        });
        Event::registerEvent<PrivateMessageEvent>([](PrivateMessageEvent a) {
            if(a.message[0]->content == "message"){
                a.sender.sendMessage(a.message);
                testEnd("privateSendMessageTest");
                return;
            }
            if(a.message[0]->content == "nudge"){
                a.sender.sendNudge();
                testEnd("memberNudgeTest");
                return;
            }
            if(a.message[0]->content == "delete"){
                a.sender.deleteFriend();
                testEnd("deleteFriendTest");
                return;
            }
            testEnd("PrivateMessageEventTest");
        });
        Event::registerEvent<NewFriendRequestEvent>([](NewFriendRequestEvent a) {
            a.accept();
            testEnd("GroupMessageEventTest");
        });
        Event::registerEvent<MemberJoinEvent>([](MemberJoinEvent a) {
            testEnd("MemberJoinEventTest");
        });
        Event::registerEvent<MemberJoinRequestEvent>([](MemberJoinRequestEvent a) {
            a.accept();
            testEnd("MemberJoinReqTest");
        });
        Event::registerEvent<MemberLeaveEvent>([](MemberLeaveEvent a) {
            Logger::logger.info("type" + std::to_string(a.type));
            testEnd("MemberLeaveEventKickTest");
        });
        Event::registerEvent<BotOnlineEvent>([](BotOnlineEvent a) {
            testEnd("BotOnlineEventTest");
        });
        Event::registerEvent<RecallEvent::MemberRecallEvent>([](RecallEvent::MemberRecallEvent a) {
            testEnd("MemberRecallEventTest");
        });
        Event::registerEvent<RecallEvent::FriendRecallEvent>([](RecallEvent::FriendRecallEvent a) {
            testEnd("FriendRecallEventTest");
        });
        Event::registerEvent<BotJoinGroupEvent>([](BotJoinGroupEvent a) {
            Logger::logger.info(a.type);
            testEnd("BotJoinGroupEventTest");
        });
        Event::registerEvent<GroupInviteEvent>([](GroupInviteEvent a) {
            a.accept();
            testEnd("GroupInviteEvent");
        });
        Event::registerEvent<BotLeaveEvent>([](BotLeaveEvent a) {
            testEnd("BotLeaveEvent");
        });
        Event::registerEvent<MessagePreSendEvent>([](MessagePreSendEvent a) {
            if(a.message.first<PlainText>()->content == "MsgPreSend") {
                testEnd("MessagePreSendEvent");
            }
        });
        Event::registerEvent<TimeOutEvent>([](TimeOutEvent a) {
            testEnd("TimeOutException");
        });
    }

    void onDisable() override {}
};

void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin<Main>();
}
