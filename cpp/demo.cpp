// Copyright (C) 2020-2021 Eritque arcus and contributors.
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
#include <miraiCP.hpp>
using namespace std;
using namespace MiraiCP;
// 多线程示例
void func(unsigned long long i, unsigned long long botid) {
	// 执行操作
	Friend(i, botid).sendMsg("hi");
	manager->detach();
}

// 插件实例
class Main:public CPPPlugin {
public:
    // 配置插件信息
    Main(): CPPPlugin(PluginConfig("name", "1.0", "Eritque arcus", "hello world", "2021")){}
    void onEnable() override {
        /*插件启动, 请勿在此函数运行前执行操作mirai的代码*/
        /*
        logger - 日志组件
            logger->info(string)发送消息级日志
            logger->warning(string)发送警告级日志
            logger->error(string)发送错误级日志
        一共有3种logger
         1. 是最外层的logger指针，为MiraiCP插件级logger, 标识符为MiraiCP: [content], 不建议用这个logger输出，该logger通常在MiraiCP内部使用
         2. 是CPPPlugin下的pluginLogger，为插件级logger，即当前MiraiCP加载的插件，标识符为[name]: [content], 建议用于调试信息
         3. 是每个事件的botLogger, 如:e.botLogger, 为每个机器人账号独有的logger，建议日常使用，标识符为[botid]: [content]
        procession 广播源
            procession->registerEvent<EventType>(lambda) 注册监听
            procession->registerEvent<GroupMessageEvent>([](GroupMessageEvent param){ \*处理*\});是监听群消息
            procession->registerEvent<PrivateMessageEvent>([](PrivateMessageEvent param){ \*处理*\});是监听私聊消息
            ...
        参数都在param变量里，在lambda块中使用param.xxx来调用
        */
        procession->registerEvent<BotOnlineEvent>([](BotOnlineEvent e) {
            e.botlogger.info("Bot is Online");
        });
        // 邀请事件
        // 好友申请
        procession->registerEvent<NewFriendRequestEvent>([](NewFriendRequestEvent e) {
            e.accept();
            Friend(e.fromid, e.bot.id).sendMsg("HI");
        });
        // 邀请加群
        procession->registerEvent<GroupInviteEvent>([](GroupInviteEvent e) {
            e.accept();
            logger->info("x");
            Group(e.groupid, e.bot.id).sendMsg("被" + e.inviterNick + "邀请进入" + e.groupName);
        });
        // 消息事件
        // 监听私聊
        procession->registerEvent<PrivateMessageEvent>([](PrivateMessageEvent e) {
            unsigned long long id = e.bot.id;
            e.botlogger.info(std::to_string(id));
            e.message.source.quoteAndSendMsg("HI");
            std::thread func1(func, e.sender.id(), e.bot.id);
            e.sender.sendMsg(e.message.content);
            func1.detach();
            // 多线程测试,线程应该在lambda中决定要detach还是join, 否则会报错
            // 测试取图片
            std::vector<std::string> temp = Image::GetImgIdsFromMiraiCode(e.message.content);
            for (const std::string &a : temp) {
                e.sender.sendMsg(a);
            }
            // 发送图片
            Image tmp = e.sender.uploadImg(R"(C:\Users\19308\Desktop\a.jpg)");
            e.sender.sendMsg(tmp.toMiraiCode());
            e.sender.sendMiraiCode(tmp.toMiraiCode());
            e.message.source.recall();
        });

        // 监听群信息
        procession->registerEvent<GroupMessageEvent>([=](GroupMessageEvent e) {
            e.group.sendMsg("--开始测试--");
           //  e.group.sendMsg(e.sender.at() + "发送纯文本MiraiCode");
           //  e.group.sendMiraiCode(e.sender.at() + "发送MiraiCode");
           //  e.group.sendMsg("禁言测试");
           //  try{
           //      e.sender.mute(60*2);
           //      e.sender.mute(0);
           //  }catch(BotException& b){e.group.sendMsg("权限不足");}
           //  if(!e.group.announcements.empty())
           //      e.group.sendMsg("第一个群公告内容: " + e.group.announcements[0].content);
           //  e.group.sendMsg("消息中全部at对象: ");
           //  try{
           //      e.group.sendMsg(e.message.content.filter(MiraiCP::MiraiCode::at));
           //  }catch(IllegalArgumentException& i){}
           //  e.group.sendMsg("群成员列表");
           //  e.group.sendMsg(e.group.MemberListToString());
           //  e.group.sendMsg("发送一个群公告并删除");
           //  Group::OfflineAnnouncement("Helloooooooo!", Group::AnnouncementParams()).publishTo(e.group).deleteThis();
           //  logger->info("Global全局日志");
           //  Main::pluginLogger->info("Plugin插件日志");
           //  e.botlogger.info("bot机器人日志");
           //  e.botlogger.info("上一条信息:"+e.message.content.toString());
           //  e.botlogger.info("上一条信息:"+e.message.content.toMiraiCode());
           //  e.group.sendMiraiCode(MiraiCode("[mirai:service:1,<?xml version=\"1.0\" encoding=\"utf-8\"?>\\n<msg templateID=\"12345\" action=\"web\" brief=\"简介 没点进来看见的样子\" serviceID=\"1\" url=\"https://github.com/\"><item layout=\"2\"><picture cover=\"https://avatars.githubusercontent.com/u/35139537?s=400&u=c7e3794d25a2e0f27f15caf5ba7a57c7346962f0&v=4\"/><title>标题</title><summary>描述文字</summary></item><source/></msg>\\n]"));
           //  e.group.sendMiraiCode(MiraiCode(new ServiceMessage(URLSharer())));
           //  e.group.sendMsg("上下文测试，在每接收一个'a'就加1");
           // if(e.message.content.toString() == "a"){
           //     if(!e.getContext().content.contains("count"))
           //         e.getContext().content["count"] = 1;
           //     else
           //         e.getContext().content["count"] = e.getContext().content["count"].get<int>() + 1;
           // }
           // if(e.getContext().content.contains("count"))
           //     e.group.sendMsg(e.getContext().content["count"].get<int>());
           // e.group.sendMsg("发送语音测试:");
           // e.group.sendVoice(R"(D:\下载缓存\test.amr)");
           // e.group.sendMsg("UTF8 emoji测试: ☺");
           // e.group.sendMsg("群名称，并改名为'x':"+e.group.setting.name);
           // e.group.setting.name = "x";
           // try{
           //     e.group.updateSetting();
           // }catch(BotException& b){e.group.sendMsg("没有权限");}
           // e.message.source.quoteAndSendMsg("引用测试");
           // e.botlogger.info("messageSource: "+e.message.source.serializeToString());
           // e.group.sendMsg("撤回测试:");
           // e.group.sendMsg("撤回测试").recall();
           e.group.sendMsg("发送卡片:");
           e.group.sendMiraiCode(new LightApp(LightAppStyle1()));
           e.group.sendMiraiCode(LightApp(LightAppStyle2()).toMiraiCode());
           e.group.sendMiraiCode(new LightApp(LightApp(LightAppStyle3())));
           e.group.sendMiraiCode(new LightApp(LightAppStyle4()));
           schedule(10000, "aaa");
           // e.group.sendMsg("转发测试:");
           // ForwardMessage(&e.group,
           //                {
           //     ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
           //     ForwardNode(1930893235, "Eritque arcus", "hahaha", -100)
           //                }).sendTo(&e.group);
           // e.group.sendMsg("bot属性");
           // e.sender.sendMsg(e.bot.nick());
           // e.sender.sendMsg(e.bot.FriendListToString());
           // e.sender.sendMsg(e.bot.GroupListToString());
        });
        // 监听群临时会话
        procession->registerEvent<GroupTempMessageEvent>([](GroupTempMessageEvent e) {
            e.sender.sendMsg("hi");
        });
        // 群事件
        procession->registerEvent<MemberJoinEvent>([](MemberJoinEvent e) {
            e.group.sendMiraiCode(At(e.group.getOwner()) + std::to_string(e.member.id()) + "加入了群聊");
        });
        procession->registerEvent<MemberLeaveEvent>([](MemberLeaveEvent e) {
            e.group.sendMiraiCode(At(e.group.getOwner()) + std::to_string(e.memberid) + "退出了群聊");
        });
        procession->registerEvent<TimeOutEvent>([](TimeOutEvent e){
            logger->info(e.msg);
        });
    }

    void onDisable() override {
        /*插件结束*/
    }
};

// 绑定当前插件实例
void MiraiCP::enrollPlugin(){
    MiraiCP::enrollPlugin0(new Main());
}