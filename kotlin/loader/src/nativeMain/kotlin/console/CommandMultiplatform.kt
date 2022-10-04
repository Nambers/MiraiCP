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

@file:Suppress("PackageDirectoryMismatch")

package tech.eritquearcus.miraicp.loader.console

import kotlinx.serialization.encodeToString
import tech.eritquearcus.miraicp.loader.KotlinMain
import tech.eritquearcus.miraicp.loader.KotlinMainData
import tech.eritquearcus.miraicp.loader.console.Command.lastOneOrMoreParamOrder
import tech.eritquearcus.miraicp.loader.console.Command.printHelp
import tech.eritquearcus.miraicp.shared.CPPEvent
import tech.eritquearcus.miraicp.shared.PublicShared.json
import tech.eritquearcus.miraicp.shared.PublicSharedData
import tech.eritquearcus.miraicp.shared.UlitsMultiPlatform.event

actual object CommandMultiplatform {
    actual fun pureOrder(order: String) {
        when (order) {
            "exit" -> KotlinMain.exit()
            "help" -> printHelp()
            "status" -> {
                PublicSharedData.logger.info("Not imply")
//                val s = Duration.between(Console.start, LocalDateTime.now()).seconds
//                println(
//                    "该Loader已经持续运行 " + String.format(
//                        "%d:%02d:%02d",
//                        s / 3600,
//                        (s % 3600) / 60,
//                        (s % 60)
//                    ) + " 啦"
//                )
            }

            "accountList", "aList" -> KotlinMainData.loginAccount.let { acs ->
                acs.forEach { println(json.encodeToString(it)) }
            }

            "pluginList", "pList" -> {
                event(CPPEvent.LibLoaderEvent("PluginList"))
            }

            "disablePluginList", "dList" -> {
                event(CPPEvent.LibLoaderEvent("DisablePluginList"))
            }

            else -> lastOneOrMoreParamOrder(listOf(order))
        }
    }
}