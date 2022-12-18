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
import net.mamoe.mirai.event.events.GroupMessagePreSendEvent
import net.mamoe.mirai.message.data.*
import org.junit.jupiter.api.Test
import tech.eritquearcus.miraicp.shared.test.TestBase
import tech.eritquearcus.miraicp.shared.test.TestUtils.eventList
import tech.eritquearcus.miraicp.shared.test.TestUtils.filter
import tech.eritquearcus.miraicp.shared.test.TestUtils.listener
import tech.eritquearcus.miraicp.shared.test.TestUtils.waitUntilEnd


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
            member.says(buildMessageChain {
//                this.add(QuoteReply(mc))
                this.add(PlainText("IAmPlainText"))
                val builder = Image.Builder.newBuilder("{01E9451B-70ED-EAE3-B37C-101F1EEBF5B5}.jpg")
                builder.size = 123
                builder.type = ImageType.PNG
                this.add(builder.build())
                this.add(AtAll)
                this.add(At(member))
                this.add(Face(1))
                this.add(UnsupportedMessage.create("".encodeToByteArray()))
//                this.add(FileMessage("",1,"",1))
//                this.add(FlashImage(builder.build()))
//                this.add(MusicShare(MusicKind.QQMusic, "", "", "", "", ""))
            })
            waitUntilEnd()
        }
        println(eventList)
    }
}