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
import kotlinx.serialization.decodeFromString
import kotlinx.serialization.encodeToString
import tech.eritquearcus.miraicp.shared.PublicShared.json

expect object CPPLibMultiplatform {
    fun init(
        // 本地地址
        libPath: List<String>?, cfgPath: String?,
        // 只在libPath == null下生效
        callback: () -> Unit
    )
}

object CPPLib {
    fun init(
        // 本地地址
        libPath: List<String>? = null,
        cfgPath: String? = null,
        // 只在libPath == null下生效
        callback: () -> Unit = {}
    ) = CPPLibMultiplatform.init(libPath, cfgPath, callback)

    fun contact(source: String): Config.Contact = json.decodeFromString(source)
    suspend fun KSend(source: String, miraiCode: Boolean): String = run {
        val tmp = json.decodeFromString<Config.SendRequest>(source)
        return when (miraiCode) {
            false -> PublicShared.sendMsg(tmp.content, tmp.contact)
            true -> PublicShared.sendMiraiCode(tmp.content, tmp.contact)
        }
    }

    // 日志发送接口
    fun sendLog(log: String, level: Int) {
        val j = json.decodeFromString<Config.Log>(log)
        if (j.id == -1L) when (level) {
            0 -> PublicShared.basicSendLog(j.log, j.id, j.name!!)
            1 -> PublicShared.sendWarning(j.log, j.id, j.name!!)
            2 -> PublicShared.sendError(j.log, j.id, j.name!!)
        }
        else when (level) {
            0 -> PublicShared.basicSendLog(j.log, j.id)
            1 -> PublicShared.sendWarning(j.log, j.id)
            2 -> PublicShared.sendError(j.log, j.id)
        }
    }

    suspend fun KSendFile(source: String, contactSource: String): String = run {
        val t = json.decodeFromString<Config.FileInfoIn>(source)
        PublicShared.sendFile(
            t.path!!, t.filepath!!, contact(contactSource)
        )
    }

    suspend fun KRemoteFileInfo(source: String, contactSource: String): String = run {
        val t = json.decodeFromString<Config.FileInfoIn>(source)
        return if (t.path != null) PublicShared.remoteFileInfo(
            t.path, t.id!!, contact(contactSource)
        )
        else PublicShared.remoteFileInfo(t.id!!, contact(contactSource))
    }

    // new friend request operation
    suspend fun KNfroperation(text: String): String = run {
        val tmp = json.decodeFromString<CPPEvent.Request>(text)
        PublicShared.accpetFriendRequest(tmp.text, tmp.botid, tmp.accept, tmp.ban)
    }

    // Group invite operation
    suspend fun KGioperation(text: String): String = run {
        val tmp = json.decodeFromString<CPPEvent.Request>(text)
        PublicShared.accpetGroupInvite(tmp.text, tmp.botid, tmp.accept)
    }

    suspend fun KAnnouncement(identify: String, source: String?): String =
        json.decodeFromString<Config.IdentifyA>(identify).let { i ->
            return when (i.type) {
                1 -> {
                    PublicShared.deleteOnlineAnnouncement(i)
                }

                2 -> {
                    PublicShared.publishOfflineAnnouncement(
                        i, json.decodeFromString(source!!)
                    )
                }

                else -> {
                    "EA"
                }
            }
        }

