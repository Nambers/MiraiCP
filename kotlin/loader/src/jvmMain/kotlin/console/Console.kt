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

package tech.eritquearcus.miraicp.loader.console

import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.CoroutineName
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import org.jline.reader.LineReader
import org.jline.reader.LineReaderBuilder
import org.jline.reader.UserInterruptException
import org.jline.reader.impl.completer.NullCompleter
import org.jline.terminal.Terminal
import org.jline.terminal.TerminalBuilder
import org.jline.terminal.impl.AbstractWindowsTerminal
import tech.eritquearcus.miraicp.loader.KotlinMain
import tech.eritquearcus.miraicp.loader.KotlinMainData
import tech.eritquearcus.miraicp.shared.PublicSharedData
import java.time.LocalDateTime
import kotlin.system.exitProcess

object Console {
    val start: LocalDateTime = LocalDateTime.now() // 当前日期和时间

    private val terminal: Terminal =
        TerminalBuilder
            .builder()
            .name("MiraiCP Console")
            .system(true)
            .jansi(true)
            .dumb(true)
            .paused(true)
            .build()
            .let { terminal ->
                if (terminal is AbstractWindowsTerminal) {
                    runCatching {
                        AbstractWindowsTerminal::class.java.getDeclaredField("pump").also {
                            it.isAccessible = true
                        }
                    }.onFailure { err ->
                        err.printStackTrace()
                        return@let terminal.also { it.resume() }
                    }.getOrThrow()
                    return@let terminal.also { it.resume() }
                }
                terminal.also { it.resume() }
            }

    internal val lineReader: LineReader by lazy {
        LineReaderBuilder.builder().terminal(terminal).completer(NullCompleter()).build()
    }
    private const val prompt = "> "

    fun listen() {
        KotlinMainData.coroutineScope.launch(CoroutineName("Console Command")) {
            while (isActive) {
                val re = try {
                    lineReader.readLine(prompt)
                } catch (e: InterruptedException) {
                    PublicSharedData.logger.error(e)
                    return@launch
                } catch (e: CancellationException) {
                    PublicSharedData.logger.error(e)
                    return@launch
                } catch (e: UserInterruptException) {
                    KotlinMain.exit()
                    exitProcess(0)
                }
                if (re.isEmpty() || re.isBlank()) continue
                Command.parse(re)
            }
        }
    }

    fun registerShutDownHook() {
        val shutdownListener: Thread = object : Thread() {
            override fun run() {
                KotlinMain.exit()
            }
        }
        Runtime.getRuntime().addShutdownHook(shutdownListener)
    }
}