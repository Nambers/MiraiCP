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
import net.mamoe.mirai.mock.utils.broadcastMockEvents
import org.junit.jupiter.api.Test
import tech.eritquearcus.miraicp.shared.test.TestBase
import tech.eritquearcus.miraicp.shared.test.TestUtils.waitUntilEnd
import kotlin.test.assertTrue

// ref: https://github.com/mamoe/mirai/blob/dev/mirai-core-mock/test/DslTest.kt
class EventsTest : TestBase() {
    @Test
    fun groupMessageEvent() = runBlocking {
        member.says("event")
        waitUntilEnd()
    }

    @Test
    fun privateMessageEvent() = runBlocking {
        friend.says("event")
        waitUntilEnd()
    }

//    @Test
//    fun groupTempMessageEvent() = runBlocking{
//        // todo
//    }

    @Test
    fun groupNudgeEvent() = runBlocking {
        broadcastMockEvents {
            bot.nudgedBy(member)
            waitUntilEnd()
        }
    }

    @Test
    fun friendNudgeEvent() = runBlocking {
        broadcastMockEvents {
            bot.nudgedBy(friend)
            waitUntilEnd()
        }
    }

    @Test
    fun newFriendRequestEvent() = runBlocking {
        bot.broadcastNewFriendRequestEvent(
            requester = 2333,
            requesterNick = "Karlatemp",
            fromGroup = group.id,
            message = "さくらが落ちる",
        )
        waitUntilEnd()
        assertTrue(bot.friends.any { it.id == 2333L })
    }

    @Test
    fun newMemberRequestEvent() = runBlocking {
        group.broadcastNewMemberJoinRequestEvent(
            requester = 2333,
            requesterName = "Him188moe",
            message = "Hi!",
        )
        waitUntilEnd()
        assertTrue(group.members.any { it.id == 2333L })
        // for MemberJoinEvent
        waitUntilEnd()
    }
}