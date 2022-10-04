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

import tech.eritquearcus.miraicp.shared.CPPLib.operation
import tech.eritquearcus.miraicp.shared.CPPLib.sendLog
import tech.eritquearcus.miraicp.shared.UlitsMultiPlatform.getLibLoader

actual object CPPLibMultiplatform {
    actual fun init(
        libPath: List<String>?,
        cfgPath: String?,
        callback: () -> Unit
    ) {
        if (libPath == null) {
            callback()
        } else {
            System.load(getLibLoader(libPath))
        }
        Verify(BuiltInConstants.version, cfgPath!!)
    }

    @Suppress("unused")
    @JvmStatic
    fun KSendLog(log: String, level: Int) = sendLog(log, level)

    @Suppress("unused")
    @JvmStatic
    fun KOperation(content: String): String = operation(content)

    @JvmStatic
    @JvmName("Verify")
    external fun Verify(version: String, cfgPath: String): Void

    @JvmStatic
    @JvmName("Event")
    external fun Event(content: String): Void

    @JvmStatic
    @JvmName("PluginDisable")
    external fun PluginDisable(): Void
}