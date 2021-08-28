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

package tech.eritquearcus.miraicp.loader.group

import group.groupMessage
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.utils.MiraiInternalApi
import net.mamoe.mirai.utils.MiraiLogger
import net.mamoe.mirai.utils.PlatformLogger
import org.junit.jupiter.api.Test
import tech.eritquearcus.miraicp.loader.main

class tempTest {
    @Test
    @OptIn(
        MiraiInternalApi::class, net.mamoe.mirai.utils.MiraiExperimentalApi::class,
        kotlin.time.ExperimentalTime::class
    )
    fun tempGroupMessageTest() {
        runBlocking {
            main(arrayOf("../config.json"))
            MiraiLogger.setDefaultLoggerCreator {
                PlatformLogger(it!!) { s ->
                    println(s)
                }
            }
            // 3 events/s in 10s
            launch {
                for (i in 1..2) {
                    launch {
                        groupMessage()
                    }
                    delay(200)
                }
            }
            launch {
                for (i in 1..2) {
                    launch {
                        groupMessage()
                    }
                }
            }
            launch {
                for (i in 1..2) {
                    launch {
                        groupMessage()
                    }
                }
            }
            launch {
                for (i in 1..2) {
                    launch {
                        groupMessage()
                    }
                }
            }
        }
    }
}