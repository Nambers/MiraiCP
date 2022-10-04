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
import tech.eritquearcus.MiraiCP.Event
import tech.eritquearcus.MiraiCP.Verify

actual object CPPLibMultiplatform {
    // libLoader eventHandler ptr address
    fun eventPtr(str: String) {
        Event(str)
    }

    actual fun init(
        libPath: List<String>?,
        cfgPath: String?,
        callback: () -> Unit
    ) {
        if (libPath == null) {
            callback()
        } else {
            Verify(BuiltInConstants.version, cfgPath!!)
        }
    }
}