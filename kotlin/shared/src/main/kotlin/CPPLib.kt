/*
 * Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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
 *
 */

package tech.eritquearcus.miraicp.shared

import com.google.gson.Gson
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.utils.MiraiLogger
import org.json.JSONObject
import tech.eritquearcus.miraicp.shared.PublicShared.accpetFriendRequest
import tech.eritquearcus.miraicp.shared.PublicShared.accpetGroupInvite
import tech.eritquearcus.miraicp.shared.PublicShared.basicSendLog
import tech.eritquearcus.miraicp.shared.PublicShared.deleteOnlineAnnouncement
import tech.eritquearcus.miraicp.shared.PublicShared.getowner
import tech.eritquearcus.miraicp.shared.PublicShared.groupSetting
import tech.eritquearcus.miraicp.shared.PublicShared.gson
import tech.eritquearcus.miraicp.shared.PublicShared.isUploaded
import tech.eritquearcus.miraicp.shared.PublicShared.kkick
import tech.eritquearcus.miraicp.shared.PublicShared.kqueryM
import tech.eritquearcus.miraicp.shared.PublicShared.logger
import tech.eritquearcus.miraicp.shared.PublicShared.memberJoinRequest
import tech.eritquearcus.miraicp.shared.PublicShared.modifyAdmin
import tech.eritquearcus.miraicp.shared.PublicShared.mute
import tech.eritquearcus.miraicp.shared.PublicShared.nextMsg
import tech.eritquearcus.miraicp.shared.PublicShared.publishOfflineAnnouncement
import tech.eritquearcus.miraicp.shared.PublicShared.queryBFL
import tech.eritquearcus.miraicp.shared.PublicShared.queryBGL
import tech.eritquearcus.miraicp.shared.PublicShared.queryImgInfo
import tech.eritquearcus.miraicp.shared.PublicShared.queryML
import tech.eritquearcus.miraicp.shared.PublicShared.recallMsg
import tech.eritquearcus.miraicp.shared.PublicShared.refreshInfo
import tech.eritquearcus.miraicp.shared.PublicShared.remoteFileInfo
import tech.eritquearcus.miraicp.shared.PublicShared.scheduling
import tech.eritquearcus.miraicp.shared.PublicShared.sendError
import tech.eritquearcus.miraicp.shared.PublicShared.sendFile
import tech.eritquearcus.miraicp.shared.PublicShared.sendForwardMsg
import tech.eritquearcus.miraicp.shared.PublicShared.sendMiraiCode
import tech.eritquearcus.miraicp.shared.PublicShared.sendMsg
import tech.eritquearcus.miraicp.shared.PublicShared.sendNudge
import tech.eritquearcus.miraicp.shared.PublicShared.sendWarning
import tech.eritquearcus.miraicp.shared.PublicShared.sendWithQuote
import tech.eritquearcus.miraicp.shared.PublicShared.uploadImg
import tech.eritquearcus.miraicp.shared.PublicShared.uploadVoice

