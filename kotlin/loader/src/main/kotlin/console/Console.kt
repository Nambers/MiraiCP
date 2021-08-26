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

package tech.eritquearcus.miraicp.loader.console

import kotlinx.coroutines.*
import net.mamoe.mirai.utils.MiraiInternalApi
import net.mamoe.mirai.utils.MiraiLogger
import net.mamoe.mirai.utils.PlatformLogger
import org.fusesource.jansi.Ansi
import org.jline.reader.LineReader
import org.jline.reader.LineReaderBuilder
import org.jline.reader.UserInterruptException
import org.jline.reader.impl.completer.NullCompleter
import org.jline.terminal.Terminal
import org.jline.terminal.TerminalBuilder
import org.jline.terminal.impl.AbstractWindowsTerminal
import tech.eritquearcus.miraicp.loader.KotlinMain
import tech.eritquearcus.miraicp.shared.PublicShared
import java.time.LocalDateTime
import kotlin.system.exitProcess

@OptIn(MiraiInternalApi::class)
object Console {
    val start: LocalDateTime = LocalDateTime.now() // 当前日期和时间

    init {
        MiraiLogger.setDefaultLoggerCreator {
            PlatformLogger(it!!) { s ->
                lineReader.printAbove(s + Ansi().reset().toString())
            }
        }
    }

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
                    //                    terminal.setOnClose {
//                        response = NoConsole
//                    }
//                    terminal.resume()
//                    val pumpThread = pumpField[terminal] as? Thread ?: return@let NoConsole
//                    @Suppress("ControlFlowWithEmptyBody")
//                    while (pumpThread.state == Thread.State.NEW);
//                    Thread.sleep(1000)
//                    terminal.setOnClose(null)
                    return@let terminal.also { it.resume() }
                }
                terminal.also { it.resume() }
            }
    private val lineReader: LineReader by lazy {
        LineReaderBuilder.builder().terminal(terminal).completer(NullCompleter()).build()
    }
    private const val prompt = "> "
    private lateinit var listenJob: Job
    @OptIn(DelicateCoroutinesApi::class)
    fun listen() {
        listenJob = KotlinMain.coroutineScope.launch(CoroutineName("Console Command")) {
            while (isActive) {
                val re = try {
                    lineReader.readLine(prompt)
                } catch (e: InterruptedException) {
                    PublicShared.logger.error(e)
                    return@launch
                } catch (e: CancellationException) {
                    PublicShared.logger.error(e)
                    return@launch
                }catch(e: UserInterruptException){
                    exitProcess(1)
                }
                if (re.isEmpty() || re.isBlank()) continue
                Command.parse(re)
            }
        }
    }
}