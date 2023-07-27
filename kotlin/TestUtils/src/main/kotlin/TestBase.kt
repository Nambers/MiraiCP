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

package tech.eritquearcus.miraicp.testUtils

import kotlinx.coroutines.delay
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.contact.MemberPermission
import net.mamoe.mirai.event.events.BotOnlineEvent
import net.mamoe.mirai.mock.MockBot
import net.mamoe.mirai.mock.MockBotFactory
import net.mamoe.mirai.mock.contact.MockFriend
import net.mamoe.mirai.mock.contact.MockGroup
import net.mamoe.mirai.mock.contact.MockNormalMember
import net.mamoe.mirai.mock.utils.mock
import net.mamoe.mirai.utils.MiraiExperimentalApi
import org.junit.jupiter.api.*
import tech.eritquearcus.miraicp.shared.CPPLib
import tech.eritquearcus.miraicp.shared.Packets
import tech.eritquearcus.miraicp.shared.Packets.Utils.toEventData
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.UlitsMultiPlatform
import java.io.File

val onCloseLock: Unit by lazy {
    println("load libLoader")
    MockBotFactory.initialize()
    println("Currently working dir:" + TestUtils.workingDir.absolutePath)
    require(TestUtils.workingDir.exists())
    val cfgPath = TestUtils.workingDir.resolve("testFileFromKt4Mock/config.json")
    cfgPath.writeText(
        """
{
  "accounts": [],
  "cppPaths": [
    {
      "path": "${
            TestUtils.workingDir.resolve("libMiraiCP_mock_test.${TestUtils.libExtension}").absolutePath.replace(
                File.separator,
                "/"
            )
        }"
    }
  ]
}
    """.trimIndent()
    )
    CPPLib.init(
        listOf(TestUtils.workingDir.absolutePath.replace(File.separator, "/")),
        cfgPath.absolutePath
    )
    runBlocking {
        delay(1000)
    }
}

// each test timeOut in 6 sec
@Timeout(6)
open class TestBase {
    companion object {
        @BeforeAll
        @JvmStatic
        @Timeout(10)
        fun loadCPPLib() {
            onCloseLock
            println("enable All plugins")
            UlitsMultiPlatform.event(Packets.Outgoing.LibLoaderEvent("EnableAllPlugins"))
        }

        @AfterAll
        @JvmStatic
        @Timeout(10)
        fun endTest() {
            runBlocking {
                delay(100)
            }
            TestUtils.logListener.complete()
            println("disable All plugins")
            UlitsMultiPlatform.event(Packets.Outgoing.LibLoaderEvent("DisableAllPlugins"));
            runBlocking {
                delay(100)
            }
        }
    }

    lateinit var bot: MockBot
    val group: MockGroup
        get() = bot.groups[111]!!.mock()

    val member: MockNormalMember
        get() = group[222L]!!.mock()

    val friend: MockFriend
        get() = bot.getFriend(333L)!!.mock()

    @OptIn(MiraiExperimentalApi::class)
    @BeforeEach
    fun prepare() {
        bot = MockBotFactory.newMockBotBuilder().create()
        runBlocking {
            bot.login()
        }
        PublicShared.onEnable(bot.eventChannel)
        bot.eventChannel.subscribeAlways<BotOnlineEvent> {
            UlitsMultiPlatform.event(toEventData())
        }
        bot.addGroup(111, "testGroup0")
        group.addMember(222, "testMember0")
        member.mockApi.permission = MemberPermission.MEMBER
        bot.addFriend(333, "testFriend0")
        TestUtils.logListener
        TestUtils.logList.clear()
        println("test start")
    }

    @AfterEach
    fun closeBot() {
        bot.close()
    }
}