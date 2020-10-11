/*
* @Name:sample example of kotlin mirai
* @author:Eritque arcus
* @blog:https:blog.csdn.net/qq_40832960
 */
package com.example.plugin

import net.mamoe.mirai.Bot
import net.mamoe.mirai.console.plugins.PluginBase
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.event.subscribeAlways
import net.mamoe.mirai.message.*

object ExamplePluginMain : PluginBase() {
    lateinit var AIbot:Bot
    suspend fun Send(message:String, id:Long){
        //反向调用
        logger.info("Send message for $id is $message")
        logger.info(AIbot.selfQQ.toString())
        val a=AIbot.getFriend(id)
        logger.info("successful get friend")
        a.sendMessage(message)
        logger.info("Send success")
    }
    fun BasicSendLog(log:String){
        logger.info(log);
    }
    suspend fun SendG(message:String,id:Long){
        AIbot.getGroup(id).sendMessage(message)
    }
    fun GetN(qqid:Long):String {
        return AIbot.getFriend(qqid).nick;
    }
        override fun onEnable() {
            val cpp=CPP_lib()
            super.onEnable()
            //插件已加载
            logger.info("Plugin loaded!")
            logger.info(cpp.ver)//输出2333 正常
            subscribeAlways<BotOnlineEvent> {
                AIbot=bot
            }
            subscribeAlways<GroupMessageEvent> {
                //群信息
                cpp.GroupMessage(subject.id,subject.name,subject.owner.id, sender.nick,sender.nameCard,sender.id, message.toString())

            }
            subscribeAlways<FriendMessageEvent> {
                //个人信息
                cpp.PrivateMessage(subject.id,subject.nick,subject.avatarUrl, message.toString())
            }
            subscribeAlways<MemberJoinEvent.Active> {
                cpp.GroupMemberJoin(member.id, true,group.id)
            }
            subscribeAlways<MemberJoinEvent.Invite> {
                cpp.GroupMemberJoin(member.id, false,group.id)
            }
            subscribeAlways<MemberLeaveEvent.Kick> {
                cpp.GroupMemberLeave(member.id, true,group.id)
            }
            subscribeAlways<MemberLeaveEvent.Quit> {
                cpp.GroupMemberLeave(member.id, false,group.id)
            }
            subscribeAlways<GroupNameChangeEvent> {
                val a:Long
                if(operator==null) a=0
                else a= operator!!.id
                cpp.GroupNameChange(origin, new, group.id, a)
            }

            subscribeAlways<NewFriendRequestEvent> {
                //自动同意加好友
                if(cpp.FriendRequest(fromId, fromNick, message)) accept()
                else reject()
            }

        }

    }

