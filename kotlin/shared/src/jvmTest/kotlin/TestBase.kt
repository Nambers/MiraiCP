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
import org.junit.jupiter.api.fail
import tech.eritquearcus.miraicp.shared.CPPLib
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.test.TestUtils.runCommand
import java.io.File
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
                println("Currently working dir:" + TestUtils.workingDir)
                require(TestUtils.workingDir.exists())
                val cfgPath = TestUtils.workingDir.resolve("testFileFromKt4Mock/config.json")
                cfgPath.createFileIfNotExists()
                cfgPath.writeText(
                    """
{
  "accounts": [],
  "cppPaths": [
    {
      "path": "${TestUtils.workingDir.resolve("libMiraiCP_multi.${TestUtils.libExtension}").absolutePath.replace(File.separator, "/")}"
    }
  ]
}
    """.trimIndent()
                )
                bot
                CPPLib.init(listOf(TestUtils.workingDir.absolutePath.replace(File.separator, "/")),
                    cfgPath.absolutePath)
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