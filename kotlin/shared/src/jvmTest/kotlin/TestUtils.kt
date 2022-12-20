/*
 * Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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
package tech.eritquearcus.miraicp.shared.test

import kotlinx.coroutines.delay
import net.mamoe.mirai.event.*
import net.mamoe.mirai.mock.MockBot
import net.mamoe.mirai.mock.MockBotFactory
import net.mamoe.mirai.utils.MiraiInternalApi
import net.mamoe.mirai.utils.PlatformLogger
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.PublicSharedData
import java.io.File
import java.io.IOException
import java.util.concurrent.TimeUnit

object TestUtils {
    var init = false
    val workingDir by lazy{
        File(System.getenv("libpath"))
    }
    val libExtension by lazy{
        if (System.getProperty("os.name").lowercase().contains("win")) {
            "dll"
        } else {
            "so"
        }
    }
    private val botFactory by lazy {
        MockBotFactory.initialize()
        MockBotFactory.newMockBotBuilder()
    }
    var filter: (Event) -> Boolean = { true }
    val eventList = mutableListOf<Event>()
    val logList = mutableListOf<String>()
    private var end = false
    private fun collectLog(log: String) {
        println(log)
        logList.add(log)
        if (log.contains("--test end--")) {
            end = true
        }
    }

    @OptIn(MiraiInternalApi::class)
    val listener by lazy {
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

    suspend fun waitUntilEnd() {
        while (!end) {
            delay(1000)
        }
        listener.cancel()
    }

    fun String.runCommand(workingDir: File): String? {
        return try {
            val parts = this.split("\\s".toRegex())
            val proc = ProcessBuilder(*parts.toTypedArray())
                .directory(workingDir)
                .redirectOutput(ProcessBuilder.Redirect.PIPE)
                .redirectError(ProcessBuilder.Redirect.PIPE)
                .start()

            proc.waitFor(60, TimeUnit.MINUTES)
            proc.inputStream.bufferedReader().readText() + proc.errorStream.bufferedReader().readText()
        } catch (e: IOException) {
            e.printStackTrace()
            null
        }
    }

    fun newBot(): MockBot = botFactory.create()

    inline fun <reified T : Event, R> noBroadcast(chan: EventChannel<Event>, block: () -> R): R {
        val listener = chan.subscribe<T>(priority = EventPriority.HIGHEST) { event ->
            event.intercept()
            ListeningStatus.LISTENING
        }
        listener.start()
        val re = block()
        listener.cancel()
        return re
    }
}