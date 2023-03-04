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

import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.toList
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.contact.MemberPermission
import net.mamoe.mirai.contact.announcement.Announcement.Companion.publishAnnouncement
import net.mamoe.mirai.data.GroupHonorType
import net.mamoe.mirai.message.data.buildMessageChain
import net.mamoe.mirai.message.data.ids
import net.mamoe.mirai.utils.ExternalResource.Companion.toExternalResource
import net.mamoe.mirai.utils.md5
import org.junit.jupiter.api.Assertions.assertFalse
import org.junit.jupiter.api.Test
import tech.eritquearcus.miraicp.testUtils.TestBase
import tech.eritquearcus.miraicp.testUtils.TestUtils
import tech.eritquearcus.miraicp.testUtils.TestUtils.waitUntilEnd
import kotlin.test.assertContentEquals
import kotlin.test.assertEquals
import kotlin.test.assertTrue

class OperationTest : TestBase() {
    @Test
    fun queryHonorMember() = runBlocking {
        group.honorMembers[GroupHonorType.TALKATIVE] = member
        member.says("honorMember")
        waitUntilEnd()
        val logs = TestUtils.logList.filter { it.contains("honorMember:") }.map { it.substringAfter("honorMember:") }
        assertContentEquals(listOf(member.id.toString()), logs)
    }

    @Test
    fun recallTestGroup() = runBlocking {
        val mc = member.says("recall")
        // recallEvent + recallTest
        waitUntilEnd(2)
        assertEquals(null, bot.msgDatabase.queryMessageInfo(mc.ids[0].toLong()))
    }

    @Test
    fun uploadImage() = runBlocking {
        member.says("image")
        waitUntilEnd()
    }

    @Test
    fun queryBotList() = runBlocking {
        member.says("botList")
        waitUntilEnd()
        val logs = TestUtils.logList.filter { it.contains("botList:") }.map { it.substringAfter("botList:") }
        assertContentEquals(listOf(friend.id, group.id, member.id, bot.id), logs.map { it.toLong() })
    }

    @Test
    fun sendFileTest() = runBlocking {
        member.says("sendFile")
        waitUntilEnd()
        val files = group.files.root.resolveFiles("/img/img.png").toList()
        assertEquals(1, files.size)
        assertContentEquals(
            this@OperationTest.javaClass.classLoader.getResourceAsStream("img.png")!!.use { it.md5() },
            files[0].md5
        )
    }

    @Test
    fun remoteFileInfo() = runBlocking {
        val file = this@OperationTest.javaClass.classLoader.getResourceAsStream("mic.amr")!!.use { res ->
            res.toExternalResource().use {
                group.files.root.uploadNewFile(
                    "/mic.amr",
                    it
                )
            }
        }
        member.says("remoteFileInfo" + file.id)
        // remoteFileInfo
        waitUntilEnd()
        val logs = TestUtils.logList.filter {
            it.contains("reId")
        }.map {
            it.substringAfter("reId")
        }
        assertContentEquals(listOf(file.id, file.id), logs)
    }

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
    fun sendMessageTestTemp() = runBlocking {
        member.says("sendTemp")
        waitUntilEnd()
    }

    @Test
    fun memberNudgeTest() = runBlocking {
        member.says("nudge")
        // groupMessageEvent + nudgeEvent
        waitUntilEnd(2)
    }

    @Test
    fun friendNudgeTest() = runBlocking {
        friend.says("nudge")
        // groupMessageEvent + nudgeEvent
        waitUntilEnd(2)
    }

    @Test
    fun memberMuteTest() = runBlocking {
        member.says("mute")
        waitUntilEnd()
        assertTrue(member.isMuted)
    }

    @Test
    fun memberUpgrade() = runBlocking {
        member.says("upgrade")
        waitUntilEnd()
        assertEquals(MemberPermission.ADMINISTRATOR, member.permission)
    }

    @Test
    fun getOwner() = runBlocking {
        member.says("getOwner")
        waitUntilEnd()
        val logs = TestUtils.logList.filter {
            it.contains("reId")
        }.map {
            it.substringAfter("reId")
        }
        assertContentEquals(listOf(group.owner.id.toString()), logs)
    }

    @Test
    fun uploadVoice() = runBlocking {
        member.says("voice")
        waitUntilEnd()
    }


