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

package tech.eritquearcus.miraicp.loader

import kotlinx.coroutines.runBlocking
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import net.mamoe.mirai.BotFactory
import net.mamoe.mirai.contact.nameCardOrNick
import net.mamoe.mirai.event.EventPriority
import net.mamoe.mirai.event.events.BotOnlineEvent
import net.mamoe.mirai.event.events.FriendMessageEvent
import net.mamoe.mirai.event.events.MessageEvent
import net.mamoe.mirai.network.LoginFailedException
import net.mamoe.mirai.utils.BotConfiguration
import net.mamoe.mirai.utils.MiraiExperimentalApi
import tech.eritquearcus.miraicp.shared.CPPConfig
import tech.eritquearcus.miraicp.shared.CPPEvent
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.PublicSharedData
import tech.eritquearcus.miraicp.shared.UlitsMultiPlatform.event

val json = Json {
    ignoreUnknownKeys = true

}
internal fun String.decodeHex(): ByteArray {
    check(length % 2 == 0) { "Must have an even length" }
    return chunked(2).map { it.toInt(16).toByte() }.toByteArray()
}

/**
 * @throws LoginFailedException 正常登录失败时抛出
 */
@MiraiExperimentalApi
fun CPPConfig.LoaderConfig.Account.login() {
    this.logined = true
    val p = when (this.protocol?.uppercase()) {
        "PAD" -> BotConfiguration.MiraiProtocol.ANDROID_PAD
        "IPAD" -> BotConfiguration.MiraiProtocol.IPAD
        "MACOS" -> BotConfiguration.MiraiProtocol.MACOS
        "WATCH" -> BotConfiguration.MiraiProtocol.ANDROID_WATCH
        "PHONE" -> BotConfiguration.MiraiProtocol.ANDROID_PHONE
        null -> BotConfiguration.MiraiProtocol.ANDROID_PHONE
        else -> {
            PublicSharedData.logger.warning("Warning: 登录协议无效, 应为PAD/WATCH/PHONE/IPAD/MACOS其中一个,使用默认的PHONE进行登录")
            BotConfiguration.MiraiProtocol.ANDROID_PHONE
        }
    }
    val h = when (this.heatBeat?.uppercase()) {
        "STAT_HB" -> BotConfiguration.HeartbeatStrategy.STAT_HB
        "REGISTER" -> BotConfiguration.HeartbeatStrategy.REGISTER
        "NONE" -> BotConfiguration.HeartbeatStrategy.NONE
        null -> BotConfiguration.HeartbeatStrategy.STAT_HB
        else -> {
            PublicSharedData.logger.warning("Warning: 心跳策略无效, 应为STAT_HB/REGISTER/None其中一个，使用默认的STAT_HB登录")
            BotConfiguration.HeartbeatStrategy.STAT_HB
        }
    }
    PublicSharedData.logger.info("登录bot:${this.id}")
    PublicSharedData.logger.info("协议:${p.name}")
    PublicSharedData.logger.info("心跳策略:${h.name}")
    PublicSharedData.logger.info("a")
    val b = if (this.md5 == null || !this.md5!!) {
        BotFactory.newBot(this.id, this.passwords) {
            fileBasedDeviceInfo()
            this.protocol = p
            this.heartbeatStrategy = h
        }
    } else {
        BotFactory.newBot(this.id, this.passwords.decodeHex()) {
            fileBasedDeviceInfo()
            this.protocol = p
            this.heartbeatStrategy = h
        }
    }
    PublicSharedData.logger.info("a")
    b.eventChannel.subscribeAlways<BotOnlineEvent> {
        event(
            Json.encodeToString(CPPEvent.BotOnline(this.bot.id))
        )
    }
    PublicSharedData.logger.info("a")
    runBlocking {
        b.login()
    }
    PublicSharedData.logger.info("a")
    b.eventChannel.subscribeAlways<MessageEvent>(priority = EventPriority.HIGH) {
        if (this is FriendMessageEvent)// friend
            PublicSharedData.logger.info("${this.sender.nameCardOrNick}(${this.sender.id}) -> ${this.message.contentToString()}")
        else PublicSharedData.logger.info("[${this.bot.getGroup(this.subject.id)!!.name}(${this.subject.id})] ${this.sender.nameCardOrNick}(${this.sender.id}) -> ${this.message.contentToString()}")
    }
    PublicSharedData.logger.info("a")
    PublicShared.onEnable(b.eventChannel)
}