class CPPLib(
    // 本地地址
    val libPath: String?,
    // 依赖dll地址, 为了解决数据库依赖之类的
    private val dependencies: List<String>?,
    // 只在libPath == null下生效
    callback: () -> Unit = {}
) {
    var config: PluginConfig

    init {
        config = if (libPath == null) {
            PublicShared.cpp.add(this)
            callback()
            Gson().fromJson(Verify("0"), PluginConfig::class.java)
        } else {
            dependencies?.forEach {
                System.load(it)
            }
            System.load(libPath)
            val last = PublicShared.cpp.size
            PublicShared.cpp.add(this)
            val now = PublicShared.cpp.size
            val precise = if (now - last == 1) (now - 1) else PublicShared.cpp.indexOf(this)
            Gson().fromJson(Verify(precise.toString()), PluginConfig::class.java)
        }
    }

    fun showInfo(logger: MiraiLogger = PublicShared.logger, version: String = PublicShared.now_tag) {
        logger.info("⭐已加载插件(${config.id}): ${config.name}")
        logger.info("⭐作者: ${config.author}")
        logger.info("⭐版本: ${config.version}")
        logger.info("⭐本机地址: $libPath")
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
        private fun contact(source: String): Config.Contact = gson.fromJson(source, Config.Contact::class.java)
        private suspend fun KSend(source: String, miraiCode: Boolean, retryTime: Int): String =
            run {
                val tmp = gson.fromJson(source, Config.SendRequest::class.java)
                return when (miraiCode) {
                    false -> sendMsg(tmp.content, tmp.contact, retryTime)
                    true -> sendMiraiCode(tmp.content, tmp.contact, retryTime)
                }
            }

        // 日志发送接口
        @JvmStatic
        fun KSendLog(log: String, level: Int) {
            val j = JSONObject(log)
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

        private suspend fun KSendFile(source: String, contactSource: String): String =
            run {
                val t = JSONObject(source)
                sendFile(
                    t.getString("path"),
                    t.getString("filepath"),
                    contact(contactSource)
                )
            }

        private suspend fun KRemoteFileInfo(source: String, contactSource: String): String =
            run {
                val t = JSONObject(source)
                return if (t.has("path"))
                    remoteFileInfo(
                        t.getString("path"),
                        t.getString("id"),
                        contact(contactSource)
                    )
                else
                    remoteFileInfo(t.getString("id"), contact(contactSource))
            }

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

        private suspend fun kQueryImgInfo(source: String): String =
            gson.fromJson(source, Config.ImgInfo::class.java).let {
                queryImgInfo(it.imageid!!, it.size, it.width, it.height, it.type)
            }

        enum class Operation_code {
            Recall,
            Send,
            RefreshInfo,
            UploadImg,
            QueryBFL,
            QueryBGL,
            SendFile,
            RemoteFileInfo,
            QueryImgInfo,
            MuteM,
            QueryM,
            KickM,
            QueryOwner,
            UploadVoice,
            QueryML,
            GroupSetting,
            Buildforward,
            Nfroperation,
            Gioperation,
            SendWithQuote,
            Announcement,
            Timer,
            Nudge,
            NextMsg,
            ModifyAdmin,
            MemberJoinRequest,
            ImageUploaded,
            CommandReg,
            ChangeNameCard
        }

        // 通用接口
        @JvmStatic
        fun KOperation(content: String): String =
            runBlocking {
                try {
                    val j = JSONObject(content)
                    val root = j.getJSONObject("data")
                    when (j.getInt("type")) {
                        /// 撤回信息
                        Operation_code.Recall.ordinal -> recallMsg(root.getString("source"))
                        /// 发送信息
                        Operation_code.Send.ordinal -> KSend(
                            root.getString("source"),
                            root.getBoolean("miraiCode"),
                            root.getInt("retryTime")
                        )
                        /// 查询信息接口
                        Operation_code.RefreshInfo.ordinal -> refreshInfo(
                            contact(root.getString("source")),
                            root.has("quit"),
                            root.has("announcement")
                        )
                        /// 上传图片
                        Operation_code.UploadImg.ordinal -> uploadImg(
                            root.getString("fileName"),
                            contact(root.getString("source"))
                        )
                        /// 取好友列表
                        Operation_code.QueryBFL.ordinal -> queryBFL(root.getLong("botid"))
                        /// 取群组列表
                        Operation_code.QueryBGL.ordinal -> queryBGL(root.getLong("botid"))
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
                        Operation_code.QueryImgInfo.ordinal -> kQueryImgInfo(root.toString())
                        /// 禁言
                        Operation_code.MuteM.ordinal -> mute(
                            root.getInt("time"),
                            contact(root.getString("contactSource"))
                        )
                        /// 查询权限
                        Operation_code.QueryM.ordinal -> kqueryM(
                            contact(root.getString("contactSource"))
                        )
                        /// 踢出
                        Operation_code.KickM.ordinal -> kkick(
                            root.getString("message"),
                            contact(root.getString("contactSource"))
                        )
                        /// 取群主
                        Operation_code.QueryOwner.ordinal -> getowner(
                            contact(root.getString("contactSource"))
                        )
                        /// 上传语音
                        Operation_code.UploadVoice.ordinal -> uploadVoice(
                            JSONObject(root.getString("source")).getString("path"),
                            contact(root.getString("contactSource"))
                        )
                        /// 查询群成员列表
                        Operation_code.QueryML.ordinal -> queryML(
                            contact(root.getString("contactSource"))
                        )
                        /// 群设置
                        Operation_code.GroupSetting.ordinal -> groupSetting(
                            contact(root.getString("contactSource")),
                            root.getString("source")
                        )
                        /// 构建转发信息
                        Operation_code.Buildforward.ordinal -> sendForwardMsg(
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
                        Operation_code.SendWithQuote.ordinal -> sendWithQuote(
                            root.getString("messageSource"),
                            root.getString("msg"),
                            root.getString("sign")
                        )
                        /// 群公告操作
                        Operation_code.Announcement.ordinal -> KAnnouncement(
                            root.getString("identify"),
                            root.getOrNull("source")
                        )
                        /// 定时任务
                        Operation_code.Timer.ordinal -> scheduling(root.getLong("time"), root.getString("msg"))
                        /// 发送戳一戳
                        Operation_code.Nudge.ordinal -> sendNudge(
                            contact(root.getString("contactSource"))
                        )
                        /// 下一条信息
                        Operation_code.NextMsg.ordinal -> nextMsg(
                            contact(root.getString("contactSource")),
                            root.getLong("time"),
                            root.getBoolean("halt")
                        )
                        Operation_code.ModifyAdmin.ordinal -> modifyAdmin(
                            contact(root.getString("contactSource")),
                            root.getBoolean("admin")
                        )
                        Operation_code.MemberJoinRequest.ordinal -> memberJoinRequest(
                            root.getString("source"),
                            root.getBoolean("sign"),
                            root.getLong("botid"),
                            root.getString("msg")
                        )
                        Operation_code.ImageUploaded.ordinal -> isUploaded(
                            gson.fromJson(
                                root.toString(),
                                Config.ImgInfo::class.java
                            ), root.getLong("botid")
                        )
                        Operation_code.CommandReg.ordinal -> {
                            PublicShared.commandReg.register(
                                gson.fromJson(
                                    root.getString("command"),
                                    Command::class.java
                                )
                            )
                        }
                        Operation_code.ChangeNameCard.ordinal -> {
                            PublicShared.changeNameCard(
                                contact(root.getString("contactSource")),
                                root.getString("newName")
                            )
                        }
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

    external fun Verify(pluginid: String): String
    external fun Event(content: String): String
    external fun PluginDisable(): Void
}
