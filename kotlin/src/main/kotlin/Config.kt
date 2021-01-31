package org.example.mirai.plugin

class Config {
    //消息事件
    data class GroupMessage(
        val groupid: Long,
        val senderid: Long,
        val message: String,
        val type: Int = 1
    )
    data class PrivateMessage(
        val senderid: Long,
        val message: String,
        val type: Int = 2
    )
    //群邀请
    data class GroupInvite(
        val groupid: Long,
        val invitorid: Long,
        val type: Int = 3
    )
    //好友邀请
    data class NewFriendRequest(
        val friendid: Long,
        val message: String,
        val type: Int = 4
    )
    //群成员加入
    data class MemberJoin(
        val groupid: Long,
        val memberid: Long,
        /*
        invite - 1
        active - 2
        retrieve - 3
         */
        val jointype: Int,
        //如果没有则为0
        val invitorid: Long = 0,
        val type: Int = 5
    )
}