/*
 * Copyright (C) 2020-2021 Eritque arcus and contributors.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version(in your opinion).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package tech.eritquearcus.miraicp.shared

import com.google.gson.Gson
import kotlinx.coroutines.delay
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.utils.MiraiLogger
import org.json.JSONObject
import tech.eritquearcus.miraicp.shared.PublicShared.QueryBFL
import tech.eritquearcus.miraicp.shared.PublicShared.QueryBGL
import tech.eritquearcus.miraicp.shared.PublicShared.QueryImg
import tech.eritquearcus.miraicp.shared.PublicShared.QueryML
import tech.eritquearcus.miraicp.shared.PublicShared.RefreshInfo
import tech.eritquearcus.miraicp.shared.PublicShared.SendMiraiCode
import tech.eritquearcus.miraicp.shared.PublicShared.SendMsg
import tech.eritquearcus.miraicp.shared.PublicShared.accpetFriendRequest
import tech.eritquearcus.miraicp.shared.PublicShared.accpetGroupInvite
import tech.eritquearcus.miraicp.shared.PublicShared.basicSendLog
import tech.eritquearcus.miraicp.shared.PublicShared.buildforwardMsg
import tech.eritquearcus.miraicp.shared.PublicShared.deleteOnlineAnnouncement
import tech.eritquearcus.miraicp.shared.PublicShared.getowner
import tech.eritquearcus.miraicp.shared.PublicShared.groupSetting
import tech.eritquearcus.miraicp.shared.PublicShared.gson
import tech.eritquearcus.miraicp.shared.PublicShared.kkick
import tech.eritquearcus.miraicp.shared.PublicShared.kqueryM
import tech.eritquearcus.miraicp.shared.PublicShared.memberJoinRequest
import tech.eritquearcus.miraicp.shared.PublicShared.modifyAdmin
import tech.eritquearcus.miraicp.shared.PublicShared.mute
import tech.eritquearcus.miraicp.shared.PublicShared.nextMsg
import tech.eritquearcus.miraicp.shared.PublicShared.publishOfflineAnnouncement
import tech.eritquearcus.miraicp.shared.PublicShared.recallMsg
import tech.eritquearcus.miraicp.shared.PublicShared.remoteFileInfo
import tech.eritquearcus.miraicp.shared.PublicShared.scheduling
import tech.eritquearcus.miraicp.shared.PublicShared.sendError
import tech.eritquearcus.miraicp.shared.PublicShared.sendFile
import tech.eritquearcus.miraicp.shared.PublicShared.sendNudge
import tech.eritquearcus.miraicp.shared.PublicShared.sendWarning
import tech.eritquearcus.miraicp.shared.PublicShared.sendWithQuote
import tech.eritquearcus.miraicp.shared.PublicShared.uploadImg
import tech.eritquearcus.miraicp.shared.PublicShared.uploadVoice

class CPP_lib (
    val dll_path: String,
    val dependencies: List<String>?
){
    var config:PluginConfig

    init {
        dependencies?.forEach {
            System.load(it)
        }
        System.load(dll_path)
        config = Gson().fromJson(Verify(), PluginConfig::class.java)
    }

    fun showInfo(logger: MiraiLogger = PublicShared.logger, version: String = PublicShared.now_tag) {
        logger.info("⭐已加载插件(${config.id}): ${config.name}")
        logger.info("⭐作者: ${config.author}")
        logger.info("⭐版本: ${config.version}")
        logger.info("⭐本机地址: $dll_path")
        if (dependencies != null) logger.info("⭐依赖dll: ${dependencies.joinToString(" ")}")
        if (config.description != "")
            logger.info("⭐描述: ${config.description}")
        if (config.time != "")
            logger.info("⭐发行时间: ${config.time}")
        if (config.MiraiCPversion != version) {
            logger.warning("Warning: 当前MiraiCP框架版本($version)和加载的插件的C++ SDK(${config.MiraiCPversion})不一致")
        }
    }
    //cd shared/build/classes/kotlin/main && javap.exe -s tech.eritquearcus.miraicp.shared.CPP_lib
    companion object {
        var test: Boolean = false
        private suspend fun KSend(source: String, miraiCode: Boolean, retryTime: Int): String =
            run {
                val tmp = gson.fromJson(source, Config.SendRequest::class.java)
                if (test) {
                    when (tmp.contact.type) {
                        1 -> println("send [${Thread.currentThread().name}] Friend<MiraiCode: $miraiCode>: ${tmp.content}")
                        2 -> println("send [${Thread.currentThread().name}] Group<MiraiCode: $miraiCode>: ${tmp.content}")
                        3 -> println("send [${Thread.currentThread().name}] Member<MiraiCode: $miraiCode>: ${tmp.content}")
                    }
                    delay(171)
                    return """
                               {"kind":"GROUP","botId":692928873,"ids":[3926],"internalIds":[1921344034],"time":1629788808,"fromId":692928873,"targetId":788189105,"originalMessage":[{"type":"PlainText","content":"x"}]}
                                            """.trimIndent()
                }
                return when (miraiCode) {
                    false -> SendMsg(tmp.content, tmp.contact, retryTime)
                    true -> SendMiraiCode(tmp.content, tmp.contact, retryTime)
                }
            }

        //recall messageSource
        private suspend fun KRecall(source: String): String = recallMsg(source)

        //查询图片下载链接
        private suspend fun KQueryImgUrl(id: String): String = QueryImg(id)

        @JvmStatic
        fun KSendLog(log: String, level: Int) {
            val j = JSONObject(log)
            if (test) {
                when (level) {
                    0 -> println("I: ${j.getString("log")}")
                    1 -> println("W: ${j.getString("log")}")
                    2 -> println("E: ${j.getString("log")}")
                }
                return
            }
            if (j.getLong("id") == -1L)
                when (level) {
                    0 -> basicSendLog(j.getString("log"), j.getLong("id"), j.getString("name"))
                    1 -> sendWarning(j.getString("log"), j.getLong("id"), j.getString("name"))
                    2 -> sendError(j.getString("log"), j.getLong("id"), j.getString("name"))
                }
            else
                when (level) {
                    0 -> basicSendLog(j.getString("log"), j.getLong("id"))
                    1 -> sendWarning(j.getString("log"), j.getLong("id"))
                    2 -> sendError(j.getString("log"), j.getLong("id"))
                }
        }

        private suspend fun KRefreshInfo(source: String, quit: Boolean, announcement:Boolean): String =
            RefreshInfo(gson.fromJson(source, Config.Contact::class.java), quit, announcement)

        private suspend fun KUploadImg(fileName: String, source: String): String =
            uploadImg(fileName, gson.fromJson(source, Config.Contact::class.java))

        private suspend fun KSendFile(source: String, contactSource: String): String =
            run {
                val t = JSONObject(source)
                sendFile(
                    t.getString("path"),
                    t.getString("filename"),
                    gson.fromJson(contactSource, Config.Contact::class.java)
                )
            }

        private suspend fun KRemoteFileInfo(source: String, contactSource: String): String =
            run {
                val t = JSONObject(source)
                return if(t.has("path"))
                    remoteFileInfo(
                        t.getString("path"),
                        t.getString("id"),
                        gson.fromJson(contactSource, Config.Contact::class.java)
                    )
                else
                    remoteFileInfo(t.getString("id"), gson.fromJson(contactSource, Config.Contact::class.java))
            }

        //mute member
        private suspend fun KMuteM(time: Int, contactSource: String): String =
            mute(time, gson.fromJson(contactSource, Config.Contact::class.java))

        //query the permission of a member in a group
        private fun KQueryM(contactSource: String): String =
            kqueryM(gson.fromJson(contactSource, Config.Contact::class.java))

        //kick a member
        private suspend fun KKickM(message: String, contactSource: String): String =
            kkick(message, gson.fromJson(contactSource, Config.Contact::class.java))

        //query the member list of a group
        private fun KQueryML(contactSource: String): String =
            QueryML(gson.fromJson(contactSource, Config.Contact::class.java))

        // query the friend lst of the bot
        private fun KQueryBFL(botid: Long): String {
            return QueryBFL(botid)
        }

        // query the group list of the bot
        private fun KQueryBGL(botid: Long): String {
            return QueryBGL(botid)
        }

        //query the owner of a group
        private fun KQueryOwner(contactSource: String): String =
            getowner(gson.fromJson(contactSource, Config.Contact::class.java))

        //build forward message
        private suspend fun KBuildforward(text: String, botid: Long): String =
            buildforwardMsg(text, botid)

        // new friend request operation
        private suspend fun KNfroperation(text: String): String =
            run {
                val tmp = gson.fromJson(text, CPPEvent.Request::class.java)
                accpetFriendRequest(tmp.text, tmp.botid, tmp.accept, tmp.ban)
            }

        // Group invite operation
        private suspend fun KGioperation(text: String): String =
            run {
                val tmp = gson.fromJson(text, CPPEvent.Request::class.java)
                accpetGroupInvite(tmp.text, tmp.botid, tmp.accept)
            }

        private suspend fun KSendWithQuote(messageSource: String, msg: String, sign: String): String =
            sendWithQuote(messageSource, msg, sign)


        private fun KUpdateSetting(contactSource: String, source: String): String =
            groupSetting(gson.fromJson(contactSource, Config.Contact::class.java), source)

        private suspend fun KUploadVoice(contactSource: String, source: String): String =
            JSONObject(source).let { tmp ->
                uploadVoice(tmp.getString("path"), gson.fromJson(contactSource, Config.Contact::class.java))
            }

        private suspend fun KAnnouncement(identify: String, source: String?): String =
            gson.fromJson(identify, Config.IdentifyA::class.java).let { i ->
                return when (i.type) {
                    1 -> {
                        deleteOnlineAnnouncement(i)
                    }
                    2 -> {
                        publishOfflineAnnouncement(i, gson.fromJson(source!!, Config.BriefOfflineA::class.java))
                    }
                    else -> {
                        "EA"
                    }
                }
            }

        private fun KNudge(contactSource: String): String =
            sendNudge(gson.fromJson(contactSource, Config.Contact::class.java))

        private fun KNextMsg(contactSource: String, time: Long, halt: Boolean): String =
            nextMsg(gson.fromJson(contactSource, Config.Contact::class.java), time, halt)

        private suspend fun KModifyAdmin(contactSource: String, admin: Boolean): String =
            modifyAdmin(gson.fromJson(contactSource, Config.Contact::class.java), admin)

        private suspend fun KMemberJoinRequest(source: String, sign:Boolean, botid: Long, msg: String)=
            memberJoinRequest(source, sign, botid, msg)

        enum class Operation_code {
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
            /// 上传语音
            UploadVoice,
            /// 查询群成员列表
            QueryML,
            /// 群设置
            GroupSetting,
            /// 构建转发信息
            Buildforward,
            /// 好友申请事件
            Nfroperation,
            /// 群聊邀请事件
            Gioperation,
            /// 回复(引用并发送)
            SendWithQuote,
            /// 群公告操作
            Announcement,
            /// 定时任务
            Timer,
            ///发送戳一戳
            Nudge,
            /// 好友对象下一条消息
            NextMsg,
            /// 更改群成员权限
            ModifyAdmin,
            /// 群成员申请
            MemberJoinRequest
        }

        @JvmStatic
        fun KOperation(content: String): String =
            runBlocking {
                try {
                    val j = JSONObject(content)
                    val root = j.getJSONObject("data")
                    when (j.getInt("type")) {
                        /// 撤回信息
                        Operation_code.Recall.ordinal -> KRecall(root.getString("source"))
                        /// 发送信息
                        Operation_code.Send.ordinal -> KSend(
                            root.getString("source"),
                            root.getBoolean("miraiCode"),
                            root.getInt("retryTime")
                        )
                        /// 查询信息接口
                        Operation_code.RefreshInfo.ordinal -> KRefreshInfo(
                            root.getString("source"),
                            root.has("quit"),
                            root.has("announcement")
                        )
                        /// 上传图片
                        Operation_code.UploadImg.ordinal -> KUploadImg(
                            root.getString("fileName"),
                            root.getString("source")
                        )
                        /// 取好友列表
                        Operation_code.QueryBFL.ordinal -> KQueryBFL(root.getLong("botid"))
                        /// 取群组列表
                        Operation_code.QueryBGL.ordinal -> KQueryBGL(root.getLong("botid"))
                        /// 上传文件
                        Operation_code.SendFile.ordinal -> KSendFile(
                            root.getString("source"),
                            root.getString("contactSource")
                        )
                        /// 查询文件信息
                        Operation_code.RemoteFileInfo.ordinal -> KRemoteFileInfo(
                            root.getString("source"),
                            root.getString("contactSource")
                        )
                        /// 查询图片下载地址
                        Operation_code.QueryImgUrl.ordinal -> KQueryImgUrl(root.getString("id"))
                        /// 禁言
                        Operation_code.MuteM.ordinal -> KMuteM(root.getInt("time"), root.getString("contactSource"))
                        /// 查询权限
                        Operation_code.QueryM.ordinal -> KQueryM(root.getString("contactSource"))
                        /// 踢出
                        Operation_code.KickM.ordinal -> KKickM(
                            root.getString("message"),
                            root.getString("contactSource")
                        )
                        /// 取群主
                        Operation_code.QueryOwner.ordinal -> KQueryOwner(root.getString("contactSource"))
                        /// 上传语音
                        Operation_code.UploadVoice.ordinal -> KUploadVoice(
                            root.getString("contactSource"),
                            root.getString("source")
                        )
                        /// 查询群成员列表
                        Operation_code.QueryML.ordinal -> KQueryML(root.getString("contactSource"))
                        /// 群设置
                        Operation_code.GroupSetting.ordinal -> KUpdateSetting(
                            root.getString("contactSource"),
                            root.getString("source")
                        )
                        /// 构建转发信息
                        Operation_code.Buildforward.ordinal -> KBuildforward(
                            root.getString("text"),
                            root.getLong("botid")
                        )
                        /// 好友申请事件
                        Operation_code.Nfroperation.ordinal -> KNfroperation(
                            root.toString()
                        )
                        /// 群聊邀请事件
                        Operation_code.Gioperation.ordinal -> KGioperation(
                            root.toString()
                        )
                        /// 回复(引用并发送)
                        Operation_code.SendWithQuote.ordinal -> KSendWithQuote(
                            root.getString("messageSource"),
                            root.getString("msg"),
                            root.getString("sign")
                        )
                        /// 群公告操作
                        Operation_code.Announcement.ordinal -> KAnnouncement(
                            root.getString("identify"),
                            if (root.has("source")) root.getString("source") else null
                        )
                        /// 定时任务
                        Operation_code.Timer.ordinal -> scheduling(root.getLong("time"), root.getString("msg"))
                        /// 发送戳一戳
                        Operation_code.Nudge.ordinal -> KNudge(root.getString("contactSource"))
                        /// 下一条信息
                        Operation_code.NextMsg.ordinal -> KNextMsg(
                            root.getString("contactSource"),
                            root.getLong("time"),
                            root.getBoolean("halt")
                        )
                        Operation_code.ModifyAdmin.ordinal -> KModifyAdmin(
                            root.getString("contactSource"),
                            root.getBoolean("admin")
                        )
                        Operation_code.MemberJoinRequest.ordinal-> KMemberJoinRequest(
                            root.getString("source"),
                            root.getBoolean("sign"),
                            root.getLong("botid"),
                            root.getString("msg")
                        )
                        else -> "EA"
                    }
                } catch (e: Exception) {
                    println(e.message)
                    println(content)
                    e.printStackTrace()
                    "EA"
                }
            }
    }
    private external fun Verify(): String
    external fun Event(content: String): String
    external fun PluginDisable(): Void
}
