/*
 * Copyright (C) 2020-2021 Eritque arcus and contributors.
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
 */

package tech.eritquearcus.miraicp.console

import net.mamoe.mirai.console.command.CommandManager
import net.mamoe.mirai.console.command.CommandSender
import net.mamoe.mirai.console.command.SimpleCommand
import tech.eritquearcus.miraicp.PluginMain
import tech.eritquearcus.miraicp.shared.PublicShared

object pluginList : SimpleCommand(
    PluginMain,
    "pluginList", "plist",
    description = "Tell somebody privately"
) {
    @Handler
    fun CommandSender.handle() {
        PublicShared.cpp.forEach {
            it.showInfo()
        }
    }
}

fun registerCommands() {
    CommandManager.registerCommand(pluginList)
}
