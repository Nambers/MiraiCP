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
#define MESSAGE_EQ(msg) a.message[0]->content == #msg
#define TEST(msg, endMsg, test) \
    if (MESSAGE_EQ(msg)) {      \
        test\
        if(strcmp(#endMsg, "") != 0) testEnd(#endMsg);\
        return; \
    }

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
            TEST(event, GroupMessageEventTest,)
            TEST(message, groupSendMessageTest, {
                a.group.sendMessage(a.message);
            })
            TEST(nudge, memberNudgeTest, {
                a.sender.sendNudge();
            })
            TEST(mute, memberMuteTest, {
                a.sender.mute(999);
            })
            TEST(upgrade, memberUpgradeTest, {
                    a.sender.modifyAdmin(true);
            })
            TEST(kick, memberKickTest, {
                a.sender.kick("X");
            })
            TEST(recall, messageRecallTest,{
                a.message.source->recall();
            })
            TEST(refresh, refreshInfoTest, {
                refresh(a);
                a.group.getAnnouncementsList();
            })
            TEST(timer, , {
                schedule(1, "This is a message");
            })
            TEST(command, registerCommandTest, {
                CommandManager::commandManager.registerCommand(CustomCommand());
            })
            TEST(image, uploadImageTest, {
                Logger::logger.info(absolute(std::filesystem::path("./src/jvmTest/resources/img.png")).string());
                auto img = a.group.uploadImg(absolute(std::filesystem::path("./src/jvmTest/resources/img.png")).string());
                a.group.sendMessage(img);
            })
            TEST(botList, queryBotListTest, {
                Logger::logger.info(a.bot.FriendListToString());
                Logger::logger.info(a.bot.GroupListToString());
                Logger::logger.info(a.group.MemberListToString());
            })
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
            TEST(message, privateSendMessageTest, {
                a.sender.sendMessage(a.message);
            })
            TEST(nudge, memberNudgeTest, {
                a.sender.sendNudge();
            })
            TEST(delete, deleteFriendTest, {
                a.sender.deleteFriend();
            })
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
            TEST(MsgPreSend, MessagePreSendEvent,)
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
