package org.example.mirai.plugin
import kotlinx.coroutines.*
import org.example.mirai.plugin.PluginMain.BasicSendLog
import org.example.mirai.plugin.PluginMain.Send
import org.example.mirai.plugin.PluginMain.dll_name
import org.example.mirai.plugin.PluginMain.GetN
import org.example.mirai.plugin.PluginMain.GetNN
import org.example.mirai.plugin.PluginMain.QueryImg
import org.example.mirai.plugin.PluginMain.SendError
import org.example.mirai.plugin.PluginMain.SendG
import org.example.mirai.plugin.PluginMain.SendWarning
import org.example.mirai.plugin.PluginMain.mute
import org.example.mirai.plugin.PluginMain.scheduling
import org.example.mirai.plugin.PluginMain.uploadImgFriend
import org.example.mirai.plugin.PluginMain.uploadImgGroup
import org.example.mirai.plugin.PluginMain.uploadImgMember

class CPP_lib {
    var ver:String=""
    init {
        ver=Verify()//如果ver不等于2333 载入错误
    }
    //"C:\Program Files\Java\jdk1.8.0_261\bin\javah.exe" org.example.mirai.plugin.CPP_lib
    companion object{
        init {
            //这里填自己的路径
            System.load(dll_name)
        }
        @JvmStatic
        fun SendPrivateMSG(message: String, id: Long){
            // 反向调用发送消息
            GlobalScope.launch {
                Send(message, id)
            }

        }
        @JvmStatic
        fun SendPrivateM2M(message: String, id: Long, gid: Long){
            // 反向调用发送消息
            GlobalScope.launch {
                Send(message, id, gid)
            }

        }
        @JvmStatic
        fun SendGroup(message:String,id:Long) {
            GlobalScope.launch {
                    SendG(message, id)
            }

        }
        //查询图片下载链接
        @JvmStatic
        fun QueryImgUrl(id:String): String {
            var temp = ""
            runBlocking {
                temp = QueryImg(id)
            }
            return temp
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
            var re = ""
            runBlocking {
                re = uploadImgGroup(qqid, fileName)
            }
            return re
        }
        //给好友上传图片以备发送
        @JvmStatic
        fun uploadImgF(qqid: Long, fileName:String): String{
            var re = ""
            runBlocking {
                re = uploadImgFriend(qqid, fileName)
            }
            return re
        }
        //群成员对象上传图片
        @JvmStatic
        fun uploadImgM(groupid: Long, qqid: Long, fileName:String): String{
            var re = ""
            runBlocking {
                re = uploadImgMember(groupid,qqid, fileName)
            }
            return re
        }
        //定时任务
        @JvmStatic
        fun schedule(time:Long, id:Int){
            scheduling(time, id)
            return
        }
        @JvmStatic
        fun muteM(qqid: Long, groupid: Long, time: Int): String{
            var re = ""
            runBlocking {
                re = mute(qqid, groupid, time)
            }
            return re
        }
    }


    external fun Verify(): String
    external fun Event(content: String): String
    external fun PluginDisable(): Void
    external fun ScheduleTask(id:Int): Void
}

