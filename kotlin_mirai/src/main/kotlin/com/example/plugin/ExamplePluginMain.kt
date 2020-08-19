/*
* Name:sample example of kotlin mirai
* author:Eritque arcus
* blog:https:blog.csdn.net/qq_40832960
 */
package com.example.plugin

import net.mamoe.mirai.console.plugins.PluginBase
import net.mamoe.mirai.event.events.NewFriendRequestEvent
import net.mamoe.mirai.event.subscribeAlways
import net.mamoe.mirai.message.FriendMessageEvent
import net.mamoe.mirai.message.GroupMessageEvent
import net.mamoe.mirai.message.MessageEvent
import net.mamoe.mirai.message.TempMessageEvent


object ExamplePluginMain : PluginBase() {


    override fun onEnable() {
        var cpp=CPP_lib()
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
            var r=cpp.GroupMessage(subject.id,sender.id,message.toString())
            if(r!="CONTINUE") reply(r)//����

        }
        subscribeAlways<FriendMessageEvent> {
            //������Ϣ
            var r=cpp.PrivateMessage(sender.id,message.toString())
            if(r!="CONTINUE") reply(r)
        }
//        subscribeAlways<TempMessageEvent> {
//            //��ʱ��Ϣ
//        }
        subscribeAlways<NewFriendRequestEvent> {
            //�Զ�ͬ��Ӻ���
            if(cpp.FriendRequest(fromId,fromNick,message)) accept()
            else reject()
        }

    }
}