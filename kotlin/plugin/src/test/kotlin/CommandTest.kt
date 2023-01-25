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

package tech.eritquearcus.miraicp.plugin

import kotlinx.coroutines.delay
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.console.command.CommandManager
import net.mamoe.mirai.console.command.ConsoleCommandSender
import net.mamoe.mirai.console.command.descriptor.ExperimentalCommandDescriptors
import net.mamoe.mirai.console.util.ConsoleExperimentalApi
import net.mamoe.mirai.message.data.PlainText
import net.mamoe.mirai.message.data.buildMessageChain
import org.junit.jupiter.api.Test
import tech.eritquearcus.miraicp.console.CommandHandlerImpl
import tech.eritquearcus.miraicp.shared.PublicSharedData
import tech.eritquearcus.miraicp.testUtils.TestBase
import tech.eritquearcus.miraicp.testUtils.TestUtils.checkMessageChainJsonResultFromLog
import tech.eritquearcus.miraicp.testUtils.TestUtils.waitUntilEnd

class CommandTest : TestBase() {
    @OptIn(ExperimentalCommandDescriptors::class, ConsoleExperimentalApi::class)
    @Test
    fun customCommandTest() = runBlocking {
        // todo commandTest don't work
        PublicSharedData.commandReg = CommandHandlerImpl()
        member.says("command")
        delay(100)
        CommandManager.executeCommand(ConsoleCommandSender, buildMessageChain { add(PlainText("commandPrefix a b c")) })
        waitUntilEnd(2)
        checkMessageChainJsonResultFromLog(buildMessageChain {
            add(PlainText("a"))
            add(PlainText("b"))
            add(PlainText("c"))
        })
    }
}