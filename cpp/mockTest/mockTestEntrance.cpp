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

#include "CPPPlugin.h"
#include "Command.h"
#include "ForwardedMessage.h"
#include "Schedule.h"
#include "Tools.h"
#include "mockTests.h"
#include "utils.h"


#define MESSAGE_EQ(msg) a.message[0]->content == #msg
#define TEST(msg, endMsg, test)                              \
    if (MESSAGE_EQ(msg)) {                                   \
        test if (strcmp(#endMsg, "") != 0) testEnd(#endMsg); \
        return;                                              \
    }
#define TEST_STARTWITH(msg, endMsg, test)                    \
    if (Tools::starts_with(a.message[0]->content, #msg)) {   \
        test if (strcmp(#endMsg, "") != 0) testEnd(#endMsg); \
        return;                                              \
    }

using namespace MiraiCP;
using namespace MockTests;

const PluginConfig CPPPlugin::config{
        "idMockTest",   // 插件id
        "test",         // 插件名称
        "v1.0",         // 插件版本
        "a",            // 插件作者
        "测试中文描述", // 可选：插件描述
                        // 可选：日期
};

class CustomCommand : public IRawCommand {
public:
    Config config() override {
        return {"commandPrefix", {"commandPrefix1"}};
    }

    void onCommand(std::shared_ptr<Contact> ptr, const Bot &bot, const MessageChain &chain) override {
        Message::messageSerialization(chain);
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
            TEST(event, GroupMessageEventTest, )
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
            TEST(recall, messageRecallTest, {
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
                CommandManager::registerCommand<CustomCommand>();
            })
            TEST(image, uploadImageTest, {
                Logger::logger.info(absolute(std::filesystem::path("./src/jvmTest/resources/img.png")).string());
                auto img = a.group.uploadImg(absolute(std::filesystem::path("./src/jvmTest/resources/img.png")).string());
                a.group.sendMessage(img);
            })
            TEST(botList, queryBotListTest, {
                Logger::logger.info("botList:", a.bot.FriendListToString());
                Logger::logger.info("botList:",a.bot.GroupListToString());
                Logger::logger.info("botList:",a.group.MemberListToString());
                Logger::logger.info("botList:",a.bot.OnlineBotsListToString());
            })
            TEST(sendFile, sendFileTest, {
                a.group.sendFile("/img/img.png", absolute(std::filesystem::path("./src/jvmTest/resources/img.png")).string());
            })
            TEST_STARTWITH(remoteFileInfo, remoteFileInfoTest, {
                Logger::logger.info("reId" + a.group.getFile("/mic.amr").id);
                Logger::logger.info("reId" + a.group.getFileById(a.message[0]->content.substr(14)).id);
            })
            TEST(getOwner, getOwnerTest, {
                Logger::logger.info("reId" + std::to_string(a.group.getOwner().id()));
            })
            TEST(voice, uploadVoiceTest, {
                a.group.sendVoice(absolute(std::filesystem::path("./src/jvmTest/resources/mic.amr")).string());
            })
            TEST(groupSetting, getGroupSettingTest, {
                auto settings = a.group.setting();
                settings.isAllowMemberInvite = true;
                settings.isMuteAll = true;
                settings.name = "test";
                a.group.updateSetting(settings);
            })
            TEST(buildForward, buildForwardMessageTest, {
                auto style1 = ForwardedMessageDisplayStrategy();
                style1.summary = "Summary";
                style1.preview.emplace_back("preview1");
                style1.source = "SourceA";
                style1.brief = "BriefA";
                ForwardedMessage(
                        {ForwardedNode(12, "a", MessageChain(PlainText("test")), 1),
                         ForwardedNode(12, "b", ForwardedMessage({ForwardedNode(12, "a", MessageChain(PlainText("test")), 1)}), 1)},
                        ForwardedMessageDisplayStrategy::defaultStrategy())
                        .sendTo(&a.group);
            })
            TEST(sendWithQuote, sendWithQuoteTest, {
                a.group.quoteAndSendMessage(a.message, a.message.source.value());
            })
            TEST(announcement, announcementTest, {
                a.group.getAnnouncementsList()[0].deleteThis();
                auto announcement = Group::OfflineAnnouncement();
                announcement.content = "test";
                announcement.params.pinned = true;
                announcement.publishTo(a.group);
            })
            TEST(nextMsg, , {
                auto msg = a.nextMessage();
                if (msg[0]->content == "nextMsg1" && msg.size() == 1) {
                    testEnd("nextMsgTest");
                }
            })
            TEST(imgUploaded, , {
                if (a.message[1].getVal<Image>().isUploaded(a.bot.id())) {
                    testEnd("imgUploadedTest");
                }
            })
            TEST(changeNameCard, changeNameCardTest, {
                a.sender.changeNameCard("test");
            })
            TEST(sendTemp, sendTempTest, {
                a.sender.sendMessage("x");
            })
            TEST(specialTitle, changeSpecialTitleTest, {
                a.sender.changeSpecialTitle("test");
            })
            TEST(deserializeMiraiCode, deserializeMiraiCodeTest, {
                auto msg = MessageChain::deserializationFromMiraiCode("[mirai:at:111]");
                Message::messageSerialization(msg);
            })
            TEST(honorMember, honorMemberTest, {
                auto m = a.group.queryCurrentHonorMember(MiraiCP::Group::TALKATIVE);
                Logger::logger.info("honorMember:", m.has_value()? m.value().id() : 0);
            })
            Message::messageSerialization(a.message);
            testEnd("groupMessageEventMessageTest");
        });
        Event::registerEvent<NudgeEvent>([](const NudgeEvent &a) {
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
            TEST(MsgPreSend, MessagePreSendEvent, )
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
