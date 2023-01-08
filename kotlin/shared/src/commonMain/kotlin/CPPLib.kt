/*
 * Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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
        val tmp = json.decodeFromString<Packets.Incoming.Request>(text)
        PublicShared.accpetFriendRequest(tmp.text, tmp.botid, tmp.accept, tmp.ban)
    }

    // Group invite operation
    suspend fun KGioperation(text: String): String = run {
        val tmp = json.decodeFromString<Packets.Incoming.Request>(text)
        PublicShared.acceptGroupInvite(tmp.text, tmp.botid, tmp.accept)
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
        PublicShared.queryImgInfo(it.imageId!!, it.size, it.width, it.height, it.imageType)
    }


    // 通用接口
    fun operation(content: String): String = runBlocking {
        try {
            val j = json.decodeFromString<Packets.Incoming.OperationPacket>(content)
            Packets.Incoming.operations[Packets.Incoming.OperationCode.values()[j.type]]!!(j.data)
        } catch (e: Exception) {
            println(e.message)
            println(content)
            e.printStackTrace()
            "EA"
        }
    }
}