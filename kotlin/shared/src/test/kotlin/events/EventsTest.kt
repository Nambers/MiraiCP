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
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.message.data.MessageSource.Key.recall
import net.mamoe.mirai.mock.MockActions
import net.mamoe.mirai.mock.utils.broadcastMockEvents
import net.mamoe.mirai.utils.MiraiExperimentalApi
import net.mamoe.mirai.utils.MiraiInternalApi
import org.junit.jupiter.api.Test
import tech.eritquearcus.miraicp.testUtils.TestBase
import tech.eritquearcus.miraicp.testUtils.TestUtils.logList
import tech.eritquearcus.miraicp.testUtils.TestUtils.waitUntilEnd
import kotlin.test.assertEquals
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
    fun memberLeaveEvent() = runBlocking {
        member.kick("xx")
        waitUntilEnd()
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
    fun memberJoinEvent() = runBlocking {
        MemberJoinEvent.Active(member).broadcast()
        waitUntilEnd()
    }

    @OptIn(MiraiInternalApi::class)
    @Test
    fun memberJoinInvitedEvent() = runBlocking {
        val invitor = group.addMember(2333, "invitedMember")
        MemberJoinEvent.Invite(member, invitor).broadcast()
        MemberJoinEvent.Active(member).broadcast()
        MemberJoinEvent.Retrieve(member).broadcast()
        waitUntilEnd(3)
        assertEquals(
            invitor.id.toString(),
            logList.first { it.contains("Invitor:") }.substringAfter("Invitor:")
        )
    }

    @OptIn(MiraiInternalApi::class)
    @Test
    fun botInvitedJoinGroupRequestEvent() = runBlocking {
        val g = bot.addGroup(112, "group2")
        val inv = g.addMember(223, "invited")
        BotInvitedJoinGroupRequestEvent(
            bot = bot,
            eventId = 2333,
            invitorId = inv.id,
            groupId = g.uin,
            groupName = g.name,
            invitorNick = inv.nick,
        ).broadcast()
        // botInvitedEvent + botJoinEvent
        waitUntilEnd(2)
        assertTrue(bot.groups.any { it.id == group.id })
    }

    @OptIn(MiraiInternalApi::class, MiraiExperimentalApi::class)
    @Test
    fun botLeaveEventTest() = runBlocking {
        BotLeaveEvent.Active(group).broadcast()
        waitUntilEnd()
    }

    @Test
    fun messagePreSendEventTest() = runBlocking {
        group.sendMessage("MsgPreSend")
        // MessagePreSend
        waitUntilEnd()
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
        waitUntilEnd(2)
    }

    @OptIn(MiraiInternalApi::class, MiraiExperimentalApi::class)
    @Test
    fun botJoinGroupEvent(): Unit = runBlocking {
        BotJoinGroupEvent.Active(group).broadcast()
        waitUntilEnd()
    }

    @Test
    fun timerTest() = runBlocking {
        member.says("Timer")
        waitUntilEnd()
    }
}