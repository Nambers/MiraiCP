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

import com.github.ajalt.mordant.terminal.Terminal
import kotlinx.coroutines.CoroutineName
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import net.mamoe.mirai.Bot
import tech.eritquearcus.miraicp.loader.KotlinMainData
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.PublicSharedData
import kotlin.system.exitProcess


object Console {
    init {
        registerFactory()
    }

    val console = Terminal()

    fun listen() {
        KotlinMainData.coroutineScope.launch(CoroutineName("Console Command")) {
            while (isActive) {
                val re = try {
                    console.prompt(">", promptSuffix = " ")
                } catch (e: Exception) {
                    PublicSharedData.logger.info("Closing MiraiCP because exception raised...", e)
                    Bot.instances.forEach {
                        it.closeAndJoin()
                        PublicSharedData.logger.info("Bot ${it.id} closed")
                    }
                    PublicShared.onDisable()
                    exitProcess(0)
                }
                if (re != null) {
                    if (re.isEmpty() || re.isBlank()) continue
                    Command.parse(re)
                }
            }
        }
    }

}