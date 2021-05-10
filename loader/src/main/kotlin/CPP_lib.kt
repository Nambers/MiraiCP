package tech.eritquearcus.miraicp
import com.google.gson.Gson
import kotlinx.coroutines.runBlocking
import org.json.JSONObject
import tech.eritquearcus.miraicp.KotlinMain.BasicSendLog
import tech.eritquearcus.miraicp.KotlinMain.QueryBFL
import tech.eritquearcus.miraicp.KotlinMain.QueryBGL
import tech.eritquearcus.miraicp.KotlinMain.QueryImg
import tech.eritquearcus.miraicp.KotlinMain.QueryML
import tech.eritquearcus.miraicp.KotlinMain.RefreshInfo
import tech.eritquearcus.miraicp.KotlinMain.SendError
import tech.eritquearcus.miraicp.KotlinMain.SendMiraiCode
import tech.eritquearcus.miraicp.KotlinMain.SendMsg
import tech.eritquearcus.miraicp.KotlinMain.SendWarning
import tech.eritquearcus.miraicp.KotlinMain.accpetFriendRequest
import tech.eritquearcus.miraicp.KotlinMain.accpetGroupInvite
import tech.eritquearcus.miraicp.KotlinMain.buildforwardMsg
import tech.eritquearcus.miraicp.KotlinMain.dll_name
import tech.eritquearcus.miraicp.KotlinMain.getowner
import tech.eritquearcus.miraicp.KotlinMain.kkick
import tech.eritquearcus.miraicp.KotlinMain.kqueryM
import tech.eritquearcus.miraicp.KotlinMain.mute
import tech.eritquearcus.miraicp.KotlinMain.muteall
import tech.eritquearcus.miraicp.KotlinMain.recallMsg
import tech.eritquearcus.miraicp.KotlinMain.rejectFriendRequest
import tech.eritquearcus.miraicp.KotlinMain.rejectGroupInvite
import tech.eritquearcus.miraicp.KotlinMain.remoteFileInfo
import tech.eritquearcus.miraicp.KotlinMain.scheduling
import tech.eritquearcus.miraicp.KotlinMain.uploadFile
import tech.eritquearcus.miraicp.KotlinMain.uploadImg

class CPP_lib {
    var ver:String=""
    init {
        ver=Verify()
    }
    //"C:\Program Files\Java\jdk1.8.0_261\bin\javap.exe" -s tech.eritquearcus.miraicp.CPP_lib
    companion object{
        private val gson = Gson()
        init {
            System.load(dll_name)
        }
        //send MiraiCode
        @JvmStatic
        fun KSend(source: String, miraiCode:Boolean):String{
            return runBlocking {
                val tmp = gson.fromJson(source, Config.SendRequest::class.java)
                return@runBlocking when (miraiCode) {
                    false -> SendMsg(tmp.content, tmp.contact)
                    true -> SendMiraiCode(tmp.content, tmp.contact)
                }
            }
        }
        //recall messageSource
        @JvmStatic
        fun KRecall(source: String):String {
            return runBlocking {
                recallMsg(source)
            }
        }
        //查询图片下载链接
        @JvmStatic
        fun KQueryImgUrl(id:String): String {
            return runBlocking {
                QueryImg(id)
            }
        }
        @JvmStatic
        fun KSendLog(log:String, level: Int){
            when(level){
                0-> BasicSendLog(log)
                1-> SendWarning(log)
                2-> SendError(log)
            }
        }
        @JvmStatic
        fun KRefreshInfo(source: String): String {
            return RefreshInfo(gson.fromJson(source, Config.Contact::class.java))
        }
        @JvmStatic
        fun KUploadImg(fileName:String,source:String):String{
            return runBlocking {
                uploadImg(fileName, gson.fromJson(source, Config.Contact::class.java))
            }
        }
        @JvmStatic
        fun KUploadFile(path:String, fileName: String, contactSource:String):String{
            return runBlocking {
                uploadFile(path, fileName, gson.fromJson(contactSource, Config.Contact::class.java))
            }
        }
        @JvmStatic
        fun KRemoteFileInfo(source: String, contactSource: String):String {
            return runBlocking {
                val t = JSONObject(source)
                return@runBlocking remoteFileInfo(
                    t.getString("path"),
                    t.getString("id"),
                    gson.fromJson(contactSource, Config.Contact::class.java)
                )
            }
        }
        //定时任务
        @JvmStatic
        fun KSchedule(time:Long, id:String){
            scheduling(time, id)
        }
        //mute member
        @JvmStatic
        fun KMuteM(qqid: Long, groupid: Long, time: Int): String{
            return runBlocking {
                mute(qqid, groupid, time)
            }
        }
        //query the permission of a member in a group
        @JvmStatic
        fun KQueryM(qqid: Long, groupid: Long): String{
            return kqueryM(qqid, groupid)
        }
        //kick a member
        @JvmStatic
        fun KKickM(qqid: Long, groupid: Long, message: String):String{
            return runBlocking {
                kkick(qqid, groupid, message)
            }
        }
        //Mute the whole group
        @JvmStatic
        fun KMuteGroup(groupid: Long, sign: Boolean):String{
            return muteall(groupid, sign)
        }
        //query the member list of a group
        @JvmStatic
        fun KQueryML(groupid: Long):String{
            return QueryML(groupid)
        }
        // query the friend lst of the bot
        @JvmStatic
        fun KQueryBFL(botid: Long): String{
            return QueryBFL()
        }
        // query the group list of the bot
        @JvmStatic
        fun KQueryBGL(botid: Long): String{
            return QueryBGL()
        }
        //query the owner of a group
        @JvmStatic
        fun KQueryOwner(groupid: Long):String{
            return getowner(groupid)
        }
        //build forward message
        @JvmStatic
        fun KBuildforward(text:String):String{
            return runBlocking {
                 buildforwardMsg(text)
            }
        }
        @JvmStatic
        // new friend request operation
        fun KNfroperation(text: String, sign: Boolean):String{
            return runBlocking {
                if (sign) accpetFriendRequest(text)
                else rejectFriendRequest(text)
            }
        }
        // Group invite operation
        @JvmStatic
        fun KGioperation(text: String, sign:Boolean):String{
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
