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

package tech.eritquearcus.miraicp.loader

import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.BotFactory
import net.mamoe.mirai.event.events.BotOnlineEvent
import net.mamoe.mirai.utils.BotConfiguration
import tech.eritquearcus.miraicp.shared.CPPConfig
import tech.eritquearcus.miraicp.shared.CPPEvent
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.event

internal fun String.decodeHex(): ByteArray {
    check(length % 2 == 0) { "Must have an even length" }

    return chunked(2)
        .map { it.toInt(16).toByte() }
        .toByteArray()
}

internal fun CPPConfig.loaderConfig.Account.login() {
    val it = this
    this.logined = true
    val p = when (it.protocol?.uppercase()) {
        "PAD" -> BotConfiguration.MiraiProtocol.ANDROID_PAD
        "WATCH" -> BotConfiguration.MiraiProtocol.ANDROID_WATCH
        "PHONE" -> BotConfiguration.MiraiProtocol.ANDROID_PHONE
        null -> BotConfiguration.MiraiProtocol.ANDROID_PHONE
        else -> {
            PublicShared.logger.warning("Warning: 登录协议无效, 应为PAD/WATCH/PHONE其中一个,使用默认的PHONE进行登录")
            BotConfiguration.MiraiProtocol.ANDROID_PHONE
        }
    }
    val h = when (it.heatBeat?.uppercase()) {
        "STAT_HB" -> BotConfiguration.HeartbeatStrategy.STAT_HB
        "REGISTER" -> BotConfiguration.HeartbeatStrategy.REGISTER
        "NONE" -> BotConfiguration.HeartbeatStrategy.NONE
        null -> BotConfiguration.HeartbeatStrategy.STAT_HB
        else -> {
            PublicShared.logger.warning("Warning: 心跳策略无效, 应为\"STAT_HB\"\\\"REGISTER\"\\\"None\"其中一个，使用默认的STAT_HB登录")
            BotConfiguration.HeartbeatStrategy.STAT_HB
        }
    }
    PublicShared.logger.info("登录bot:${it.id}")
    PublicShared.logger.info("协议:${p.name}")
    PublicShared.logger.info("心跳策略:${h.name}")
    val b = if (it.md5 == null || !it.md5!!) {
        BotFactory.newBot(it.id, it.passwords) {
            fileBasedDeviceInfo()
            this.protocol = p
            this.heartbeatStrategy = h
        }
    } else {
        BotFactory.newBot(it.id, it.passwords.decodeHex()) {
            fileBasedDeviceInfo()
            this.protocol = p
            this.heartbeatStrategy = h
        }
    }
    b.eventChannel.subscribeAlways<BotOnlineEvent> {
        PublicShared.cpp.event(
            PublicShared.gson.toJson(CPPEvent.BotOnline(this.bot.id))
        )
    }
    runBlocking {
        b.login()
    }
    PublicShared.onEnable(b.eventChannel)
}