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

import io.ktor.utils.io.core.*
import io.ktor.utils.io.errors.*
import kotlinx.cinterop.*
import net.mamoe.mirai.utils.ExternalResource
import net.mamoe.mirai.utils.ExternalResource.Companion.toExternalResource
import platform.posix.*

// from https://github.com/mamoe/mirai/blob/dev/mirai-core-utils/src/unixMain/kotlin/MiraiFileImpl.kt
private fun readlink(path: String): String = memScoped {
    val len = realpath(path, null)
    if (len != null) {
        try {
            return len.toKString()
        } finally {
            free(len)
        }
    } else {
        when (val errno = errno) {
            //not a dir, permission denied, no such file
            ENOTDIR, EACCES, ENOENT -> return@memScoped path
            else -> throw IllegalArgumentException(
                "Invalid path($errno): $path", cause = PosixException.forErrno(posixFunctionName = "realpath()")
            )
        }
    }
}

inline infix fun UInt.flag(flag: Int): Boolean = this and flag.toUInt() != 0u
fun <R> MiraiCPFileUnixImpl.useStat(block: (stat) -> R): R? {
    memScoped {
        val stat = alloc<stat>()
        val ret = stat(absolutePath, stat.ptr)
        if (ret != 0) return null
        return block(stat)
    }
}

class MiraiCPFileUnixImpl(val path: String) : MiraiCPFile {
    override val isFile: Boolean
        get() = useStat { it.st_mode.convert<UInt>() flag S_IFREG } ?: false

    override fun exists(): Boolean = useStat { true } ?: false
    override val extension: String
        get() = path.substringAfterLast('.', "")
    override val absolutePath: String by lazy { kotlin.run { readlink(path) } }
    override val name: String
        get() = absolutePath.substringAfterLast('/', "").ifEmpty { absolutePath }
    override val pathWithOutName: String
        get() {
            val absolutePath = absolutePath
            val p = absolutePath.substringBeforeLast("/", "")
            if (p.isEmpty()) {
                return if (absolutePath.singleOrNull() == '/') "/" // root
                else "/"
            }
            throw IllegalStateException("File path error")
        }

    override fun delete(): Boolean {
        return if (isFile) {
            remove(absolutePath) == 0
        } else {
            rmdir(absolutePath) == 0
        }
    }

    override fun toExternalResource(): ExternalResource = readText().toByteArray().toExternalResource()

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
        @Suppress("UnnecessaryOptInAnnotation") // bug
        @OptIn(UnsafeNumber::class)
        return (mkdir("$absolutePath/", "755".toUShort(8).convert()).convert<Int>() == 0)
    }

    override fun writeText(text: String) {
        // from https://www.nequalsonelifestyle.com/2020/11/16/kotlin-native-file-io/
        val file = fopen(absolutePath, "w") ?: throw IllegalArgumentException("Cannot open output file $absolutePath")
        try {
            memScoped {
                if (fputs(text, file) == EOF) throw Error("File write error")
            }
        } finally {
            fclose(file)
        }
    }

    override fun canRead(): Boolean = exists() && useStat { it.st_mode.convert<UInt>() flag S_IRUSR } ?: false

    override fun readText(): String {
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

        return returnBuffer.toString()
    }
}

actual object MiraiCPFiles {
    actual fun create(path: String): MiraiCPFile = MiraiCPFileUnixImpl(path)
}