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

package tech.eritquearcus.miraicp.shared

import kotlinx.coroutines.TimeoutCancellationException
import kotlinx.coroutines.withTimeout
import kotlinx.serialization.DeserializationStrategy
import kotlinx.serialization.serializer
import net.mamoe.mirai.Bot
import net.mamoe.mirai.contact.*
import net.mamoe.mirai.event.EventPriority
import net.mamoe.mirai.event.GlobalEventChannel
import net.mamoe.mirai.event.events.EventCancelledException
import net.mamoe.mirai.event.events.MessageEvent
import net.mamoe.mirai.event.nextEvent
import net.mamoe.mirai.message.MessageSerializers
import net.mamoe.mirai.message.data.Image
import net.mamoe.mirai.message.data.ImageType
import net.mamoe.mirai.message.data.MessageChain
import net.mamoe.mirai.message.data.MessageSource

//suspend inline fun <T, R> T.runInTP(
//    crossinline block: T.() -> R,
//): R = runInterruptible(context = cc, block = { block() })
expect object UlitsMultiPlatform {
    // 广播事件到cpp
    actual inline fun <reified T> event(value: T)
    fun getLibLoader(pathsInput: List<String>): String
}

fun Contact.toContact(): Packets.Contact? = when (this) {
    is Group -> this.toContact()
    is Friend -> this.toContact()
    is Member -> this.toContact()
    else -> {
        PublicSharedData.logger.error("MiraiCP遇到意料之中的问题, 请到github仓库发送issue和黏贴本信息以修复此问题, 位置:Contact.toContact(), info:${this}")
        null
    }
}

// convert mirai Contact type to MiraiCP contact
fun ContactOrBot.toContact(): Packets.Contact? = when (this) {
    is Contact -> this.toContact()
    is Bot -> this.asFriend.toContact()
    else -> {
        PublicSharedData.logger.error("MiraiCP遇到意料之中的问题, 请到github仓库发送issue和本信息以修复此问题, 位置:ContactOrBot.toContact(), info:$this")
        null
    }
}

fun Group.toContact(): Packets.Contact = Packets.Contact(2, this.id, this.bot.id)

fun Member.toContact(): Packets.Contact = Packets.Contact(3, this.id, this.bot.id, this.group.id)

fun Friend.toContact(): Packets.Contact = Packets.Contact(1, this.id, this.bot.id)

internal inline fun <T> withData(source: String, strategy: DeserializationStrategy<T>, block: (T) -> String): String =
    block(json.decodeFromString(strategy, source))

internal inline fun withBot(botid: Long, Err: String = "", block: (Bot) -> String): String {
    val bot = Bot.getInstanceOrNull(botid)
    if (bot == null) {
        if (Err != "") PublicSharedData.logger.error(Err)
        return "EB"
    }
    return block(bot)
}

internal inline fun Packets.Contact.withBot(err: String = "", block: (Bot) -> String): String {
    val bot = Bot.getInstanceOrNull(this.botId)
    if (bot == null) {
        if (err != "") PublicSharedData.logger.error(err)
        return "EB"
    }
    return block(bot)
}

internal inline fun withFriend(bot: Bot, friendid: Long, Err: String = "", block: (Friend) -> String): String {
    val f = bot.getFriend(friendid)
    if (f == null) {
        if (Err != "") PublicSharedData.logger.error(Err)
        return "EF"
    }
    return block(f)
}

internal inline fun Packets.Contact.withFriend(bot: Bot, Err: String = "", block: (Friend) -> String): String {
    require(this.id != 0L) { "id required to construct a friend, please contact MiraiCP" }
    val f = bot.getFriend(this.id)
    if (f == null) {
        if (Err != "") PublicSharedData.logger.error(Err)
        return "EF"
    }
    return block(f)
}

internal inline fun withGroup(bot: Bot, groupid: Long, Err: String = "", block: (Group) -> String): String {
    val g = bot.getGroup(groupid)
    if (g == null) {
        if (Err != "") PublicSharedData.logger.error(Err)
        return "EG"
    }
    return block(g)
}

internal inline fun Packets.Contact.withGroup(bot: Bot, Err: String = "", block: (Group) -> String): String {
    require(this.id != 0L) { "id is required to construct a group, please contact MiraiCP" }
    require(this.groupId != 0L) { "groupId id is required to construct a group, please contact MiraiCP" }
    val g = if (this.type == 2) bot.getGroup(this.id) else bot.getGroup(this.groupId)
    if (g == null) {
        if (Err != "") PublicSharedData.logger.error(Err)
        return "EG"
    }
    return block(g)
}

internal inline fun withMember(
    bot: Bot, groupid: Long, id: Long, Err1: String = "", Err2: String = "", block: (Group, Member) -> String
): String {
    val group = bot.getGroup(groupid)
    if (group == null) {
        if (Err1 != "") PublicSharedData.logger.error(Err1)
        return "EM"
    }
    val m = group[id]
    if (m == null) {
        if (Err2 != "") PublicSharedData.logger.error(Err2)
        return "EMM"
    }
    return block(group, m)
}

internal inline fun Packets.Contact.withMember(
    bot: Bot, Err1: String = "", Err2: String = "", block: (Group, NormalMember) -> String
): String {
    require(this.groupId != 0L) { "groupid is required to construct member, plz contact MiraiCP" }
    require(this.id != 0L) { "id is required to construct a member, plz contact MiraiCP" }
    val group = bot.getGroup(this.groupId)
    if (group == null) {
        if (Err1 != "") PublicSharedData.logger.error(Err1)
        return "EM"
    }
    for (a in PublicShared.friend_cache) {
        if (a.id == this.id && a.group.id == this.groupId) {
            return block(group, a)
        }
    }
    val m = group[this.id]
    if (m == null) {
        if (Err2 != "") PublicSharedData.logger.error(Err2)
        return "EMM"
    }
    return block(group, m)
}

internal inline fun Packets.Contact.withContact(
    bot: Bot,
    errPos: String = "Packets.Contact.withContact",
    block: (Contact) -> String
): String =
    when (type) {
        1 -> withFriend(
            bot,
            "找不到对应好友，位置${errPos}，id:${id}"
        ) { block(it) }

        2 -> withGroup(
            bot,
            "找不到对应群组，位置${errPos}，id:${id}"
        ) { block(it) }

        3 -> withMember(
            bot,
            "找不到对应群组，位置${errPos}，id:${id}",
            "找不到对应群成员，位置${errPos}，id:${id}, gid:${groupId}"
        ) { _, member ->
            block(member)
        }

        else -> {
            PublicSharedData.logger.error("类型出错, 位置:${errPos}, contact:${this}")
            "EA"
        }
    }

fun Packets.Contact.withMiraiMember(block: (Bot, Group, NormalMember) -> String): String = withBot { bot ->
    return withMember(bot) { g, m -> block(bot, g, m) }
}

// MiraiCP image info to mirai image
internal fun Config.ImgInfo.toImage(): Image = Image.newBuilder(this.imageId!!).apply {
    this@apply.height = this@toImage.height ?: 0
    this@apply.width = this@toImage.width ?: 0
    this@apply.size = this@toImage.size
    this@apply.type = ImageType.valueOf(this@toImage.imageType ?: "UNKNOWN")
    this@apply.isEmoji = this@toImage.isEmoji ?: false
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
    return json.encodeToString(MessageSerializers.serializersModule.serializer(), s)
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