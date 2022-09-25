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

import io.ktor.utils.io.core.*
import io.ktor.utils.io.errors.*
import kotlinx.cinterop.*
import net.mamoe.mirai.utils.ExternalResource
import net.mamoe.mirai.utils.ExternalResource.Companion.toExternalResource
import platform.posix.*
import platform.windows.*

// from https://github.com/mamoe/mirai/blob/dev/mirai-core-utils/src/mingwX64Main/kotlin/MiraiFileImpl.kt
fun <R> MiraiCPFile.useStat(block: (stat) -> R): R? {
    memScoped {
        val stat = alloc<stat>()
        val ret = stat(absolutePath, stat.ptr)
        if (ret != 0) return null
        return block(stat)
    }
}

private fun getFullPathName(path: String): String = memScoped {
    ShortArray(MAX_PATH).usePinned { pin ->
        val len = GetFullPathNameW(path, MAX_PATH, pin.addressOf(0).reinterpret(), null).toInt()
        if (len != 0) {
            return pin.get().toKStringFromUtf16(len)
        } else {
            when (val errno = errno) {
                ENOTDIR -> return@memScoped path
                EACCES -> return@memScoped path // permission denied
                ENOENT -> return@memScoped path // no such file
                else -> throw IllegalArgumentException(
                    "Invalid path($errno): $path",
                    cause = PosixException.forErrno(posixFunctionName = "GetFullPathNameW()")
                )
            }
        }
    }
}

private fun ShortArray.toKStringFromUtf16(len: Int): String {
    val chars = CharArray(len)
    var index = 0
    while (index < len) {
        chars[index] = this[index].toInt().toChar()
        ++index
    }
    return chars.concatToString()
}

inline infix fun UInt.flag(flag: Int): Boolean = this and flag.toUInt() != 0u
class MiraiCPFileWinImpl(private val path: String) : MiraiCPFile {
    companion object {
        private val ROOT_REGEX = Regex("""^([a-zA-z]+:[/\\])""")
        private const val SEPARATOR = '\\'
    }

    override val isFile: Boolean
        get() = useStat { it.st_mode.convert<UInt>() flag S_IFREG } ?: false

    override fun exists(): Boolean = getFileAttributes() != INVALID_FILE_ATTRIBUTES

    private fun getFileAttributes(): DWORD = memScoped { GetFileAttributesW(absolutePath) }
    override val extension: String
        get() = path.substringAfterLast('.', "")

    override val absolutePath: String by lazy {
        val result = ROOT_REGEX.matchEntire(this.path)
            ?: return@lazy getFullPathName(this.path).removeSuffix(SEPARATOR.toString())
        return@lazy result.groups.first()!!.value
    }

    override fun toExternalResource(): ExternalResource {
        // from https://www.nequalsonelifestyle.com/2020/11/16/kotlin-native-file-io/
        val returnBuffer = StringBuilder()
        val file = fopen(absolutePath, "r") ?: throw IllegalArgumentException("Cannot open input file $absolutePath")

        try {
            memScoped {
                val readBufferLength = 64 * 1024
                val buffer = allocArray<ByteVar>(readBufferLength)
                var line = fgets(buffer, readBufferLength, file)?.toKString()
                while (line != null) {
                    returnBuffer.append(line)
                    line = fgets(buffer, readBufferLength, file)?.toKString()
                }
            }
        } finally {
            fclose(file)
        }

        return returnBuffer.toString().toByteArray().toExternalResource()
    }

    override fun delete(): Boolean {
        return if (isFile) {
            DeleteFileW(absolutePath) != 0
        } else {
            RemoveDirectoryW(absolutePath) != 0
        }
    }

    private val deleteFile =
        staticCFunction<CPointer<ByteVarOf<Byte>>?, CPointer<stat>?, Int, CPointer<FTW>?, Int> { pathPtr, _, _, _ ->
            val path = pathPtr!!.toKString()
            if (remove(path) < 0) {
                -1
            } else {
                0
            }
        }

    override fun deleteRecursively(): Boolean {
        return nftw(absolutePath, deleteFile, 10, FTW_DEPTH or FTW_MOUNT or FTW_PHYS) >= 0
    }

    override fun mkdir(): Boolean {
        memScoped {
            val v = alloc<_SECURITY_ATTRIBUTES>()
            return CreateDirectoryW(absolutePath, v.ptr) != 0
        }
    }
}

actual object MiraiCPFiles {
    actual fun create(path: String): MiraiCPFile = MiraiCPFileWinImpl(path)
}