    @Test
    fun memberKick() = runBlocking {
        member.says("kick")
        // groupMessageEvent + memberLeaveEvent
        waitUntilEnd(2)
        assertFalse(group.members.any { it.id == member.id })

    }

    @Test
    fun groupSetting() = runBlocking {
        group.settings.isMuteAll = false
        group.settings.isAllowMemberInvite = false
        member.says("groupSetting")
        waitUntilEnd()
        assertEquals("test", group.name)
        assertEquals(true, group.settings.isMuteAll)
        assertEquals(true, group.settings.isAllowMemberInvite)
    }

    @Test
    fun buildForward() = runBlocking {
        member.says("buildForward")
        waitUntilEnd()
    }

    @Test
    fun sendWithQuote() = runBlocking {
        member.says("sendWithQuote")
        waitUntilEnd()
    }

    @Test
    fun announcement() = runBlocking {
        val announcement = group.publishAnnouncement("test1")
        member.says("announcement")
        waitUntilEnd()
        assertFalse(group.announcements.toList().any {
            it.fid == announcement.fid
        })
        assertEquals(1, group.announcements.toList().size)
        val pinnedAnnouncements = group.announcements.toList().filter { it.parameters.isPinned }
        assertEquals(1, pinnedAnnouncements.size)
        assertEquals("test", pinnedAnnouncements[0].content)
    }

    @Test
    fun nextMsg() = runBlocking {
        member.says("nextMsg")
        delay(100)
        member.says("nextMsg1")
        waitUntilEnd()
    }

    @Test
    fun imgUploaded() = runBlocking {
        member.says(buildMessageChain {
            add("imgUploaded")
        })
        waitUntilEnd()
    }

    @Test
    fun changeNameCard() = runBlocking {
        member.says("changeNameCard")
        waitUntilEnd()
        assertEquals("test", member.nameCard)
    }

    @Test
    fun friendDeleteTest(): Unit = runBlocking {
        friend.says("delete")
        waitUntilEnd()
        assertFalse(bot.friends.any { it.id == friend.id })
    }

    @Test
    fun memberRefreshInfoTest() = runBlocking {
        member.mockApi.specialTitle = "IAmSpecialTitle群头衔"
        member.says("refresh")
        waitUntilEnd()
    }

    @Test
    fun memberChangeSpecialTitleTest() = runBlocking {
        member.mockApi.specialTitle = "IAmSpecialTitle群头衔"
        member.says("specialTitle")
        waitUntilEnd()
        assertEquals("test", member.specialTitle)
    }

    @Test
    fun groupFileListRootEmpty() = runBlocking {
        member.says("groupFileListRoot")
        waitUntilEnd()
        val logs = TestUtils.logList.filter {
            it.contains("fileList:")
        }.map {
            it.substringAfter("fileList:")
        }
        assertContentEquals(listOf("[]"), logs)
    }

    @Test
    fun groupFileListRoot() = runBlocking {
        val file = this@OperationTest.javaClass.classLoader.getResourceAsStream("mic.amr")!!.use { res ->
            res.toExternalResource().use {
                group.files.root.uploadNewFile(
                    "/mic.amr",
                    it
                )
            }
        }
        member.says("groupFileListRoot")
        waitUntilEnd()
        val logs = TestUtils.logList.filter {
            it.contains("fileList:")
        }.map {
            it.substringAfter("fileList:")
        }
        assertContentEquals(listOf("[[\"/mic.amr\", \"${file.id}\"]]"), logs)
    }

    @Test
    fun groupFileListFolder() = runBlocking {
        val file = this@OperationTest.javaClass.classLoader.getResourceAsStream("mic.amr")!!.use { res ->
            res.toExternalResource().use {
                listOf(
                    group.files.root.uploadNewFile(
                        "/a/mic.amr",
                        it
                    ),
                    group.files.root.uploadNewFile(
                        "/a/micb.amr",
                        it
                    )
                )
            }
        }
        member.says("groupFileListFolder")
        waitUntilEnd()
        val logs = TestUtils.logList.filter {
            it.contains("fileList:")
        }.map {
            it.substringAfter("fileList:")
        }
        assertContentEquals(
            listOf("[[\"/a/mic.amr\", \"${file[0].id}\"], [\"/a/micb.amr\", \"${file[1].id}\"]]"),
            logs
        )
    }
}