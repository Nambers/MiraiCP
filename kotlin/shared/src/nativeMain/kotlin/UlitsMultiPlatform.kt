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

import kotlinx.serialization.SerializationStrategy
import kotlinx.serialization.json.Json
import tech.eritquearcus.miraicp.uilts.MiraiCPFiles

actual object UlitsMultiPlatform {
    actual fun <T> event(value: T, obj: SerializationStrategy<T>?) {
        CPPLibMultiplatform.eventPtr(
            if (value is String) value else Json.encodeToString(
                obj!!,
                value
            )
        )
    }

    actual fun getLibLoader(pathsInput: List<String>): String {
        val paths = pathsInput
            .plus(MiraiCPFiles.create("./libLoader.so").absolutePath)
            .plus(MiraiCPFiles.create("./libLoader.dll").absolutePath)
        paths.forEach { path ->
            MiraiCPFiles.create(path).let {
                if (it.exists()) return it.absolutePath
            }
        }
        PublicSharedData.logger.error(
            "找不到 libLoader 组件于下列位置:\n" +
                    paths.joinToString(prefix = "\t", postfix = "\n") +
                    "请到 MiraiCP release 下载 libLoader 组件并放在以上目录其中一个的位置"
        )
        throw IllegalStateException("找不到 libLoader 组件")
    }
}