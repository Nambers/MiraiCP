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

package tech.eritquearcus.miraicp.loader

import kotlinx.coroutines.runBlocking
import tech.eritquearcus.miraicp.loader.KotlinMainData.job
import tech.eritquearcus.miraicp.loader.console.Jline3AppenderImpl
import tech.eritquearcus.miraicp.shared.PublicShared
import java.io.File
import kotlin.test.Test
import kotlin.test.assertTrue

class LoaderCITest {
    @Test
    fun test() {
        val extension = if (System.getProperty("os.name").contains("Windows")) "dll" else "so"
        val f = File("config.json")
        f.writeText(
            """
{
  "advanceConfig": {
    "libLoaderPath": "${File("../../cpp/cmake-build-debug/libLoader.$extension").absolutePath.replace("\\", "/")}"
  },
  "accounts": [],
  "cppPaths": [
    {
      "path": "${File("../../cpp/cmake-build-debug/libMiraiCP_multi.$extension").absolutePath.replace("\\", "/")}"
    }
  ]
}
    """
        )
        println(File("").absolutePath)
        val text = mutableListOf<String>()
        Jline3AppenderImpl.handle = {
            println(it)
            text.add(it)
        }
        KotlinMain.exit = {
            PublicShared.exit()
            job.cancel()
            runBlocking {
                KotlinMainData.aliveChan.send(Unit)
            }
        }
        println("--- Start Loader CI Test ---")
        KotlinMainEntry.main(arrayOf(f.absolutePath))
        val re = text.filter { it.contains("[ERROR]") && !it.contains("Error: 无可登录账号，请检查config.json内容") }
        println("--- Errors: ---")
        re.forEach {
            println(it)
        }
        assertTrue { re.isEmpty() }
    }
}