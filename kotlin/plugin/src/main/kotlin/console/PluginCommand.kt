/*
 * Copyright (c) 2020 - 2021. Eritque arcus and contributors.
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

package tech.eritquearcus.miraicp.console

import net.mamoe.mirai.console.command.CommandManager
import net.mamoe.mirai.console.command.CommandSender
import net.mamoe.mirai.console.command.RawCommand
import net.mamoe.mirai.message.data.MessageChain
import net.mamoe.mirai.message.data.MessageChain.Companion.serializeToJsonString
import tech.eritquearcus.miraicp.PluginMain
import tech.eritquearcus.miraicp.shared.*
import kotlin.properties.Delegates

class CommandHandlerImpl : CommandHandler {
    override fun register(c: Command): String {
        val a = object : RawCommand(
            PluginMain, c.primaryName, *c.secondName.toTypedArray(),
            description = c.description?:"<no descriptions given>",
            usage = c.usage?:"<no usages given>"
        ) {
            var pluginId : Int = -1
            var bindId: Int = -1
            override suspend fun CommandSender.onCommand(args: MessageChain) {
                PublicShared.cpp[pluginId] .Event(
                    PublicShared.gson.toJson(
                        Command2C(
                            this.user?.toContact(),
                            this.bot?.id ?: 0,
                            args.serializeToJsonString(),
                            bindId
                        )
                    )
                )
            }
        }
        a::pluginId.set(c.pluginId)
        a::bindId.set(c.bindId)
        return CommandManager.registerCommand(a, c.override).toString()
    }
}