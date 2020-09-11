/*
* @Name:sample example of kotlin mirai
* @author:Eritque arcus
* @blog:https:blog.csdn.net/qq_40832960
 */
package com.example.plugin

import net.mamoe.mirai.console.plugins.PluginBase
import net.mamoe.mirai.contact.Member
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.event.subscribeAlways
import net.mamoe.mirai.message.*


object ExamplePluginMain : PluginBase() {
//    fun showStatic(a:String) {
//        logger.info(a)
//    }
    override fun onEnable() {
        val cpp=CPP_lib()
        super.onEnable()
        //插件已加载
        logger.info("Plugin loaded!")
        logger.info(cpp.ver)//输出2333 正常


//        subscribeAlways<MessageEvent> {
//            //全部信息
//
//        }

        subscribeAlways<GroupMessageEvent> {
            //群信息

            val r=cpp.GroupMessage(subject.id,subject.name,subject.owner.id, sender.nick,sender.nameCard,sender.id, message.toString())
            if(r!="CONTINUE") reply(r)//继续

        }
        subscribeAlways<FriendMessageEvent> {
            //个人信息

            val r=cpp.PrivateMessage(subject.id,subject.nick,subject.avatarUrl, message.toString())
            if(r!="CONTINUE") reply(r)
        }
        subscribeAlways<MemberJoinEvent.Active> {
            val r=cpp.GroupMemberJoin(member.id, true)
            if(r!="CONTINUE") group.sendMessage(r)
        }
        subscribeAlways<MemberJoinEvent.Invite> {
            val r=cpp.GroupMemberJoin(member.id, false)
            if(r!="CONTINUE") group.sendMessage(r)
        }
        subscribeAlways<MemberLeaveEvent.Kick> {
            val r=cpp.GroupMemberLeave(member.id, true)
            if(r!="CONTINUE") group.sendMessage(r)
        }
        subscribeAlways<MemberLeaveEvent.Quit> {
            val r=cpp.GroupMemberLeave(member.id, false)
            if(r!="CONTINUE") group.sendMessage(r)
        }
        subscribeAlways<GroupNameChangeEvent> {
            val a:Long
            if(operator==null) a=0
            else a= operator!!.id
            val r=cpp.GroupNameChange(origin, new, group.id, a)
            if(r!="CONTINUE") group.sendMessage(r)
        }

        subscribeAlways<NewFriendRequestEvent> {
            //自动同意加好友
            if(cpp.FriendRequest(fromId, fromNick, message)) accept()
            else reject()
        }

    }
}