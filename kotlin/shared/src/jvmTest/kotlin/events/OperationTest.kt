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
import net.mamoe.mirai.contact.MemberPermission
import org.junit.jupiter.api.Assertions.assertFalse
import org.junit.jupiter.api.Test
import tech.eritquearcus.miraicp.shared.test.TestBase
import tech.eritquearcus.miraicp.shared.test.TestUtils.waitUntilEnd
import kotlin.test.assertEquals
import kotlin.test.assertTrue

class OperationTest : TestBase() {
    @Test
    fun sendMessageTestGroup() = runBlocking {
        member.says("message")
        waitUntilEnd()
    }

    @Test
    fun sendMessageTestFriend() = runBlocking {
        friend.says("message")
        waitUntilEnd()
    }

    @Test
    fun memberNudgeTest() = runBlocking {
        member.says("nudge")
        waitUntilEnd()
    }

    @Test
    fun friendNudgeTest() = runBlocking {
        friend.says("nudge")
        waitUntilEnd()
    }

    @Test
    fun memberMuteTest() = runBlocking {
        group.botAsMember.mockApi.permission = MemberPermission.ADMINISTRATOR
        member.says("mute")
        waitUntilEnd()
        assertTrue(member.isMuted)
    }

    @Test
    fun memberUpgrade() = runBlocking {
        group.botAsMember.mockApi.permission = MemberPermission.OWNER
        member.says("upgrade")
        waitUntilEnd()
        assertEquals(MemberPermission.ADMINISTRATOR, member.permission)
    }

    @Test
    fun memberKick() = runBlocking {
        group.botAsMember.mockApi.permission = MemberPermission.ADMINISTRATOR
        member.says("kick")
        waitUntilEnd()
        assertFalse(group.members.any { it.id == member.id })
        // for MemberLeaveEvent
        waitUntilEnd()
    }

//    @Test
//    fun botOffLineEvent() = runBlocking {
//        bot.broadcastOfflineEvent()
//        waitUntilEnd()
//    }

    @Test
    fun friendDeleteTest(): Unit = runBlocking {
        friend.says("delete")
        waitUntilEnd()
        assertFalse(bot.friends.any { it.id == friend.id })
    }
}