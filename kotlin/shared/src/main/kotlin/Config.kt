/*
 * Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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

import kotlinx.serialization.Serializable


object CPPConfig {
    @Serializable
    data class cppPath(
        val path: String,
        val dependencies: List<String>? = null,
    )

    @Serializable
    data class AdvanceConfig(
//        val maxThread: Int? = null,
        val libLoaderPath: String? = null,
    )

    @Serializable
    data class PluginConfig(
        val pluginConfig: List<cppPath>,
        val advanceConfig: AdvanceConfig? = null,
    )

    @Serializable
    data class LoaderConfig(
        val accounts: List<Account>? = null,
        val cppPaths: List<cppPath>,
        val advanceConfig: AdvanceConfig? = null,
    ) {
        @Serializable
        data class Account(
            val id: Long,
            val passwords: String,
            val protocol: String? = null,
            val heatBeat: String? = null,
            val md5: Boolean? = null,
            val autoLogin: Boolean? = null,
            var logined: Boolean? = null,
        )
    }
}