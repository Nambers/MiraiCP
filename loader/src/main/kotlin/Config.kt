package tech.eritquearcus.miraicp
import com.google.gson.annotations.SerializedName

class Config {
    //消息事件
    data class GroupMessage(
        val groupid: Long,
        val senderid: Long,
        val message: String,
        val Source: String,
        val type: Int = 1
    )
    data class PrivateMessage(
        val senderid: Long,
        val message: String,
        val Source: String,
        val type: Int = 2
    )
    //群邀请
    data class GroupInvite(
        val groupid: Long,
        val groupname: String,
        val invitorid: Long,
        val invitorname: String,
        val source: String,
        val type: Int = 3
    )
    //好友邀请
    data class NewFriendRequest(
        val friendid: Long,
        val message: String,
        val eventhandle: String,
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
    //群成员退出
    data class MemberLeave(
        val groupid: Long,
        val memberid: Long,
        /*
        kick - 1
        quit - 2
         */
        val leavetype: Int,
        //如果没有则为0
        val operatorid: Long = 0,
        val type: Int = 6
    )
    //撤回
    data class RecallEvent(
        val Etype: Int,
        val authorid: Long,
        val operatorid: Long,
        val ids: String,
        val internalids: String,
        val time: Int,
        val groupid: Long = 0,
        val type: Int = 7
    )
    data class TimeOutEvent(
        val message: String,
        val type: Int = 8
    )
    data class BotJoinGroup(
        val etype: Int,
        val groupid: Long,
        val invitorid: Long,
        val type: Int = 9
    )
    data class ForwardMessageJson (

        @SerializedName("type") val type : Int,
        @SerializedName("id") val id : Long,
        @SerializedName("id2") val id2 : Long,
        @SerializedName("content") val content: Content
    )
    data class Content(
        @SerializedName("type") val type : Int,
        @SerializedName("id") val id : Long,
        @SerializedName("id2") val id2 : Long,
        @SerializedName("value") val value : List<Value>
    )
    data class Value (
        @SerializedName("name") val name : String,
        @SerializedName("id") val id : Long,
        @SerializedName("time") val time : Int,
        @SerializedName("message") val message : String
    )
}