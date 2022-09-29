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

import tech.eritquearcus.miraicp.shared.UlitsMultiPlatform.getLibLoader
import tech.eritquearcus.miraicp.uilts.Library

actual object CPPLibMultiplatform {
    actual fun init(
        libPath: List<String>?,
        cfgPath: String?,
        callback: () -> Unit
    ) {
        if (libPath == null) {
            callback()
        } else {
            Library.load(getLibLoader(libPath))
        }
        Verify(BuiltInConstants.version, cfgPath!!)
    }

    external fun Verify(version: String, cfgPath: String): Void
    external fun Event(content: String): Void
    external fun PluginDisable(): Void
}