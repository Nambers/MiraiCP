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

package tech.eritquearcus.miraicp.loader

import net.mamoe.mirai.Bot
import net.mamoe.mirai.Mirai
import net.mamoe.mirai.event.broadcast
import net.mamoe.mirai.event.events.GroupMessageEvent
import net.mamoe.mirai.message.data.MessageSourceKind
import net.mamoe.mirai.message.data.PlainText
import net.mamoe.mirai.message.data.toMessageChain

suspend fun groupMessage() {
    val b = Bot.getInstance(692928873)
    val g = b.getGroup(788189105)!!
    val s = g[1930893235]!!
    val m = Mirai.constructMessageSource(
        b.id,
        MessageSourceKind.GROUP,
        s.id,
        g.id,
        intArrayOf(1),
        1,
        intArrayOf(1),
        PlainText("x").toMessageChain()
    ).toMessageChain()
    GroupMessageEvent(s.nameCard, s.permission, s, m, 1).broadcast()
}