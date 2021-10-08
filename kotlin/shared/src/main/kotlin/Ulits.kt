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

import net.mamoe.mirai.Bot
import net.mamoe.mirai.contact.*
import net.mamoe.mirai.utils.MiraiLogger
import java.io.File
import java.util.concurrent.SynchronousQueue
import java.util.concurrent.ThreadPoolExecutor
import java.util.concurrent.TimeUnit

private val queue = SynchronousQueue<Runnable>()
private val cc by lazy {
    ThreadPoolExecutor(
        0, PublicShared.maxThread,
        60L, TimeUnit.SECONDS,
        queue
    )
}

//suspend inline fun <T, R> T.runInTP(
//    crossinline block: T.() -> R,
//): R = runInterruptible(context = cc, block = { block() })

fun ArrayList<CPP_lib>.event(content: String) {
    cc.submit {
        when {
            PublicShared.disablePlugins.isNotEmpty() -> {
                this@event.filter {
                    !PublicShared.disablePlugins.contains(it.config.name)
                }.forEach {
                    it.Event(content)
                }
            }
            else -> {
                this@event.forEach { it.Event(content) }
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

internal inline fun withBot(botid: Long, Err: String = "", block: (Bot) -> String): String {
    val bot = Bot.getInstanceOrNull(botid)
    if (bot == null) {
        if (Err != "") PublicShared.logger.error(Err)
        return "EB"
    }
    return block(bot)
}

internal inline fun Config.Contact.withBot(Err: String = "", block: (Bot) -> String): String {
    val bot = Bot.getInstanceOrNull(botid)
    if (bot == null) {
        if (Err != "") PublicShared.logger.error(Err)
        return "EB"
    }
    return block(bot)
}

internal inline fun withFriend(bot: Bot, friendid: Long, Err: String = "", block: (Friend) -> String): String {
    val f = bot.getFriend(friendid)
    if (f == null) {
        if (Err != "") PublicShared.logger.error(Err)
        return "EF"
    }
    return block(f)
}

internal inline fun Config.Contact.withFriend(bot: Bot, Err: String = "", block: (Friend) -> String): String {
    val f = bot.getFriend(this.id)
    if (f == null) {
        if (Err != "") PublicShared.logger.error(Err)
        return "EF"
    }
    return block(f)
}

internal inline fun withGroup(bot: Bot, groupid: Long, Err: String = "", block: (Group) -> String): String {
    val g = bot.getGroup(groupid)
    if (g == null) {
        if (Err != "") PublicShared.logger.error(Err)
        return "EG"
    }
    return block(g)
}

internal inline fun Config.Contact.withGroup(bot: Bot, Err: String = "", block: (Group) -> String): String {
    val g = if (this.type == 2) bot.getGroup(this.id) else bot.getGroup(this.groupid)
    if (g == null) {
        if (Err != "") PublicShared.logger.error(Err)
        return "EG"
    }
    return block(g)
}

internal inline fun withMember(
    bot: Bot,
    groupid: Long,
    id: Long,
    Err1: String = "",
    Err2: String = "",
    block: (Group, Member) -> String
): String {
    val group = bot.getGroup(groupid)
    if (group == null) {
        if (Err1 != "") PublicShared.logger.error(Err1)
        return "EM"
    }
    val m = group[id]
    if (m == null) {
        if (Err2 != "") PublicShared.logger.error(Err2)
        return "EMM"
    }
    return block(group, m)
}

internal inline fun Config.Contact.withMember(
    bot: Bot,
    Err1: String = "",
    Err2: String = "",
    block: (Group, NormalMember) -> String
): String {
    val group = bot.getGroup(this.groupid)
    if (group == null) {
        if (Err1 != "") PublicShared.logger.error(Err1)
        return "EM"
    }
    val m = group[this.id]
    if (m == null) {
        if (Err2 != "") PublicShared.logger.error(Err2)
        return "EMM"
    }
    return block(group, m)
}