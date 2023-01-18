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

package tech.eritquearcus.miraicp.shared.test.events

import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.event.events.GroupMessageEvent
import net.mamoe.mirai.message.data.*
import net.mamoe.mirai.utils.ExternalResource.Companion.toExternalResource
import org.junit.jupiter.api.Test
import tech.eritquearcus.miraicp.shared.test.TestBase
import tech.eritquearcus.miraicp.shared.test.TestUtils.checkMessageChainJsonResultFromLog
import tech.eritquearcus.miraicp.shared.test.TestUtils.noBroadcast
import tech.eritquearcus.miraicp.shared.test.TestUtils.waitUntilEnd


// all message tests under GroupMessageEvent
// ref: https://github.com/mamoe/mirai/blob/dev/mirai-core-mock/test/mock/MessagingTest.kt
class MessageTest : TestBase() {
    @Test
    fun singleMessages() = runBlocking {
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
        checkMessageChainJsonResultFromLog(mc)
    }

    @Test
    fun forwardMessageOfSingleMessages() = runBlocking {
        val mc = buildForwardMessage(group) {
            this.add(member, PlainText("IAmPlainText"))
            val builder = Image.Builder.newBuilder("{01E9451B-70ED-EAE3-B37C-101F1EEBF5B5}.jpg")
            builder.size = 123
            builder.type = ImageType.PNG
            this.add(member, builder.build())
            this.add(member, AtAll)
            this.add(member, At(member))
            this.add(member, Face(1))
        }.toMessageChain()
        member.says(mc)
        waitUntilEnd()
        checkMessageChainJsonResultFromLog(mc)
    }

    @Test
    fun forwardMessageOfForwardMessages() = runBlocking {
        val mc = buildForwardMessage(group) {
            this.add(member, PlainText("IAmPlainText"))
            val builder = Image.Builder.newBuilder("{01E9451B-70ED-EAE3-B37C-101F1EEBF5B5}.jpg")
            builder.size = 123
            builder.type = ImageType.PNG
            this.add(member, builder.build())
            this.add(member, AtAll)
            this.add(member, At(member))
            this.add(member, Face(1))
            this.add(member, buildForwardMessage(group) {
                this.add(member, PlainText("IAmPlainText"))
                val builder2 = Image.Builder.newBuilder("{01E9451B-70ED-EAE3-B37C-101F1EEBF5B5}.jpg")
                builder2.size = 123
                builder2.type = ImageType.PNG
                this.add(member, builder2.build())
                this.add(member, AtAll)
                this.add(member, At(member))
                this.add(member, Face(1))
            })
        }.toMessageChain()
        member.says(mc)
        waitUntilEnd()
        checkMessageChainJsonResultFromLog(mc)
    }

    @Test
    fun voiceMessage() = runBlocking {
        val au = this@MessageTest.javaClass.classLoader.getResourceAsStream("mic.amr")!!.use { res ->
            res.toExternalResource().use {
                bot.uploadOnlineAudio(it)
            }
        }
        member.says(au)
        waitUntilEnd()
    }
    // nudge is in Event test
}