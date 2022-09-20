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

import kotlinx.coroutines.runBlocking
import org.json.JSONObject

@Suppress("UNCHECKED_CAST")
internal fun <T> JSONObject.getOrNull(key: String): T? = if (this.has(key)) (this.get(key) as T) else null

actual object CPPLib {
    actual fun init(
        // 本地地址
        libPath: List<String>?,
        cfgPath: String?,
        // 只在libPath == null下生效
        callback: () -> Unit
    ) {
        if (libPath == null) {
            callback()
        } else {
            System.load(getLibLoader(libPath))
        }
        Verify(BuiltInConstants.version, cfgPath!!)
    }

    //cd shared/build/classes/kotlin/main && javap.exe -s tech.eritquearcus.miraicp.shared.CPP_lib
    actual fun contact(source: String): Config.Contact = PublicShared.gson.fromJson(source, Config.Contact::class.java)
    actual suspend fun KSend(source: String, miraiCode: Boolean): String =
        run {
            val tmp = PublicShared.gson.fromJson(source, Config.SendRequest::class.java)
            return when (miraiCode) {
                false -> PublicShared.sendMsg(tmp.content, tmp.contact)
                true -> PublicShared.sendMiraiCode(tmp.content, tmp.contact)
            }
        }

    // 日志发送接口
    @JvmStatic
    actual fun KSendLog(log: String, level: Int) {
        val j = JSONObject(log)
        if (j.getLong("id") == -1L)
            when (level) {
                0 -> PublicShared.basicSendLog(j.getString("log"), j.getLong("id"), j.getString("name"))
                1 -> PublicShared.sendWarning(j.getString("log"), j.getLong("id"), j.getString("name"))
                2 -> PublicShared.sendError(j.getString("log"), j.getLong("id"), j.getString("name"))
            }
        else
            when (level) {
                0 -> PublicShared.basicSendLog(j.getString("log"), j.getLong("id"))
                1 -> PublicShared.sendWarning(j.getString("log"), j.getLong("id"))
                2 -> PublicShared.sendError(j.getString("log"), j.getLong("id"))
            }
    }

    actual suspend fun KSendFile(source: String, contactSource: String): String =
        run {
            val t = JSONObject(source)
            PublicShared.sendFile(
                t.getString("path"),
                t.getString("filepath"),
                contact(contactSource)
            )
        }

    actual suspend fun KRemoteFileInfo(source: String, contactSource: String): String =
        run {
            val t = JSONObject(source)
            return if (t.has("path"))
                PublicShared.remoteFileInfo(
                    t.getString("path"),
                    t.getString("id"),
                    contact(contactSource)
                )
            else
                PublicShared.remoteFileInfo(t.getString("id"), contact(contactSource))
        }

    // new friend request operation
    actual suspend fun KNfroperation(text: String): String =
        run {
            val tmp = PublicShared.gson.fromJson(text, CPPEvent.Request::class.java)
            PublicShared.accpetFriendRequest(tmp.text, tmp.botid, tmp.accept, tmp.ban)
        }

    // Group invite operation
    actual suspend fun KGioperation(text: String): String =
        run {
            val tmp = PublicShared.gson.fromJson(text, CPPEvent.Request::class.java)
            PublicShared.accpetGroupInvite(tmp.text, tmp.botid, tmp.accept)
        }

    actual suspend fun KAnnouncement(identify: String, source: String?): String =
        PublicShared.gson.fromJson(identify, Config.IdentifyA::class.java).let { i ->
            return when (i.type) {
                1 -> {
                    PublicShared.deleteOnlineAnnouncement(i)
                }

                2 -> {
                    PublicShared.publishOfflineAnnouncement(
                        i,
                        PublicShared.gson.fromJson(source!!, Config.BriefOfflineA::class.java)
                    )
                }

                else -> {
                    "EA"
                }
            }
        }

    actual suspend fun kQueryImgInfo(source: String): String =
        PublicShared.gson.fromJson(source, Config.ImgInfo::class.java).let {
            PublicShared.queryImgInfo(it.imageid!!, it.size, it.width, it.height, it.type)
        }

    // 通用接口
    @JvmStatic
    actual fun KOperation(content: String): String =
        runBlocking {
            try {
                val j = JSONObject(content)
                val root = j.getJSONObject("data")
                when (j.getInt("type")) {
                    /// 撤回信息
                    Operation_code.Recall.ordinal -> PublicShared.recallMsg(root.getString("source"))
                    /// 发送信息
                    Operation_code.Send.ordinal -> KSend(
                        root.getString("source"),
                        root.getBoolean("miraiCode")
                    )
                    /// 查询信息接口
                    Operation_code.RefreshInfo.ordinal -> PublicShared.refreshInfo(
                        contact(root.getString("source")),
                        root.has("quit"),
                        root.has("announcement")
                    )
                    /// 上传图片
                    Operation_code.UploadImg.ordinal -> PublicShared.uploadImg(
                        root.getString("fileName"),
                        contact(root.getString("source"))
                    )
                    /// 取好友列表
                    Operation_code.QueryBFL.ordinal -> PublicShared.queryBFL(root.getLong("botid"))
                    /// 取群组列表
                    Operation_code.QueryBGL.ordinal -> PublicShared.queryBGL(root.getLong("botid"))
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
                    Operation_code.MuteM.ordinal -> PublicShared.mute(
                        root.getInt("time"),
                        contact(root.getString("contactSource"))
                    )
                    /// 查询权限
                    Operation_code.QueryM.ordinal -> PublicShared.kqueryM(
                        contact(root.getString("contactSource"))
                    )
                    /// 踢出
                    Operation_code.KickM.ordinal -> PublicShared.kkick(
                        root.getString("message"),
                        contact(root.getString("contactSource"))
                    )
                    /// 取群主
                    Operation_code.QueryOwner.ordinal -> PublicShared.getowner(
                        contact(root.getString("contactSource"))
                    )
                    /// 上传语音
                    Operation_code.UploadVoice.ordinal -> PublicShared.uploadVoice(
                        JSONObject(root.getString("source")).getString("path"),
                        contact(root.getString("contactSource"))
                    )
                    /// 查询群成员列表
                    Operation_code.QueryML.ordinal -> PublicShared.queryML(
                        contact(root.getString("contactSource"))
                    )
                    /// 群设置
                    Operation_code.GroupSetting.ordinal -> PublicShared.groupSetting(
                        contact(root.getString("contactSource")),
                        root.getString("source")
                    )
                    /// 构建转发信息
                    Operation_code.Buildforward.ordinal -> PublicShared.sendForwardMsg(
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
                    Operation_code.SendWithQuote.ordinal -> PublicShared.sendWithQuote(
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
                    Operation_code.Timer.ordinal -> PublicShared.scheduling(root.getLong("time"), root.getString("msg"))
                    /// 发送戳一戳
                    Operation_code.Nudge.ordinal -> PublicShared.sendNudge(
                        contact(root.getString("contactSource"))
                    )
                    /// 下一条信息
                    Operation_code.NextMsg.ordinal -> PublicShared.nextMsg(
                        contact(root.getString("contactSource")),
                        root.getLong("time"),
                        root.getBoolean("halt")
                    )

                    Operation_code.ModifyAdmin.ordinal -> PublicShared.modifyAdmin(
                        contact(root.getString("contactSource")),
                        root.getBoolean("admin")
                    )

                    Operation_code.MemberJoinRequest.ordinal -> PublicShared.memberJoinRequest(
                        root.getString("source"),
                        root.getBoolean("sign"),
                        root.getLong("botid"),
                        root.getString("msg")
                    )

                    Operation_code.ImageUploaded.ordinal -> PublicShared.isUploaded(
                        PublicShared.gson.fromJson(
                            root.toString(),
                            Config.ImgInfo::class.java
                        ), root.getLong("botid")
                    )

                    Operation_code.CommandReg.ordinal -> {
                        PublicShared.commandReg.register(
                            PublicShared.gson.fromJson(
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


    external fun Verify(version: String, cfgPath: String): Void
    external fun Event(content: String): Void
    external fun PluginDisable(): Void
}