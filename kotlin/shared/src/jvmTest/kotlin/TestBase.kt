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
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.utils.MiraiExperimentalApi
import net.mamoe.mirai.utils.createFileIfNotExists
import org.junit.jupiter.api.AfterAll
import org.junit.jupiter.api.BeforeAll
import org.junit.jupiter.api.Order
import tech.eritquearcus.miraicp.shared.CPPLib
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.test.TestUtils.runCommand
import java.util.*
import kotlin.system.exitProcess

open class TestBase {
    companion object {
        @OptIn(MiraiExperimentalApi::class)
        val bot by lazy {
            val bot = TestUtils.newBot()
            bot.addFriend(111, "testOver")
            runBlocking {
                bot.login()
            }
            PublicShared.onEnable(bot.eventChannel)
            bot
        }

        @JvmStatic
        @BeforeAll
        @Order(0)
        fun loadCPPLib() {
            if (!TestUtils.init) {
                require(TestUtils.path.exists())
                require(TestUtils.cppPath.exists())
                val os = System.getProperty("os.name").lowercase(Locale.getDefault())
                var libPath = TestUtils.cppPath
                var pluginPath = TestUtils.cppPath
                val cfgPath = TestUtils.path.resolve("file/config.json")
                cfgPath.createFileIfNotExists()
                val result = when {
                    os.contains("win") -> {
                        libPath = libPath.resolve("build\\src\\libloader\\Debug\\libLoader.dll")
                        pluginPath = pluginPath.resolve("build\\Debug\\libMiraiCP_multi.dll")
                        "cmd /C \"(if not exist \"build\\\" mkdir build) && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DGOOGLE_TEST=OFF .. && cd .. && cmake --build build --target Loader && cmake --build build --target MiraiCP_multi\"".runCommand(
                            TestUtils.cppPath
                        )
                    }

                    os.contains("nix") || os.contains("nux") || os.contains("aix") -> {
                        "mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DGOOGLE_TEST=OFF .. && make Loader".runCommand(
                            TestUtils.cppPath
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

        @JvmStatic
        @AfterAll
        fun closeBot() {
            runBlocking {
                delay(1000)
            }
            bot.close()
        }
    }
}