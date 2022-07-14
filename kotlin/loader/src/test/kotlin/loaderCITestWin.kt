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

import tech.eritquearcus.miraicp.loader.KotlinMain.alive
import tech.eritquearcus.miraicp.loader.KotlinMain.job
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
    "libLoaderPath": "../../cpp/cmake-build-debug/libLoader.$extension"
  },
  "accounts": [],
  "cppPaths": [
    {
      "path": "../../cpp/cmake-build-debug/libMiraiCP_multi.$extension"
    }
  ]
}
    """
        )
        println(File("").absolutePath)
        val text = mutableListOf<String>()
        Jline3AppenderImpl.handle = {
            text.add(it)
        }
        KotlinMain.exit = {
            PublicShared.exit()
            job.cancel()
            alive = false
        }
        main(arrayOf(f.absolutePath))
        val re = text.filter { it.contains("[ERROR]") && !it.contains("Error: 无可登录账号，请检查config.json内容") }
        assertTrue { re.isEmpty() }
        re.forEach {
            println(it)
        }
    }
}