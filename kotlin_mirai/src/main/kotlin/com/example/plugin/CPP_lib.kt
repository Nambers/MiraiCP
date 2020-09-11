package com.example.plugin

class CPP_lib {
    var ver:String=""
    init {
        ver=Verify()//如果ver不等于2333 载入错误
    }
    companion object {
        init {
            //这里填自己的路径
            System.load("D:\\Mirai\\libraries\\mirai-demo.dll")
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