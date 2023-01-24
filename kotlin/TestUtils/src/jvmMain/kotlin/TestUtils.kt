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
import net.mamoe.mirai.event.*
import net.mamoe.mirai.message.data.MessageChain
import net.mamoe.mirai.message.data.MessageChain.Companion.serializeToJsonString
import net.mamoe.mirai.utils.MiraiInternalApi
import net.mamoe.mirai.utils.PlatformLogger
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.PublicSharedData
import java.io.File
import kotlin.test.assertEquals

object TestUtils {
    val workingDir by lazy{
        File(System.getenv("libpath") ?: "../../cpp/cmake-build-debug")
    }
    val libExtension by lazy{
        if (System.getProperty("os.name").lowercase().contains("win")) {
            "dll"
        } else {
            "so"
        }
    }
    var filter: (Event) -> Boolean = { true }
    val eventList = mutableListOf<Event>()
    val logList = mutableListOf<String>()
    private fun collectLog(log: String) {
        println(log)
        logList.add(log)
    }

    @OptIn(MiraiInternalApi::class)
    val logListener by lazy {
        PublicSharedData._logger = PlatformLogger("TestLogger", output = { log ->
            collectLog(log)
        }, isColored = false)
        PublicShared.logger4plugins.forEach {
            PublicShared.logger4plugins[it.key] = PlatformLogger(it.value.identity, output = { log ->
                collectLog(log)
            }, isColored = false)
        }
        GlobalEventChannel.subscribeAlways<Event> {
            println("Received event: $this")
            if (filter(this))
                eventList.add(this)
        }
    }

    suspend fun waitUntilEnd(count: Int = 1) {
        while (logList.count { it.contains("--test end--") } != count) {
            delay(1000)
        }
    }

    fun checkMessageChainJsonResultFromLog(mc: MessageChain) {
        val logs = logList.filter { it.contains("after_serialization:") }
        assertEquals(1, logs.size)
        println("rerere:" + logs.first())
        assertEquals(
            mc.serializeToJsonString(),
            MessageChain.deserializeFromJsonString(logs.first().substringAfter("after_serialization:"))
                .serializeToJsonString()
        )
    }

    inline fun <reified T : Event, R> noBroadcast(chan: EventChannel<Event>, block: () -> R): R {
        val listener = chan.subscribe<T>(priority = EventPriority.HIGHEST) { event ->
            event.intercept()
            ListeningStatus.LISTENING
        }
        listener.start()
        val re = block()
        listener.complete()
        return re
    }
}