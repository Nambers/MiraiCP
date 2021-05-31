package tech.eritquearcus.miraicp
import com.google.gson.Gson
import kotlinx.coroutines.runBlocking
import org.json.JSONObject
import tech.eritquearcus.miraicp.PluginMain.BasicSendLog
import tech.eritquearcus.miraicp.PluginMain.QueryBFL
import tech.eritquearcus.miraicp.PluginMain.QueryBGL
import tech.eritquearcus.miraicp.PluginMain.QueryImg
import tech.eritquearcus.miraicp.PluginMain.QueryML
import tech.eritquearcus.miraicp.PluginMain.RefreshInfo
import tech.eritquearcus.miraicp.PluginMain.SendError
import tech.eritquearcus.miraicp.PluginMain.SendMiraiCode
import tech.eritquearcus.miraicp.PluginMain.SendMsg
import tech.eritquearcus.miraicp.PluginMain.SendWarning
import tech.eritquearcus.miraicp.PluginMain.accpetFriendRequest
import tech.eritquearcus.miraicp.PluginMain.accpetGroupInvite
import tech.eritquearcus.miraicp.PluginMain.buildforwardMsg
import tech.eritquearcus.miraicp.PluginMain.dll_name
import tech.eritquearcus.miraicp.PluginMain.getowner
import tech.eritquearcus.miraicp.PluginMain.kkick
import tech.eritquearcus.miraicp.PluginMain.kqueryM
import tech.eritquearcus.miraicp.PluginMain.mute
import tech.eritquearcus.miraicp.PluginMain.muteall
import tech.eritquearcus.miraicp.PluginMain.recallMsg
import tech.eritquearcus.miraicp.PluginMain.rejectFriendRequest
import tech.eritquearcus.miraicp.PluginMain.rejectGroupInvite
import tech.eritquearcus.miraicp.PluginMain.remoteFileInfo
import tech.eritquearcus.miraicp.PluginMain.scheduling
import tech.eritquearcus.miraicp.PluginMain.sendFile
import tech.eritquearcus.miraicp.PluginMain.sendWithQuote
import tech.eritquearcus.miraicp.PluginMain.uploadImg

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
        fun KSendLog(log:String, level: Int):Unit{
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
        fun KSendFile(source:String, contactSource:String):String{
            return runBlocking {
                val t = JSONObject(source)
                sendFile(t.getString("path"), t.getString("filename"), gson.fromJson(contactSource, Config.Contact::class.java))
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
        fun KMuteM(time: Int, contactSource: String): String{
            return runBlocking {
                mute(time, gson.fromJson(contactSource, Config.Contact::class.java))
            }
        }
        //query the permission of a member in a group
        @JvmStatic
        fun KQueryM(contactSource: String): String{
            return kqueryM(gson.fromJson(contactSource, Config.Contact::class.java))
        }
        //kick a member
        @JvmStatic
        fun KKickM(message: String, contactSource: String):String{
            return runBlocking {
                kkick(message, gson.fromJson(contactSource, Config.Contact::class.java))
            }
        }
        //Mute the whole group
        @JvmStatic
        fun KMuteGroup(sign: Boolean, contactSource: String):String{
            return muteall(sign, gson.fromJson(contactSource, Config.Contact::class.java))
        }
        //query the member list of a group
        @JvmStatic
        fun KQueryML(contactSource: String):String{
            return QueryML(gson.fromJson(contactSource, Config.Contact::class.java))
        }
        // query the friend lst of the bot
        @JvmStatic
        fun KQueryBFL(botid: Long): String{
            return QueryBFL(botid)
        }
        // query the group list of the bot
        @JvmStatic
        fun KQueryBGL(botid: Long): String{
            return QueryBGL(botid)
        }
        //query the owner of a group
        @JvmStatic
        fun KQueryOwner(contactSource: String):String{
            return getowner(gson.fromJson(contactSource, Config.Contact::class.java))
        }
        //build forward message
        @JvmStatic
        fun KBuildforward(text:String, botid: Long):String{
            return runBlocking {
                buildforwardMsg(text, botid)
            }
        }
        @JvmStatic
        // new friend request operation
        fun KNfroperation(text: String, sign: Boolean):String{
            return runBlocking {
                val tmp = gson.fromJson(text, Config.NewFriendRequestSource::class.java)
                if (sign) accpetFriendRequest(tmp)
                else rejectFriendRequest(tmp)
            }
        }
        // Group invite operation
        @JvmStatic
        fun KGioperation(text: String, sign:Boolean):String{
            return runBlocking {
                if (sign) accpetGroupInvite(gson.fromJson(text, Config.GroupInviteSource::class.java))
                else rejectGroupInvite(gson.fromJson(text, Config.GroupInviteSource::class.java))
            }
        }
        @JvmStatic
        fun KSendWithQuote(messageSource: String, msg:String, sign: String): String{
            return runBlocking {
                sendWithQuote(messageSource, msg, sign)
            }
        }
    }
    external fun Verify(): String
    external fun Event(content: String): String
    external fun PluginDisable(): Void
}
