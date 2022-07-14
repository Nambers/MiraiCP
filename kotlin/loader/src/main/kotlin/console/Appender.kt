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

package tech.eritquearcus.miraicp.loader.console

import org.apache.logging.log4j.core.AbstractLifeCycle
import org.apache.logging.log4j.core.Filter
import org.apache.logging.log4j.core.Layout
import org.apache.logging.log4j.core.LogEvent
import org.apache.logging.log4j.core.appender.AbstractAppender
import org.apache.logging.log4j.core.config.plugins.Plugin
import org.apache.logging.log4j.core.config.plugins.PluginAttribute
import org.apache.logging.log4j.core.config.plugins.PluginElement
import org.apache.logging.log4j.core.config.plugins.PluginFactory
import org.fusesource.jansi.Ansi
import java.io.Serializable

@Plugin(name = "Jline3Appender", category = "Core", elementType = "appender", printObject = true)
class Jline3AppenderImpl private constructor(
    name: String, filter: Filter?, layout: Layout<Serializable>, ignoreExceptions: Boolean
) : AbstractAppender(name, filter, layout, ignoreExceptions, null) {

    companion object {
        var handle: (String) -> Unit =  {
            Console.lineReader.printAbove(it)
        }

        @PluginFactory
        @JvmStatic
        fun createAppender(
            @PluginAttribute("name") name: String?,
            @PluginElement("Layout") layout: Layout<Serializable>,
            @PluginElement("Filter") filter: Filter?
        ): Jline3AppenderImpl? {
            if (name == null) {
                AbstractLifeCycle.LOGGER.error("No name provided for Loader logger AppenderImpl")
                return null
            }
            return Jline3AppenderImpl(name, filter, layout, true)
        }
    }

    override fun append(event: LogEvent) {
        handle(String(layout.toByteArray(event)) + Ansi().reset().toString())
    }
}