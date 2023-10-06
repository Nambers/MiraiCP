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

package tech.eritquearcus.miraicp.loader.console

import kotlinx.serialization.Serializable
import tech.eritquearcus.miraicp.shared.CommandHandler

@Serializable
data class CommandBrief(
    val name: String,
    val pid: String,
    // val bid: Int,
    val sName: List<String>
)

class LoaderCommandHandlerImpl : CommandHandler {
    override fun register(c: tech.eritquearcus.miraicp.shared.Command): String {
        val cb = CommandBrief(c.primaryName, c.pluginId, c.secondName)
        when (c.override) {
            true -> Command.preCommand.add(cb)
            false -> Command.lastCommand.add(cb)
        }
        Command.message.add(c.primaryName to (c.usage ?: (c.description ?: "null")))
        return "true"
    }
}