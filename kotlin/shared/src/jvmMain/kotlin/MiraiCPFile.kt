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

import net.mamoe.mirai.utils.ExternalResource
import net.mamoe.mirai.utils.ExternalResource.Companion.toExternalResource
import java.io.File

fun File.toMiraiCPFile() = MiraiCPFiles.create(this)
class MiraiCPFileImpl(val file: File) : MiraiCPFile {
    override val isFile: Boolean
        get() = file.isFile
    override val extension: String
        get() = file.extension
    override val absolutePath: String
        get() = file.absolutePath

    override fun deleteRecursively(): Boolean = file.deleteRecursively()


    override fun mkdir(): Boolean = file.mkdir()

    override fun exists(): Boolean = file.exists()

    override fun toExternalResource(): ExternalResource = file.toExternalResource()
}

actual object MiraiCPFiles {
    actual fun create(path: String): MiraiCPFile = MiraiCPFileImpl(File(path))
    fun create(f: File): MiraiCPFile = MiraiCPFileImpl(f)
}