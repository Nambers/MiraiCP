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
import kotlinx.cinterop.*
import tech.eritquearcus.MiraiCP.Event
import tech.eritquearcus.MiraiCP.Verify
import tech.eritquearcus.miraicp.shared.CPPLib.operation
import tech.eritquearcus.miraicp.shared.CPPLib.sendLog

actual object CPPLibMultiplatform {
    // libLoader eventHandler ptr address
    fun eventPtr(str: String) {
        Event(str)
    }

    private val koper = staticCFunction<CPointer<ByteVar>?, CPointer<ByteVar>?> { input ->
        memScoped {
            operation(input?.toKStringFromUtf8() ?: "").cstr.getPointer(this)
        }
    }

    private val log = staticCFunction<CPointer<ByteVar>?, Int, Unit> { input, level ->
        memScoped {
            sendLog(input?.toKStringFromUtf8() ?: "", level)
        }
    }

    actual fun init(
        libPath: List<String>?,
        cfgPath: String?,
        callback: () -> Unit
    ) {
        if (libPath == null) {
            callback()
        } else {
            memScoped{
                Verify(BuiltInConstants.version, cfgPath!!, koper, log)
            }
        }
    }
}