package tech.eritquearcus.miraicp.shared
import com.google.gson.annotations.SerializedName

class Config {
    data class Contact(
        val type: Int,
        val id: Long,
        val groupid: Long,
        val nickornamecard: String,
        val botid: Long
    )
    data class SendRequest(
        val contact: Contact,
        val content:String
    )
    //消息事件
    data class GroupMessage(
        val group: Contact,
        val member: Contact,
        val message: String,
        val source: String,
        val type: Int = 1
    )

    data class PrivateMessage(
        val friend: Contact,
        val message: String,
        val source: String,
        val type: Int = 2
    )

    //群邀请
    data class GroupInviteSource(
        val botid: Long,
        val eventid: Long,
        val inviterid: Long,
        val groupid: Long,
        val groupname: String,
        val inviternick: String
    )

    data class GroupInvite(
        val source: GroupInviteSource,
        val type: Int = 3
    )
    data class NewFriendRequestSource(
        val botid: Long,
        val eventid: Long,
        val message: String,
        val fromid: Long,
        val fromgroupid: Long,
        val fromnick: String
    )
    //好友邀请
    data class NewFriendRequest(
        val source: NewFriendRequestSource,
        val type: Int = 4
    )
    //群成员加入
    data class MemberJoin(
        val group: Contact,
        val member: Contact,
        /*
        invite - 1
        active - 2
        retrieve - 3
         */
        val jointype: Int,
        //如果没有则为member.id
        val inviterid: Long = 0,
        val type: Int = 5
    )
    //群成员退出
    data class MemberLeave(
        val group: Contact,
        val memberid: Long,
        /*
        kick - 1
        quit - 2
         */
        val leavetype: Int,
        //如果没有则为memberid
        val operatorid: Long = 0,
        val type: Int = 6
    )
    //撤回
    data class RecallEvent(
        val etype: Int,
        val authorid: Long,
        val operatorid: Long,
        val ids: String,
        val internalids: String,
        val time: Int,
        val groupid: Long = 0,
        val botid: Long,
        val type: Int = 7
    )
    data class BotJoinGroup(
        val etype: Int,
        val group: Contact,
        val inviterid: Long,
        val type: Int = 9
    )
    data class GroupTempMessage(
        val group: Contact,
        val member: Contact,
        val message: String,
        val source: String,
        val type: Int = 10
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
    data class GroupSetting(
        val name:String,
        val entranceAnnouncement:String,
        val isMuteAll:Boolean,
        val isAllowMemberInvite:Boolean,
        val isAutoApproveEnabled:Boolean,
        val isAnonymousChatEnabled:Boolean
    )
    data class ContactInfo(
        val nickornamecard: String,
        val avatarUrl: String,
        val setting: GroupSetting = GroupSetting("", "", false, false, false, false)
    )
    data class DInfo(
        val url: String,
        val md5: String,
        val sha1: String
    )
    data class FInfo(
        val size: Long,
        val uploaderid: Long,
        val downloadtime: Int,
        val uploadtime: Long,
        val lastmodifytime: Long,
    )
    data class FileInfo(
        val id: String,
        val internalid: Int = 0,
        val name: String = "",
        val path: String = "/",
        val dinfo: DInfo,
        val finfo: FInfo
    )
    data class GroupAnnouncementMsg(
        val text: String,
        val text_face: String? = null,
        val title: String? = null
    )
}