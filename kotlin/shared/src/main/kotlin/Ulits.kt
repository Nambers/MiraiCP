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

package tech.eritquearcus.miraicp.shared

import kotlinx.coroutines.TimeoutCancellationException
import kotlinx.coroutines.withTimeout
import net.mamoe.mirai.Bot
import net.mamoe.mirai.contact.*
import net.mamoe.mirai.event.EventPriority
import net.mamoe.mirai.event.GlobalEventChannel
import net.mamoe.mirai.event.events.EventCancelledException
import net.mamoe.mirai.event.events.MessageEvent
import net.mamoe.mirai.event.nextEvent
import net.mamoe.mirai.message.data.Image
import net.mamoe.mirai.message.data.ImageType
import net.mamoe.mirai.message.data.MessageChain
import net.mamoe.mirai.message.data.MessageSource
import net.mamoe.mirai.utils.MiraiLogger
import org.json.JSONObject
import java.io.File
import java.util.concurrent.SynchronousQueue
import java.util.concurrent.ThreadPoolExecutor
import java.util.concurrent.TimeUnit

// 执行线程池, 以免阻塞mirai协程的线程池
private val queue = SynchronousQueue<Runnable>()
private val cc by lazy {
    ThreadPoolExecutor(
        0, PublicShared.maxThread, 60L, TimeUnit.SECONDS, queue
    )
}

//suspend inline fun <T, R> T.runInTP(
//    crossinline block: T.() -> R,
//): R = runInterruptible(context = cc, block = { block() })

// 广播事件到cpp
fun ArrayList<CPPLib>.event(obj: Any) {
    val content = if (obj is String) obj else PublicShared.gson.toJson(obj)
    cc.submit {
        when {
            PublicShared.disablePlugins.isNotEmpty() -> {
                this@event.filter {
                    !PublicShared.disablePlugins.contains(it.config.name)
                }.forEach {
                    it.Event(content)
                }
            }
            else -> this@event.forEach { it.Event(content) }
        }
    }
}

// load native lib from File
fun File.loadAsCPPLib(d: List<String>?, uncheck: Boolean = false): CPPLib {
    this.copyTo(File.createTempFile(this.name, ".cache", PublicShared.cachePath), true).let { tempFile ->
        return CPPLib(tempFile.absolutePath, d).apply {
            PublicShared.logger.info("加载${tempFile.absolutePath}成功")
            this.showInfo()
            if (!uncheck) {
                if (PublicShared.cpp.count { it.config.id == this.config.id } > 1) {
                    val lib = PublicShared.cpp.first { it.config.id == this.config.id }
                    PublicShared.logger.error("已加载id为${lib.config.id}的插件, 放弃加载当前插件(位于:${this@loadAsCPPLib.absolutePath})")
                    return lib
                }
                if (PublicShared.cpp.count { it.config.name == this.config.name } > 1)
                    PublicShared.logger.warning("检测到列表已经有重复的插件名称(${this.config.name}), 请检测配置文件中是否重复或提醒开发者改插件名称，但该插件还是会加载")
            }
            PublicShared.logger4plugins[this.config.id] = MiraiLogger.Factory.create(this::class, this.config.name)
        }
    }
}

fun Group.toContact(): Config.Contact = Config.Contact(2, this.id, 0, this.name, this.bot.id)

fun Member.toContact(): Config.Contact = Config.Contact(3, this.id, this.group.id, this.nameCardOrNick, this.bot.id)

fun Friend.toContact(): Config.Contact = Config.Contact(1, this.id, 0, this.nameCardOrNick, this.bot.id)

fun Contact.toContact(): Config.Contact? = when (this) {
    is Group -> this.toContact()
    is Friend -> this.toContact()
    is Member -> this.toContact()
    else -> {
        PublicShared.logger.error("MiraiCP遇到意料之中的问题, 请到github仓库发送issue和黏贴本信息以修复此问题, 位置:Contact.toContact(), info:${this.javaClass.name}")
        null
    }
}

// convert mirai Contact type to MiraiCP contact
fun ContactOrBot.toContact(): Config.Contact? = when (this) {
    is Contact -> this.toContact()
    is Bot -> this.asFriend.toContact()
    else -> {
        PublicShared.logger.error("MiraiCP遇到意料之中的问题, 请到github仓库发送issue和本信息以修复此问题, 位置:ContactOrBot.toContact(), info:${this.javaClass.name}")
        null
    }
}

internal fun emptyContact(botid: Long): Config.Contact = Config.Contact(0, 0, 0, "", botid)

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
    bot: Bot, groupid: Long, id: Long, Err1: String = "", Err2: String = "", block: (Group, Member) -> String
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
    bot: Bot, Err1: String = "", Err2: String = "", block: (Group, NormalMember) -> String
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

fun Config.Contact.withMiraiMember(block: (Bot, Group, NormalMember) -> String): String = withBot { bot ->
    return withMember(bot) { g, m -> block(bot, g, m) }
}

internal fun <T> JSONObject.getOrNull(key: String): T? = if (this.has(key)) (this.get(key) as T) else null

// MiraiCP image info to mirai image
internal fun Config.ImgInfo.toImage(): Image = Image.newBuilder(this.imageid!!).apply {
    this@apply.height = this@toImage.height
    this@apply.width = this@toImage.width
    this@apply.size = this@toImage.size
    this@apply.type = ImageType.values()[this@toImage.type ?: ImageType.UNKNOWN.ordinal]
}.build()

internal inline fun sendWithCatch(block: () -> MessageSource): String {
    val s = try {
        block()
    } catch (e: TimeoutCancellationException) {
        return "ET"
    } catch (e: BotIsBeingMutedException) {
        return "EBM${e.botMuteRemaining}"
    } catch (e: EventCancelledException) {
        return "EC"
    }
    return PublicShared.json.encodeToString(MessageSource.Serializer, s)
}

internal suspend inline fun <reified E : MessageEvent> nextMessage(
    time: Long,
    halt: Boolean,
    p: EventPriority = EventPriority.HIGHEST,
    crossinline filter: (E) -> Boolean
): MessageChain {
    val exec = suspend {
        GlobalEventChannel.nextEvent<E>(p) {
            if (filter(it)) {
                if (halt) it.intercept()
                true
            } else false
        }
    }
    return if (time != -1L) withTimeout(time) { exec() }.message
    else exec().message
}