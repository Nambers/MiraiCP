/*
* @Name:sample example of kotlin mirai
* @author:Eritque arcus
* @blog:https:blog.csdn.net/qq_40832960
 */
package com.example.plugin

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import net.mamoe.mirai.Bot
import net.mamoe.mirai.console.plugins.PluginBase
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.event.subscribeAlways
import net.mamoe.mirai.message.FriendMessageEvent
import net.mamoe.mirai.message.GroupMessageEvent

object ExamplePluginMain : PluginBase() {
    private lateinit var AIbot: Bot
    suspend fun Send(message: String, id: Long) {
        //反向调用
        logger.info("Send message for $id is $message")
        val a = AIbot.getFriend(id)
        a.sendMessage(message)
    }

    fun BasicSendLog(log: String) {
        logger.info(log);
    }

    suspend fun SendG(message: String, id: Long) {
        logger.info("Send message for Group($id) is $message")
        AIbot.getGroup(id).sendMessage(message)
    }

    fun GetN(qqid: Long): String {
        return AIbot.getFriend(qqid).nick
    }

    override fun onEnable() {
        val cpp = CPP_lib()
        super.onEnable()
        //插件已加载
        logger.info("Plugin loaded!")
        logger.info("github存储库:https://github.com/Nambers/MiraiCP")
        logger.info(cpp.ver)//输出2333 正常
        subscribeAlways<BotOnlineEvent> {
            AIbot = this.bot
        }
        subscribeAlways<GroupMessageEvent> {
            //群信息
            GlobalScope.launch(Dispatchers.Default) {
                cpp.GroupMessage(
                    subject.id,
                    subject.name,
                    subject.owner.id,
                    sender.nick,
                    sender.nameCard,
                    sender.id,
                    message.toString()
                )
            }
        }
        subscribeAlways<FriendMessageEvent> {
            //个人信息
            logger.info("starting")
            cpp.PrivateMessage(subject.id,message.toString())
            logger.info("ending")
        }
        subscribeAlways<MemberJoinEvent.Active> {
            cpp.GroupMemberJoin(member.id, true, group.id)
        }
        subscribeAlways<MemberJoinEvent.Invite> {
            cpp.GroupMemberJoin(member.id, false, group.id)
        }
        subscribeAlways<MemberLeaveEvent.Kick> {
            cpp.GroupMemberLeave(member.id, true, group.id)
        }
        subscribeAlways<MemberLeaveEvent.Quit> {
            cpp.GroupMemberLeave(member.id, false, group.id)
        }
        subscribeAlways<GroupNameChangeEvent> {
            val a: Long = if (operator == null) 0
            else operator!!.id
            cpp.GroupNameChange(origin, new, group.id, a)
        }

        subscribeAlways<NewFriendRequestEvent> {
            //自动同意加好友
            if (cpp.FriendRequest(fromId, fromNick, message)) accept()
            else reject()
        }

    }

}

