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

package events

import net.mamoe.mirai.mock.MockBot
import net.mamoe.mirai.mock.MockBotFactory
import net.mamoe.mirai.utils.createFileIfNotExists
import tech.eritquearcus.miraicp.shared.CPPLib
import java.io.File
import java.io.IOException
import java.util.*
import java.util.concurrent.TimeUnit
import kotlin.system.exitProcess

object Init {
    private var init = false
    private val path = File("./src/jvmTest/kotlin")
    private val cppPath = File("../../cpp/")
    private val botFactory by lazy {
        MockBotFactory.initialize()
        MockBotFactory.newMockBotBuilder()
    }

    private fun String.runCommand(workingDir: File): String? {
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

    fun loadCPPLib() {
        if (!init) {
            require(path.exists())
            require(cppPath.exists())
            val os = System.getProperty("os.name").lowercase(Locale.getDefault())
            var libPath = cppPath
            var pluginPath = cppPath
            val cfgPath = path.resolve("file/config.json")
            cfgPath.createFileIfNotExists()
            val result = when {
                os.contains("win") -> {
                    libPath = libPath.resolve("build\\src\\libloader\\Debug\\libLoader.dll")
                    pluginPath = pluginPath.resolve("build\\Debug\\libMiraiCP_multi.dll")
                    "cmd /C \"(if not exist \"build\\\" mkdir build) && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DGOOGLE_TEST=OFF .. && cd .. && cmake --build build --target Loader && cmake --build build --target MiraiCP_multi\"".runCommand(
                        cppPath
                    )
                }

                os.contains("nix") || os.contains("nux") || os.contains("aix") -> {
                    "mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DGOOGLE_TEST=OFF .. && make Loader".runCommand(
                        cppPath
                    )
                }

                else -> null
            }
            require(libPath.exists())
            require(pluginPath.exists())
            cfgPath.writeText(
                """
{
  "advanceConfig": {
    "libLoaderPath": "${libPath.absolutePath.replace("\\", "/")}"
  },
  "accounts": [],
  "cppPaths": [
    {
      "path": "${pluginPath.absolutePath.replace("\\", "/")}"
    }
  ]
}
    """.trimIndent()
            )
            if (result?.contains("err") == true) {
                println(result)
                exitProcess(-1)
            }
            CPPLib.init(listOf(libPath.parent), cfgPath.absolutePath)
        }
    }

    fun newBot(): MockBot = botFactory.create()
}