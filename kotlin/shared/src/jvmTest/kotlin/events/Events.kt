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

package tech.eritquearcus.miraicp.shared.test.events

import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.event.broadcast
import net.mamoe.mirai.event.events.BotInvitedJoinGroupRequestEvent
import net.mamoe.mirai.event.events.GroupMessageEvent
import net.mamoe.mirai.event.events.GroupMessagePreSendEvent
import net.mamoe.mirai.message.data.*
import net.mamoe.mirai.utils.MiraiInternalApi
import org.junit.jupiter.api.Test
import tech.eritquearcus.miraicp.shared.test.TestBase
import tech.eritquearcus.miraicp.shared.test.TestUtils.filter
import tech.eritquearcus.miraicp.shared.test.TestUtils.listener
import tech.eritquearcus.miraicp.shared.test.TestUtils.logList
import tech.eritquearcus.miraicp.shared.test.TestUtils.noBroadcast
import tech.eritquearcus.miraicp.shared.test.TestUtils.waitUntilEnd
import kotlin.test.assertEquals


class Events : TestBase() {
    @Test
    fun groupMessage() {
        val group = bot.addGroup(211, "testGr1")
        val member = group.addMember(311, "testMem2")
        filter = {
            it is GroupMessagePreSendEvent
        }
        runBlocking {
            listener
            val msg = noBroadcast<GroupMessageEvent, MessageChain>(bot.eventChannel) {
                member.says("msg")
            }
            val mc = buildMessageChain {
                this.add(QuoteReply(buildMessageChain {
                    this.add(msg)
                }))
                this.add(PlainText("IAmPlainText"))
                val builder = Image.Builder.newBuilder("{01E9451B-70ED-EAE3-B37C-101F1EEBF5B5}.jpg")
                builder.size = 123
                builder.type = ImageType.PNG
                this.add(builder.build())
                this.add(AtAll)
                this.add(At(member))
                this.add(Face(1))
//                this.add(UnsupportedMessage.create("".encodeToByteArray()))
                //                this.add(FileMessage("",1,"",1))
                //                this.add(FlashImage(builder.build()))
                //                this.add(MusicShare(MusicKind.QQMusic, "", "", "", "", ""))
            }
            member.says(mc)
            waitUntilEnd()
            val logs = logList.filter { it.contains("after_serialization:") }
            assertEquals(1, logs.size)
            assertEquals(mc, MessageChain.deserializeFromJsonString(logs[0].substringAfter("after_serialization:")))
        }
    }

    @OptIn(MiraiInternalApi::class)
    @Test
    fun invited() {
        val group = bot.addGroup(211, "testGr1")
        val member = group.addMember(311, "testMem2")
        runBlocking {
            BotInvitedJoinGroupRequestEvent(bot, 123, member.id, group.id, "test", member.nick).broadcast()
            while (true) {
                Thread.sleep(1000)
            }
        }
    }
}