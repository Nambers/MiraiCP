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

package tech.eritquearcus.miraicp.loader

import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.utils.MiraiInternalApi
import org.junit.jupiter.api.Test
import kotlin.system.exitProcess

class Test {
    @Test
    @OptIn(MiraiInternalApi::class, net.mamoe.mirai.utils.MiraiExperimentalApi::class)
    fun groupMessageTest() {
        runBlocking {
            main(arrayOf("../config.json"))
            groupMessage()
            exitProcess(0)
        }
    }
}