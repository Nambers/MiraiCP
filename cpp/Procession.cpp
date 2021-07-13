#include "miraiCP.hpp"
using namespace std;
using namespace MiraiCP;
// 多线程示例
void func(unsigned long long i, unsigned long long botid) {
	// 执行操作
	Friend(i, botid).sendMsg("hi");
	manager->detach();
}

class Main:public CPPPlugin {
public:
    Main(): CPPPlugin(PluginConfig("name", "1.0", "Eritque arcus", "hello world")){}
    void onEnable() override {
        /*插件启动, 请勿在此函数运行前执行操作mirai的代码*/
        /*
        注册事件监听-用户自定义
        logger - 日志组件
            logger->Info(string)发送消息级日志
            logger->Warning(string)发送警告级日志
            logger->Error(string)发送错误级日志
        一共有3种logger
         1. 是最外层的logger指针，为MiraiCP插件级logger, 标识符为MiraiCP: [content], 不建议用这个logger输出，该logger通常在MiraiCP内部使用
         2. 是CPPPlugin下的pluginLogger，为插件级logger，即当前MiraiCP加载的插件，标识符为MiraiCP-[name]: [content], 建议用于调试信息
         3. 是每个事件的botLogger, 如:e.botLogger, 为每个机器人账号独有的logger，建议日常使用，标识符为[botid]: [content]
        procession 广播源
            procession->registerEvent(lambda) 注册监听
            procession->registerEvent([](GroupMessageEvent param){ \*处理*\});是监听群消息
            procession->registerEvent([](PrivateMessageEvent param){ \*处理*\});是监听私聊消息
            ...
        参数都在param变量里，在lambda块中使用param.xxx来调用
        */
        procession->registerEvent<BotOnlineEvent>([](BotOnlineEvent e) {
            e.botlogger.Info("Bot is Online");
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
            logger->Info("x");
            Group(e.groupid, e.bot.id).sendMsg("被" + e.inviterNick + "邀请进入" + e.groupName);
        });
        // 消息事件
        // 监听私聊
        Event::NodeHandle handle = procession->registerEvent<PrivateMessageEvent>([](PrivateMessageEvent e) {
            unsigned long long id = e.bot.id;
            e.botlogger.Info(std::to_string(id));
            e.messageSource.quoteAndSendMsg("HI");
            std::thread func1(func, e.sender.id(), e.bot.id);
            e.sender.sendMsg(e.message);
            func1.detach();
            // 多线程测试,线程应该在lambda中决定要detach还是join, 否则会报错
            // 测试取图片
            std::vector<std::string> temp = Image::GetImgIdsFromMiraiCode(e.message);
            for (const std::string &a : temp) {
                e.sender.sendMsg(a);
            }
            // 发送图片
            Image tmp = e.sender.uploadImg(R"(C:\Users\19308\Desktop\a.jpg)");
            e.sender.sendMsg(tmp.toMiraiCode());
            e.sender.sendMiraiCode(tmp.toMiraiCode());
            e.messageSource.recall();
        });

        // 监听群信息
        procession->registerEvent<GroupMessageEvent>([=](GroupMessageEvent e) {
            logger->Info(e.group.setting.entranceAnnouncement);
            logger->Info("Global");
            this->pluginLogger->Info("Plugin");
            e.botlogger.Info("bot");
//	    e.group.sendVoice(R"(D:\下载缓存\test.amr)");
//        e.botlogger.Info(e.message);
//        e.group.sendMiraiCode(e.message);
//        e.group.sendMsg("☺");
//        e.group.sendMsg(e.group.setting.name);
//        e.group.setting.name = "x";
//        e.group.updateSetting();
//        e.messageSource.quoteAndSendMsg("HI");
//        e.botlogger.Info(e.messageSource.serializeToString());
//        // 发送文本信息
//        e.group.sendMsg("HI");
//        // 发送MiraiCode信息，At
//        e.group.sendMiraiCode(At(e.sender));
//        // 撤回测试
//        e.group.sendMsg("撤回测试").recall();
//        // 发送xml卡片测试,可以用new传miraicodeable指针进去，也可以用.toMiraiCode()
//        e.group.sendMiraiCode(new LightApp(LightAppStyle1()));
//        e.group.sendMiraiCode(LightApp(LightAppStyle2()).toMiraiCode());
//        e.group.sendMiraiCode(new LightApp(LightAppStyle3()));
            ForwardMessage(&e.group,
                           {
                                   ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
                                   ForwardNode(1930893235, "Eritque arcus", "hahaha", -100)
                           }).sendTo(&e.group);
//        // 关闭上面的私聊消息监听器
//        handle.stop();
//        // 当前bot属性
//        e.sender.sendMsg(e.bot.nick());
//        e.sender.sendMsg(e.bot.FriendListToString());
//        e.sender.sendMsg(e.bot.GroupListToString());
//        RemoteFile tmp = e.group.sendFile("/test.txt", "D:\\ValveUnhandledExceptionFilter.txt");
//        e.group.sendMsg(e.group.getFile("/", tmp.id).name);
//        e.group.sendMsg(e.group.getFile("/test.txt").name);
//        e.group.sendMsg(e.group.getFileListString("/"));
//        std::vector<Group::file_short_info> a = e.group.getFileList("/");
//        std::stringstream ss;
//        for (size_t i = 0; i < a.size(); ++i) {
//            if (i != 0)
//                ss << ",";
//            ss << "{" << a[i].path << "," << a[i].id << "}";
//        }
//        std::string s = ss.str();
//        e.group.sendMsg(s);
//        e.group.sendMsg(e.group.MemberListToString());
//        e.messageSource.recall();
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
    }

    void onDisable() override {
        /*插件结束*/
    }
};

void MiraiCP::enrollPlugin(){
    MiraiCP::enrollPlugin0(new Main());
}