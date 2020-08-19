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
    external fun PrivateMessage(qqid:Long,message:String): String
    external fun GroupMessage(groupid:Long,qqid:Long,message:String): String
    external fun FriendRequest(qqid:Long,Nick:String,message: String): Boolean
}