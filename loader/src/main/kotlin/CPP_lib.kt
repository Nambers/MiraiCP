package org.example.mirai.plugin
import kotlinx.coroutines.runBlocking
import org.example.mirai.plugin.KotlinMain.BasicSendLog
import org.example.mirai.plugin.KotlinMain.GetN
import org.example.mirai.plugin.KotlinMain.GetNN
import org.example.mirai.plugin.KotlinMain.QueryImg
import org.example.mirai.plugin.KotlinMain.QueryML
import org.example.mirai.plugin.KotlinMain.QueryNG
import org.example.mirai.plugin.KotlinMain.Send
import org.example.mirai.plugin.KotlinMain.SendError
import org.example.mirai.plugin.KotlinMain.SendG
import org.example.mirai.plugin.KotlinMain.SendGM
import org.example.mirai.plugin.KotlinMain.SendM
import org.example.mirai.plugin.KotlinMain.SendWarning
import org.example.mirai.plugin.KotlinMain.accpetFriendRequest
import org.example.mirai.plugin.KotlinMain.accpetGroupInvite
import org.example.mirai.plugin.KotlinMain.buildforwardMsg
import org.example.mirai.plugin.KotlinMain.dll_name
import org.example.mirai.plugin.KotlinMain.getowner
import org.example.mirai.plugin.KotlinMain.kkick
import org.example.mirai.plugin.KotlinMain.kqueryM
import org.example.mirai.plugin.KotlinMain.mute
import org.example.mirai.plugin.KotlinMain.muteall
import org.example.mirai.plugin.KotlinMain.recallMsg
import org.example.mirai.plugin.KotlinMain.rejectFriendRequest
import org.example.mirai.plugin.KotlinMain.rejectGroupInvite
import org.example.mirai.plugin.KotlinMain.scheduling
import org.example.mirai.plugin.KotlinMain.uploadImgFriend
import org.example.mirai.plugin.KotlinMain.uploadImgGroup
import org.example.mirai.plugin.KotlinMain.uploadImgMember

class CPP_lib {
    var ver:String=""
    init {
        ver=Verify()
    }
    //"C:\Program Files\Java\jdk1.8.0_261\bin\javah.exe" org.example.mirai.plugin.CPP_lib
    companion object{
        init {
            //这里填自己的路径
            System.load(dll_name)
        }
        //send MiraiCode
        //to Friend
        @JvmStatic
        fun SendPrivateMSG(message: String, id: Long):String{
            return runBlocking {
                Send(message, id)
            }

        }
        //to Member
        @JvmStatic
        fun SendPrivateM2M(message: String, id: Long, gid: Long): String{
            // 反向调用发送消息
            return runBlocking {
                Send(message, id, gid)
            }
        }
        //to Group
        @JvmStatic
        fun SendGroup(message:String,id:Long):String {
            return runBlocking {
                SendG(message, id)
            }
        }
        //send msg(painText)
        //to Friend
        @JvmStatic
        fun SendPrivateMSGM(message: String, id: Long):String{
            // 反向调用发送消息
            return runBlocking {
                SendM(message, id)
            }

        }
        //to Member
        @JvmStatic
        fun SendPrivateM2MM(message: String, id: Long, gid: Long): String{
            // 反向调用发送消息
            return runBlocking {
                SendM(message, id, gid)
            }
        }
        //to Group
        @JvmStatic
        fun SendGroupM(message:String,id:Long):String {
            return runBlocking {
                SendGM(message, id)
            }
        }
        //recall messageSource
        @JvmStatic
        fun recall(source: String):String {
            return runBlocking {
                recallMsg(source)
            }
        }
        //查询图片下载链接
        @JvmStatic
        fun QueryImgUrl(id:String): String {
            return runBlocking {
                QueryImg(id)
            }
        }
        //发送普通日志
        @JvmStatic
        fun SendLog(log:String) {
            BasicSendLog(log)
        }
        //发送警告日志
        @JvmStatic
        fun SendW(log:String) {
            SendWarning(log)
        }
        //发送错误日志
        @JvmStatic
        fun SendE(log:String) {
            SendError(log)
        }
        //取昵称
        @JvmStatic
        fun GetNick(qqid:Long): String {
            return GetN(qqid)
        }
        //取群名片
        @JvmStatic
        fun GetNameCard(qqid:Long, groupid:Long): String {
            return GetNN(qqid, groupid);
        }
        //给群聊上传图片以备发送
        @JvmStatic
        fun uploadImgG(qqid: Long, fileName:String): String{
            return runBlocking {
                uploadImgGroup(qqid, fileName)
            }
        }
        //给好友上传图片以备发送
        @JvmStatic
        fun uploadImgF(qqid: Long, fileName:String): String{
            return runBlocking {
                uploadImgFriend(qqid, fileName)
            }
        }
        //群成员对象上传图片
        @JvmStatic
        fun uploadImgM(groupid: Long, qqid: Long, fileName:String): String{
            return runBlocking {
                uploadImgMember(groupid,qqid, fileName)
            }
        }
        //定时任务
        @JvmStatic
        fun schedule(time:Long, id:String){
            scheduling(time, id)
        }
        //mute member
        @JvmStatic
        fun muteM(qqid: Long, groupid: Long, time: Int): String{
            return runBlocking {
                mute(qqid, groupid, time)
            }
        }
        //query the permission of a member in a group
        @JvmStatic
        fun queryM(qqid: Long, groupid: Long): String{
            return kqueryM(qqid, groupid)
        }
        //kick a member
        @JvmStatic
        fun kickM(qqid: Long, groupid: Long, message: String):String{
            return runBlocking {
                kkick(qqid, groupid, message)
            }
        }
        //Mute the whole group
        @JvmStatic
        fun muteGroup(groupid: Long, sign: Boolean):String{
            return muteall(groupid, sign)
        }
        //query the group name
        @JvmStatic
        fun queryNG(groupid: Long): String {
            return QueryNG(groupid)
        }
        //query the member list of a group
        @JvmStatic
        fun queryML(groupid: Long):String{
            return QueryML(groupid)
        }
        //query the owner of a group
        @JvmStatic
        fun queryOwner(groupid: Long):String{
            return getowner(groupid)
        }
        //build forward message
        @JvmStatic
        fun buildforward(text:String):String{
            return runBlocking {
                 buildforwardMsg(text)
            }
        }
        @JvmStatic
        fun nfroperation(text: String, sign: Boolean):String{
            return runBlocking {
                if (sign) accpetFriendRequest(text)
                else rejectFriendRequest(text)
            }
        }
        @JvmStatic
        fun gioperation(text: String, sign:Boolean):String{
            return runBlocking {
                if(sign) accpetGroupInvite(text)
                else rejectGroupInvite(text)
            }
        }
    }
    external fun Verify(): String
    external fun Event(content: String): String
    external fun PluginDisable(): Void
}