    suspend fun kQueryImgInfo(source: String): String = json.decodeFromString<Config.ImgInfo>(source).let {
        PublicShared.queryImgInfo(it.imageid!!, it.size, it.width, it.height, it.type)
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
    fun operation(content: String): String = runBlocking {
        try {
            val j = json.decodeFromString<Config.Operation>(content)
            val root = j.data
            when (j.type) {
                /// 撤回信息
                Operation_code.Recall.ordinal -> PublicShared.recallMsg(root.source!!)
                /// 发送信息
                Operation_code.Send.ordinal -> KSend(
                    root.source!!, root.miraiCode!!
                )
                /// 查询信息接口
                Operation_code.RefreshInfo.ordinal -> PublicShared.refreshInfo(
                    contact(root.source!!), root.quit != null, root.announcement != null
                )
                /// 上传图片
                Operation_code.UploadImg.ordinal -> PublicShared.uploadImg(
                    root.fileName!!, contact(root.source!!)
                )
                /// 取好友列表
                Operation_code.QueryBFL.ordinal -> PublicShared.queryBFL(root.botid!!)
                /// 取群组列表
                Operation_code.QueryBGL.ordinal -> PublicShared.queryBGL(root.botid!!)
                /// 上传文件
                Operation_code.SendFile.ordinal -> KSendFile(
                    root.source!!, root.contactSource!!
                )
                /// 查询文件信息
                Operation_code.RemoteFileInfo.ordinal -> KRemoteFileInfo(
                    root.source!!, root.contactSource!!
                )
                /// 查询图片下载地址
                Operation_code.QueryImgInfo.ordinal -> kQueryImgInfo(root.toString())
                /// 禁言
                Operation_code.MuteM.ordinal -> PublicShared.mute(
                    root.time!!, contact(root.contactSource!!)
                )
                /// 查询权限
                Operation_code.QueryM.ordinal -> PublicShared.kqueryM(
                    contact(root.contactSource!!)
                )
                /// 踢出
                Operation_code.KickM.ordinal -> PublicShared.kkick(
                    root.message!!, contact(root.contactSource!!)
                )
                /// 取群主
                Operation_code.QueryOwner.ordinal -> PublicShared.getowner(
                    contact(root.contactSource!!)
                )
                /// 上传语音
                Operation_code.UploadVoice.ordinal -> PublicShared.uploadVoice(
                    root.source!!, contact(root.contactSource!!)
                )
                /// 查询群成员列表
                Operation_code.QueryML.ordinal -> PublicShared.queryML(
                    contact(root.contactSource!!)
                )
                /// 群设置
                Operation_code.GroupSetting.ordinal -> PublicShared.groupSetting(
                    contact(root.contactSource!!), root.source!!
                )
                /// 构建转发信息
                Operation_code.Buildforward.ordinal -> PublicShared.sendForwardMsg(
                    root.text!!, root.botid!!
                )
                /// 好友申请事件
                Operation_code.Nfroperation.ordinal -> KNfroperation(
                    json.encodeToString(root)
                )
                /// 群聊邀请事件
                Operation_code.Gioperation.ordinal -> KGioperation(
                    json.encodeToString(root)
                )
                /// 回复(引用并发送)
                Operation_code.SendWithQuote.ordinal -> PublicShared.sendWithQuote(
                    root.messageSource!!, root.msg!!, root.sign!!
                )
                /// 群公告操作
                Operation_code.Announcement.ordinal -> KAnnouncement(
                    root.identify!!, root.source
                )
                /// 定时任务
                Operation_code.Timer.ordinal -> PublicShared.scheduling(root.time!!, root.msg!!)
                /// 发送戳一戳
                Operation_code.Nudge.ordinal -> PublicShared.sendNudge(
                    contact(root.contactSource!!)
                )
                /// 下一条信息
                Operation_code.NextMsg.ordinal -> PublicShared.nextMsg(
                    contact(root.contactSource!!), root.time!!, root.halt!!
                )

                Operation_code.ModifyAdmin.ordinal -> PublicShared.modifyAdmin(
                    contact(root.contactSource!!), root.admin!!
                )

                Operation_code.MemberJoinRequest.ordinal -> PublicShared.memberJoinRequest(
                    root.source!!, root.sign!! == "true" || root.sign == "True", root.botid!!, root.msg!!
                )

                Operation_code.ImageUploaded.ordinal -> PublicShared.isUploaded(
                    json.decodeFromString<Config.ImgInfo>(
                        root.toString()
                    ), root.botid!!
                )

                Operation_code.CommandReg.ordinal -> {
                    PublicSharedData.commandReg.register(
                        json.decodeFromString<Command>(
                            root.command!!
                        )
                    )
                }

                Operation_code.ChangeNameCard.ordinal -> {
                    PublicShared.changeNameCard(
                        contact(root.contactSource!!), root.newName!!
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