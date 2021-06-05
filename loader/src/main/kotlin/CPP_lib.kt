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
import tech.eritquearcus.miraicp.KotlinMain.logger
import tech.eritquearcus.miraicp.KotlinMain.mute
import tech.eritquearcus.miraicp.KotlinMain.muteall
import tech.eritquearcus.miraicp.KotlinMain.recallMsg
import tech.eritquearcus.miraicp.KotlinMain.rejectFriendRequest
import tech.eritquearcus.miraicp.KotlinMain.rejectGroupInvite
import tech.eritquearcus.miraicp.KotlinMain.remoteFileInfo
import tech.eritquearcus.miraicp.KotlinMain.sendFile
import tech.eritquearcus.miraicp.KotlinMain.sendWithQuote
import tech.eritquearcus.miraicp.KotlinMain.uploadImg

class CPP_lib {
    var ver:String=""
    init {
        ver=Verify()
    }

    //cd build/classes/kotlin/main && javap.exe -s tech.eritquearcus.miraicp.CPP_lib
    companion object{
        private val gson = Gson()
        init {
            System.load(dll_name)
        }

        //send MiraiCode
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
        fun KRecall(source: String):String {
            return runBlocking {
                recallMsg(source)
            }
        }

        //查询图片下载链接
        fun KQueryImgUrl(id:String): String {
            return runBlocking {
                QueryImg(id)
            }
        }

        fun KSendLog(log:String, level: Int):Unit{
            when(level){
                0-> BasicSendLog(log)
                1-> SendWarning(log)
                2-> SendError(log)
            }
        }

        fun KRefreshInfo(source: String): String {
            return RefreshInfo(gson.fromJson(source, Config.Contact::class.java))
        }

        fun KUploadImg(fileName:String,source:String):String{
            return runBlocking {
                uploadImg(fileName, gson.fromJson(source, Config.Contact::class.java))
            }
        }

        fun KSendFile(source:String, contactSource:String):String{
            return runBlocking {
                val t = JSONObject(source)
                sendFile(t.getString("path"), t.getString("filename"), gson.fromJson(contactSource, Config.Contact::class.java))
            }
        }

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

        //mute member
        fun KMuteM(time: Int, contactSource: String): String{
            return runBlocking {
                mute(time, gson.fromJson(contactSource, Config.Contact::class.java))
            }
        }

        //query the permission of a member in a group
        fun KQueryM(contactSource: String): String{
            return kqueryM(gson.fromJson(contactSource, Config.Contact::class.java))
        }

        //kick a member
        fun KKickM(message: String, contactSource: String):String{
            return runBlocking {
                kkick(message, gson.fromJson(contactSource, Config.Contact::class.java))
            }
        }

        //Mute the whole group
        fun KMuteGroup(sign: Boolean, contactSource: String):String{
            return muteall(sign, gson.fromJson(contactSource, Config.Contact::class.java))
        }

        //query the member list of a group
        fun KQueryML(contactSource: String):String{
            return QueryML(gson.fromJson(contactSource, Config.Contact::class.java))
        }

        // query the friend lst of the bot
        fun KQueryBFL(botid: Long): String{
            return QueryBFL(botid)
        }

        // query the group list of the bot
        fun KQueryBGL(botid: Long): String{
            return QueryBGL(botid)
        }

        //query the owner of a group
        fun KQueryOwner(contactSource: String):String{
            return getowner(gson.fromJson(contactSource, Config.Contact::class.java))
        }

        //build forward message
        fun KBuildforward(text:String, botid: Long):String{
            return runBlocking {
                buildforwardMsg(text, botid)
            }
        }

        // new friend request operation
        fun KNfroperation(text: String, sign: Boolean):String{
            return runBlocking {
                val tmp = gson.fromJson(text, Config.NewFriendRequestSource::class.java)
                if (sign) accpetFriendRequest(tmp)
                else rejectFriendRequest(tmp)
            }
        }

        // Group invite operation
        fun KGioperation(text: String, sign: Boolean): String {
            return runBlocking {
                if (sign) accpetGroupInvite(gson.fromJson(text, Config.GroupInviteSource::class.java))
                else rejectGroupInvite(gson.fromJson(text, Config.GroupInviteSource::class.java))
            }
        }

        fun KSendWithQuote(messageSource: String, msg: String, sign: String): String {
            return runBlocking {
                sendWithQuote(messageSource, msg, sign)
            }
        }

        enum class operation_code{
            /// 撤回信息
            Recall,
            /// 发送信息
            Send,
            /// 查询信息接口
            RefreshInfo,
            /// 上传图片
            UploadImg,
            /// 取好友列表
            QueryBFL,
            /// 取群组列表
            QueryBGL,
            /// 上传文件
            SendFile,
            /// 查询文件信息
            RemoteFileInfo,
            /// 查询图片下载地址
            QueryImgUrl,
            /// 禁言
            MuteM,
            /// 查询权限
            QueryM,
            /// 踢出
            KickM,
            /// 取群主
            QueryOwner,
            /// 全员禁言
            MuteGroup,
            /// 查询群成员列表
            QueryML,
            /// 定时任务
            Schedule,
            /// 构建转发信息
            Buildforward,
            /// 好友申请事件
            Nfroperation,
            /// 群聊邀请事件
            Gioperation,
            /// 回复(引用并发送)
            SendWithQuote
        };

        @JvmStatic
        fun KOperation(content: String):String {
            val j = JSONObject(content)
            val root = j.getJSONObject("data")
            try {
                return when (j["type"] as Int) {
                    /// 撤回信息
                    operation_code.Recall.ordinal -> {
                        KRecall(root.getString("source"))
                    }
                    /// 发送信息
                    operation_code.Send.ordinal -> {
                        KSend(root.getString("source"), root.getBoolean("miraiCode"))
                    }
                    /// 查询信息接口
                    operation_code.RefreshInfo.ordinal -> {
                        KRefreshInfo(root.getString("source"))
                    }
                    /// 上传图片
                    operation_code.UploadImg.ordinal -> {
                        KUploadImg(root.getString("fileName"), root.getString("source"))
                    }
                    /// 取好友列表
                    operation_code.QueryBFL.ordinal -> {
                        KQueryBFL(root.getLong("botid"))
                    }
                    /// 取群组列表
                    operation_code.QueryBGL.ordinal -> {
                        KQueryBGL(root.getLong("botid"))
                    }
                    /// 上传文件
                    operation_code.SendFile.ordinal -> {
                        KSendFile(root.getString("source"), root.getString("contactSource"))
                    }
                    /// 查询文件信息
                    operation_code.RemoteFileInfo.ordinal -> {
                        KRemoteFileInfo(root.getString("source"), root.getString("contactSource"))
                    }
                    /// 查询图片下载地址
                    operation_code.QueryImgUrl.ordinal -> {
                        KQueryImgUrl(root.getString("id"))
                    }
                    /// 禁言
                    operation_code.MuteM.ordinal -> {
                        KMuteM(root.getInt("time"), root.getString("contactSource"))
                    }
                    /// 查询权限
                    operation_code.QueryM.ordinal -> {
                        KQueryM(root.getString("contactSource"))
                    }
                    /// 踢出
                    operation_code.KickM.ordinal -> {
                        KKickM(root.getString("message"), root.getString("contactSOurce"))
                    }
                    /// 取群主
                    operation_code.QueryOwner.ordinal -> {
                        KQueryOwner(root.getString("contactSource"))
                    }
                    /// 全员禁言
                    operation_code.MuteGroup.ordinal -> {
                        KMuteGroup(root.getBoolean("sign"), root.getString("contactSource"))
                    }
                    /// 查询群成员列表
                    operation_code.QueryML.ordinal -> {
                        KQueryML(root.getString("contactSource"))
                    }
                    /// 构建转发信息
                    operation_code.Buildforward.ordinal -> {
                        KBuildforward(root.getString("text"), root.getLong("botid"))
                    }
                    /// 好友申请事件
                    operation_code.Nfroperation.ordinal -> {
                        KNfroperation(root.getString("text"), root.getBoolean("sign"))
                    }
                    /// 群聊邀请事件
                    operation_code.Gioperation.ordinal -> {
                        KGioperation(root.getString("text"), root.getBoolean("sign"))
                    }
                    /// 回复(引用并发送)
                    operation_code.SendWithQuote.ordinal -> {
                        KSendWithQuote(root.getString("messageSource"), root.getString("msg"), root.getString("sign"))
                    }
                    else -> "EA"
                }
            }catch(e:Exception){
                logger.error(e.message)
                logger.error(content)
                e.printStackTrace()
                return "EA"
            }
        }
    }
    external fun Verify(): String
    external fun Event(content: String): String
    external fun PluginDisable(): Void
}
