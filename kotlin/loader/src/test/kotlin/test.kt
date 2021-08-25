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

package tech.eritquearcus.miraicp.loader

import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.Bot
import net.mamoe.mirai.utils.MiraiInternalApi
import org.junit.jupiter.api.Test
import tech.eritquearcus.miraicp.shared.CPP_lib
import java.io.ByteArrayOutputStream
import java.io.PrintStream
import kotlin.test.assertEquals

internal val outContent = ByteArrayOutputStream()
internal val errContent = ByteArrayOutputStream()
private val originOut = System.out
private val originErr = System.err

internal fun setUpStreams() {
    System.setOut(PrintStream(outContent))
    System.setErr(PrintStream(errContent))
}

internal fun restoreStreams() {
    System.setOut(originOut)
    System.setOut(originErr)
}

class Test {
    @Test
    @OptIn(MiraiInternalApi::class, net.mamoe.mirai.utils.MiraiExperimentalApi::class)
    fun lotGroupMessageTest() {
        CPP_lib.test = true
        setUpStreams()
        runBlocking {
            main(arrayOf("../config.json"))
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
        }
        restoreStreams()
        println(outContent.toString())
        Bot.instances.forEach {
            it.close()
        }
        assertEquals(11 * 10 * 3 + 2, outContent.toString().trim().lines().count())
    }
}