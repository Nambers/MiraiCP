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
import tech.eritquearcus.miraicp.shared.PublicSharedData.logger

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

    // 日志发送接口
    fun sendLog(log: String, level: Int) {
        val j = json.decodeFromString<Packets.Incoming.Log>(log)
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

    // 通用接口
    fun operation(content: String): String = runBlocking {
        try {
            val j = json.decodeFromString<Packets.Incoming.OperationPacket>(content)
            Packets.Incoming.operations[Packets.Incoming.OperationCode.values()[j.type]]!!(j.data)
        } catch (e: Exception) {
            logger.error(e.message)
            logger.error(content)
            e.printStackTrace()
            "EA"
        }
    }
}