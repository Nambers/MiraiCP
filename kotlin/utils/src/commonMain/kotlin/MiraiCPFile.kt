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

package tech.eritquearcus.miraicp.uilts

import net.mamoe.mirai.utils.ExternalResource

interface MiraiCPFile {
    // this instance is file or not
    val isFile: Boolean

    // the file extension
    val extension: String

    // absolute path of this instance
    val absolutePath: String

    // name(like aaa.bbb) with extension
    val name: String

    // parent absolute path
    val parentPath: String

    fun delete(): Boolean
    fun exists(): Boolean
    fun toExternalResource(): ExternalResource
    fun deleteRecursively(): Boolean
    fun mkdir(): Boolean
    fun writeText(text: String)
    fun canRead(): Boolean
    fun readText(): String
    fun writeByteArray(bytes: ByteArray)
    fun readByteArray(): ByteArray
}

expect object MiraiCPFiles {
    fun create(path: String): MiraiCPFile
}