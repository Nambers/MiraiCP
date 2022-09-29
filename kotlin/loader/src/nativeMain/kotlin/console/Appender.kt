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
import net.mamoe.mirai.utils.MiraiInternalApi
import net.mamoe.mirai.utils.MiraiLogger
import net.mamoe.mirai.utils.Services
import kotlin.reflect.KClass

@MiraiInternalApi
class MiraiCPLogger(override val identity: String?, override val isEnabled: Boolean = true) : MiraiLogger {
    override fun debug(message: String?) {
        Console.console.println(TextColors.white(message ?: ""))
    }

    override fun debug(message: String?, e: Throwable?) {
        Console.console.println(TextColors.white(message ?: ""))
    }

    override fun error(message: String?) {
        Console.console.println(TextColors.red(message ?: ""))
    }

    override fun error(message: String?, e: Throwable?) {
        Console.console.println(TextColors.red(((message ?: "") + e?.getStackTrace()?.joinToString("\n"))))
    }

    override fun info(message: String?) {
        Console.console.println(TextColors.white(message ?: ""))
    }

    override fun info(message: String?, e: Throwable?) {
        Console.console.println(TextColors.white(message ?: ""))
    }

    override fun verbose(message: String?) {
        Console.console.println(TextColors.white(message ?: ""))
    }

    override fun verbose(message: String?, e: Throwable?) {
        Console.console.println(TextColors.white(message ?: ""))
    }

    override fun warning(message: String?) {
        Console.console.println(TextColors.yellow(message ?: ""))
    }

    override fun warning(message: String?, e: Throwable?) {
        Console.console.println(TextColors.yellow(message ?: ""))
    }
}

@MiraiInternalApi
class MiraiLoggerFactoryImpl : MiraiLogger.Factory {
    override fun create(requester: KClass<*>): MiraiLogger {
        return MiraiCPLogger(identity = requester.simpleName ?: requester.qualifiedName)
    }

    override fun create(requester: KClass<*>, identity: String?): MiraiLogger {
        return MiraiCPLogger(identity ?: requester.simpleName ?: requester.qualifiedName)
    }
}

@OptIn(MiraiInternalApi::class)
fun registerFactory() {
    Services.register(
        "net.mamoe.mirai.utils.MiraiLogger.Factory",
        "tech.eritquearcus.miraicp.loader.console.MiraiLoggerFactoryImpl"
    ) { MiraiLoggerFactoryImpl() }
}