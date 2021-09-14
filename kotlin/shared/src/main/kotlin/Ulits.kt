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

package tech.eritquearcus.miraicp.shared

import net.mamoe.mirai.contact.Friend
import net.mamoe.mirai.contact.Group
import net.mamoe.mirai.contact.Member
import net.mamoe.mirai.contact.nameCardOrNick
import net.mamoe.mirai.utils.MiraiLogger
import java.io.File
import java.util.concurrent.Executors

private val cc by lazy { Executors.newCachedThreadPool() }

//suspend inline fun <T, R> T.runInTP(
//    crossinline block: T.() -> R,
//): R = runInterruptible(context = cc, block = { block() })

fun ArrayList<CPP_lib>.Event(content: String) {
    cc.submit {
        when {
            PublicShared.disablePlugins.isNotEmpty() -> {
                this@Event.filter {
                    !PublicShared.disablePlugins.contains(it.config.name)
                }.forEach {
                    it.Event(content)
                }
            }
            else -> {
                this@Event.forEach { it.Event(content) }
            }
        }
    }
}

// load native lib
fun File.loadAsCPPLib(d: List<String>?, uncheck: Boolean = false): CPP_lib {
    this.copyTo(File.createTempFile(this.name, ".cache", PublicShared.cachePath), true).let {
        return CPP_lib(it.absolutePath, d).apply {
            PublicShared.logger.info("加载${it.absolutePath}成功")
            this.showInfo()
            if (!uncheck) PublicShared.cpp.firstOrNull { itt -> itt.config.id == this.config.id }.let check@{ c ->
                if (c == null) return@check
                PublicShared.logger.error("已加载id为${c.config.id}的插件, 放弃加载当前插件(位于:${this@loadAsCPPLib.absolutePath})")
                return c
            }
            if (PublicShared.cpp.firstOrNull { itt -> itt.config.name == this.config.name } != null)
                PublicShared.logger.warning("检测到列表已经有重复的${this.config.name}, 请检测配置文件中是否重复或提醒开发者改插件名称，但该插件还是会加载")
            PublicShared.logger4plugins[this.config.id] =
                MiraiLogger.Factory.create(this::class, this.config.name)
            PublicShared.cpp.add(this)
        }
    }
}

internal fun Group.toContact(): Config.Contact =
    Config.Contact(2, this.id, 0, this.name, this.bot.id)

internal fun Member.toContact(): Config.Contact =
    Config.Contact(3, this.id, this.group.id, this.nameCardOrNick, this.bot.id)

internal fun Friend.toContact(): Config.Contact =
    Config.Contact(1, this.id, 0, this.nameCardOrNick, this.bot.id)