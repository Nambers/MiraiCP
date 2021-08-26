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

package group

import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.Bot
import net.mamoe.mirai.event.broadcast
import net.mamoe.mirai.event.events.GroupMessageEvent
import net.mamoe.mirai.message.data.toMessageChain
import net.mamoe.mirai.utils.MiraiInternalApi
import net.mamoe.mirai.utils.MiraiLogger
import net.mamoe.mirai.utils.PlatformLogger
import org.junit.jupiter.api.Test
import tech.eritquearcus.miraicp.loader.*
import tech.eritquearcus.miraicp.shared.CPP_lib
import kotlin.test.assertEquals
import kotlin.test.assertTrue
import kotlin.time.Duration
import kotlin.time.measureTimedValue

suspend fun groupMessage() {
    val b = Bot.getInstance(692928873)
    val g = b.getGroup(788189105)!!
    val s = g[1930893235]!!
    val m = source(b, 1930893235, 788189105).toMessageChain()
    GroupMessageEvent(s.nameCard, s.permission, s, m, 1).broadcast()
}

class GroupMessageTest {
    @Test
    @OptIn(
        MiraiInternalApi::class, net.mamoe.mirai.utils.MiraiExperimentalApi::class,
        kotlin.time.ExperimentalTime::class
    )
    fun lotGroupMessageTest() {
        MiraiLogger.setDefaultLoggerCreator {
            PlatformLogger(it!!) {}
        }
        CPP_lib.test = true
        setUpStreams()
        var line: Int
        var tempTime: Duration
        var wholeTime: Duration
        runBlocking {
            main(arrayOf("../config.json"))
            tempTime = measureTimedValue {
                groupMessage()
            }.duration
            line = outContent.toString().trim().lines().count { it.startsWith("send") }
            originOut.println(outContent)
            outContent.reset()
            wholeTime = measureTimedValue {
                // 3 events/s in 10s
                launch {
                    for (i in 1..10) {
                        groupMessage()
                        delay(1000)
                    }
                }
                launch {
                    for (i in 1..10) {
                        groupMessage()
                        delay(1000)
                    }
                }
                launch {
                    for (i in 1..10) {
                        groupMessage()
                        delay(1000)
                    }
                }
            }.duration
        }
        restoreStreams()
        println(outContent.toString())
        Bot.instances.forEach {
            it.close()
        }
        assertTrue(
            wholeTime.inWholeMilliseconds <= tempTime.inWholeMilliseconds * 3 * 10 * 1.1,
            "expect timeout: ${tempTime.inWholeMilliseconds * 3 * 10 * 1.1}, real tim: $wholeTime"
        )
        assertEquals(line * 10 * 3, outContent.toString().trim().lines().count { it.startsWith("send") })
    }
}