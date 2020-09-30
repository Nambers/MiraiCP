package com.example.plugin

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch


class CPP_lib {
    var ver:String=""
    init {
        ver=Verify()//如果ver不等于2333 载入错误
    }

    companion object{
        init {
            //这里填自己的路径
            System.load("D:\\Mirai\\libraries\\mirai-demo.dll")
        }
        @JvmStatic
        fun SendLog(log:String){
            BasicSendLog(log)
        }
        @JvmStatic
        fun SendPrivate(message: String, id: Long) {
            GlobalScope.launch(Dispatchers.Default) {
                // 反向调用发送消息 测试中
                Send(message, id)
            }
        }
    }

    

    external fun Verify(): String
    external fun PrivateMessage(id:Long,nick:String,avatarUrl:String, message:String): String
    external fun GroupMessage(groupId:Long,GroupName:String,ownerId:Long,MemberName:String,MemberGroupName:String,Memberid: Long,message:String): String
    external fun FriendRequest(qqid:Long,Nick:String,message: String): Boolean
    external fun GroupNameChange(origin:String,new:String,group:Long,operate:Long):String
    external fun GroupMemberLeave(Memberid:Long,Q:Boolean):String
    external fun GroupMemberJoin(Memberid:Long,Driving:Boolean):String
}

