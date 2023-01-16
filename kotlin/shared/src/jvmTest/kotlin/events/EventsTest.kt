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
import net.mamoe.mirai.event.broadcast
import net.mamoe.mirai.event.events.BotOnlineEvent
import net.mamoe.mirai.message.data.MessageSource.Key.recall
import net.mamoe.mirai.mock.MockActions
import net.mamoe.mirai.mock.utils.broadcastMockEvents
import net.mamoe.mirai.utils.MiraiInternalApi
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
    fun nudgeEvent() = runBlocking {
        broadcastMockEvents {
            bot.nudgedBy(member)
            bot.nudgedBy(friend)
            waitUntilEnd(2)
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
        // newMemberReq + MemberJoin
        waitUntilEnd(2)
        assertTrue(group.members.any { it.id == 2333L })
    }

    @OptIn(MiraiInternalApi::class)
    @Test
    fun botOnlineEvent(): Unit = runBlocking {
        BotOnlineEvent(bot).broadcast()
        waitUntilEnd()
    }

    @Test
    fun memberRecallEvent() = runBlocking {
        val mc = member.says("x")
        mc.recall()
        // GroupMessage + Recall
        waitUntilEnd(2)
    }

    @Test
    fun friendRecallEvent() = runBlocking {
        broadcastMockEvents {
            MockActions.fireMessageRecalled(friend.says("x"))
        }
        // friendMessageEvent + friendRecallEvent
        waitUntilEnd()
    }

//    @Test
//    fun botJoinGroupEvent() = runBlocking {
//        // todo
//    }
}