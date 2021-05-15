// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
// 当使用预编译的头时，需要使用此源文件，编译才能成功。


//class Config {
//    data class Contact(
//            val type: Int,
//            val id: Long,
//            val groupid: Long,
//            val nickornamecard: String,
//            val botid: Long
//    )
//    data class SendRequest(
//            val contact: Contact,
//            val content:String
//    )
//    //消息事件
//    data class GroupMessage(
//            val groupid: Long,
//            val senderid: Long,
//            val message: String,
//            val Source: String,
//            val botid: Long,
//            val type: Int = 1
//    )
//    data class PrivateMessage(
//            val senderid: Long,
//            val message: String,
//            val Source: String,
//            val botid: Long,
//            val type: Int = 2
//    )
//    //群邀请
//    data class GroupInvite(
//            val groupid: Long,
//            val groupname: String,
//            val inviterid: Long,
//            val invitername: String,
//            val source: String,
//            val botid: Long,
//            val type: Int = 3
//    )
//    data class NewFriendRequestSource(
//            val botid: Long,
//            val eventid: Long,
//            val message: String,
//            val fromid: Long,
//            val fromgroupid: Long,
//            val fromnick: String
//    )
//    //好友邀请
//    data class NewFriendRequest(
//            val source: NewFriendRequestSource,
//            val type: Int = 4
//    )
//    //群成员加入
//    data class MemberJoin(
//            val groupid: Long,
//            val memberid: Long,
//    /*
//    invite - 1
//    active - 2
//    retrieve - 3
//     */
//    val jointype: Int,
//    //如果没有则为0
//    val inviterid: Long = 0,
//            val botid: Long,
//            val type: Int = 5
//    )
//    //群成员退出
//    data class MemberLeave(
//            val groupid: Long,
//            val memberid: Long,
//    /*
//    kick - 1
//    quit - 2
//     */
//    val leavetype: Int,
//    //如果没有则为0
//    val operatorid: Long = 0,
//            val botid: Long,
//            val type: Int = 6
//    )
//    //撤回
//    data class RecallEvent(
//            val Etype: Int,
//            val authorid: Long,
//            val operatorid: Long,
//            val ids: String,
//            val internalids: String,
//            val time: Int,
//            val groupid: Long = 0,
//            val botid: Long,
//            val type: Int = 7
//    )
//    data class TimeOutEvent(
//            val message: String,
//            val type: Int = 8
//    )
//    data class BotJoinGroup(
//            val etype: Int,
//            val groupid: Long,
//            val inviterid: Long,
//            val botid: Long,
//            val type: Int = 9
//    )
//    data class GroupTempMessage(
//            val groupid: Long,
//            val senderid: Long,
//            val message: String,
//            val Source: String,
//            val botid: Long,
//            val type: Int = 10
//    )
//    data class ForwardMessageJson (
//    @SerializedName("type") val type : Int,
//    @SerializedName("id") val id : Long,
//    @SerializedName("id2") val id2 : Long,
//    @SerializedName("content") val content: Content
//    )
//    data class Content(
//    @SerializedName("type") val type : Int,
//    @SerializedName("id") val id : Long,
//    @SerializedName("id2") val id2 : Long,
//    @SerializedName("value") val value : List<Value>
//    )
//    data class Value (
//    @SerializedName("name") val name : String,
//    @SerializedName("id") val id : Long,
//    @SerializedName("time") val time : Int,
//    @SerializedName("message") val message : String
//    )
//    data class ContactInfo(
//            val nickornamecard: String,
//            val avatarUrl: String
//    )
//    data class DInfo(
//            val url: String,
//            val md5: String,
//            val sha1: String
//    )
//    data class FInfo(
//            val size: Long,
//            val uploaderid: Long,
//            val downloadtime: Int,
//            val uploadtime: Long,
//            val lastmodifytime: Long,
//    )
//    data class FileInfo(
//            val id: String,
//            val internalid: Int = 0,
//            val name: String = "",
//            val path: String = "/",
//            val dinfo: DInfo,
//            val fInfo: FInfo
//    )
//}
