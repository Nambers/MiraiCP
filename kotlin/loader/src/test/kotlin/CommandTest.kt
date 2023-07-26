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

package tech.eritquearcus.miraicp.loader

import kotlinx.coroutines.delay
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.message.data.PlainText
import net.mamoe.mirai.message.data.buildMessageChain
import org.junit.jupiter.api.Test
import tech.eritquearcus.miraicp.loader.console.Command
import tech.eritquearcus.miraicp.loader.console.LoaderCommandHandlerImpl
import tech.eritquearcus.miraicp.shared.PublicSharedData
import tech.eritquearcus.miraicp.testUtils.TestBase
import tech.eritquearcus.miraicp.testUtils.TestUtils
import tech.eritquearcus.miraicp.testUtils.TestUtils.waitUntilEnd

class CommandTest : TestBase() {
    @Test
    fun customCommandTest() = runBlocking {
        PublicSharedData.commandReg = LoaderCommandHandlerImpl()
        member.says("command")
        delay(100)
        Command.parse("commandPrefix a b c")
        waitUntilEnd(2)
        TestUtils.checkMessageChainJsonResultFromLog(buildMessageChain {
            add(PlainText("a"))
            add(PlainText("b"))
            add(PlainText("c"))
        })
    }
}