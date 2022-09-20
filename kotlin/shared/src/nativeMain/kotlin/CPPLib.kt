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

actual object CPPLib {
    actual fun init(
        libPath: List<String>?,
        cfgPath: String?,
        callback: () -> Unit
    ) {
    }

    actual fun contact(source: String): tech.eritquearcus.miraicp.shared.Config.Contact {
        TODO("Not yet implemented")
    }

    actual suspend fun KSend(source: String, miraiCode: Boolean): String {
        TODO("Not yet implemented")
    }

    actual fun KSendLog(log: String, level: Int) {
    }

    actual suspend fun KSendFile(source: String, contactSource: String): String {
        TODO("Not yet implemented")
    }

    actual suspend fun KRemoteFileInfo(source: String, contactSource: String): String {
        TODO("Not yet implemented")
    }

    actual suspend fun KNfroperation(text: String): String {
        TODO("Not yet implemented")
    }

    actual suspend fun KGioperation(text: String): String {
        TODO("Not yet implemented")
    }

    actual suspend fun KAnnouncement(identify: String, source: String?): String {
        TODO("Not yet implemented")
    }

    actual suspend fun kQueryImgInfo(source: String): String {
        TODO("Not yet implemented")
    }

    actual fun KOperation(content: String): String {
        TODO("Not yet implemented")
    }

}