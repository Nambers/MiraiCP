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

import platform.windows.HMODULE
import tech.eritquearcus.miraicp.uilts.Library
import kotlin.native.concurrent.ThreadLocal

@ThreadLocal
actual object CPPLibMultiplatform {
    // libLoader eventHandler ptr address
    lateinit var eventPtr: (String) -> Unit
    var libPtr: HMODULE? = null
    actual fun init(
        libPath: List<String>?,
        cfgPath: String?,
        callback: () -> Unit
    ) {
        if (libPath == null) {
            callback()
        } else {
            Library.load(UlitsMultiPlatform.getLibLoader(libPath)) { verify ->
                verify(BuiltInConstants.version, cfgPath!!)
            }
        }
    }
}