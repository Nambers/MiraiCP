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
        //����Ѽ���
        logger.info("Plugin loaded!")
        logger.info(cpp.ver)//���2333 ����


//        subscribeAlways<MessageEvent> {
//            //ȫ����Ϣ
//
//        }

        subscribeAlways<GroupMessageEvent> {
            //Ⱥ��Ϣ

            val r=cpp.GroupMessage(subject.id,subject.name,subject.owner.id, sender.nick,sender.nameCard,sender.id, message.toString())
            if(r!="CONTINUE") reply(r)//����

        }
        subscribeAlways<FriendMessageEvent> {
            //������Ϣ

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
            //�Զ�ͬ��Ӻ���
            if(cpp.FriendRequest(fromId, fromNick, message)) accept()
            else reject()
        }

    }
}