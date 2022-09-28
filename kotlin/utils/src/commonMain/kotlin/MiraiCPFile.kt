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
    val isFile: Boolean
    val extension: String
    val absolutePath: String
    val name: String
    val pathWithOutName: String

    fun delete(): Boolean
    fun exists(): Boolean
    fun toExternalResource(): ExternalResource
    fun deleteRecursively(): Boolean
    fun mkdir(): Boolean
    fun writeText(text: String)
    fun canRead(): Boolean
    fun readText(): String
}

public expect object MiraiCPFiles {
    fun create(path: String): MiraiCPFile
}