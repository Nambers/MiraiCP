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

import com.github.ajalt.mordant.rendering.TextColors
import net.mamoe.mirai.utils.MiraiLogger
import net.mamoe.mirai.utils.Services
import kotlin.reflect.KClass

class MiraiCPLogger(override val identity: String?, override val isEnabled: Boolean = true) : MiraiLogger {
    enum class Level {
        INFO,
        ERR,
        WARN,
        DEBUG,
    }

    private fun color(str: String?, co: Level): String =
        when (co) {
            Level.INFO -> TextColors.green
            Level.ERR -> TextColors.red
            Level.WARN -> TextColors.yellow
            Level.DEBUG -> TextColors.white
        }(str ?: "")

    override fun debug(message: String?) {
        Console.console.println(color(message ?: "", Level.DEBUG))
    }

    override fun debug(message: String?, e: Throwable?) {
        Console.console.println(color(message + e?.getStackTrace()?.joinToString("\n"), Level.DEBUG))
    }

    override fun error(message: String?) {
        Console.console.println(color(message ?: "", Level.ERR))
    }

    override fun error(message: String?, e: Throwable?) {
        Console.console.println(color(message + e?.getStackTrace()?.joinToString("\n"), Level.ERR))
    }

    override fun info(message: String?) {
        Console.console.println(color(message ?: "", Level.INFO))
    }

    override fun info(message: String?, e: Throwable?) {
        Console.console.println(color(message + e?.getStackTrace()?.joinToString("\n"), Level.INFO))
    }

    override fun verbose(message: String?) {
        Console.console.println(color(message ?: "", Level.DEBUG))
    }

    override fun verbose(message: String?, e: Throwable?) {
        Console.console.println(color(message + e?.getStackTrace()?.joinToString("\n"), Level.DEBUG))
    }

    override fun warning(message: String?) {
        Console.console.println(color(message ?: "", Level.WARN))
    }

    override fun warning(message: String?, e: Throwable?) {
        Console.console.println(color(message + e?.getStackTrace()?.joinToString("\n"), Level.WARN))
    }
}

object MiraiLoggerFactoryImpl : MiraiLogger.Factory {
    override fun create(requester: KClass<*>): MiraiLogger {
        return MiraiCPLogger(identity = requester.simpleName ?: requester.qualifiedName)
    }

    override fun create(requester: KClass<*>, identity: String?): MiraiLogger {
        return MiraiCPLogger(identity ?: requester.simpleName ?: requester.qualifiedName)
    }
}

fun registerFactory() {
    Services.register(
        MiraiLogger.Factory::class.qualifiedName!!,
        MiraiLoggerFactoryImpl::class.qualifiedName!!
    ) {
        MiraiLoggerFactoryImpl
    }
}