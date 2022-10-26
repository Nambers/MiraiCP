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

import kotlinx.serialization.encodeToString

actual object UlitsMultiPlatform {
    actual inline fun <reified T> event(value: T) {
        println("event:" + if (value is String) value else json.encodeToString(value))
        CPPLibMultiplatform.eventPtr(if (value is String) value else json.encodeToString(value))
    }

    @Deprecated("", level = DeprecationLevel.HIDDEN)
    actual fun getLibLoader(pathsInput: List<String>): String {
        return ""
    }
}