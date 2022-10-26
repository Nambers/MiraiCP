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

package tech.eritquearcus.miraicp.loader.console

import kotlinx.serialization.encodeToString
import net.mamoe.mirai.message.data.MessageChain.Companion.serializeToJsonString
import net.mamoe.mirai.message.data.MessageChainBuilder
import net.mamoe.mirai.message.data.PlainText
import net.mamoe.mirai.utils.MiraiExperimentalApi
import tech.eritquearcus.miraicp.loader.KotlinMainData
import tech.eritquearcus.miraicp.loader.console.CommandMultiplatform.pureOrder
import tech.eritquearcus.miraicp.loader.login
import tech.eritquearcus.miraicp.shared.Command2C
import tech.eritquearcus.miraicp.shared.PublicShared.json
import tech.eritquearcus.miraicp.shared.PublicSharedData
import tech.eritquearcus.miraicp.shared.UlitsMultiPlatform.event
import tech.eritquearcus.miraicp.uilts.MiraiCPFiles

expect object CommandMultiplatform {
    fun pureOrder(order: String)
}

object Command {
    private const val ch = " "
    internal val preCommand = mutableListOf<CommandBrief>()
    internal val lastCommand = mutableListOf<CommandBrief>()
    internal val message = mutableListOf(
        "exit" to "退出",
        "status" to "查看loader状态",
        "login <qqid>" to "登录已经配置在配置文件的qq",
        "accountList" to "简写aList, 查看配置文件里的qq",
        "pluginList" to "简写pList, 输出全部插件信息",
        "disablePluginList" to "简写dList, 输出全部被禁用的插件名称",
        "disablePlugin <plugin id>" to "简写disable, 禁用插件, 后面跟插件的名字作为参数, 可用enablePlugin启用, 可能会对程序的速度造成一些影响",
        "enablePlugin <plugin id>" to "简写enable, 启用插件, 后面跟插件的名字作为参数",
        "loadPlugin <plugin path>" to "简写load, 加载某个插件, 后面跟插件的地址作为参数",
        "unloadPlugin <plugin id>" to "简写unload, 移除插件但不会取消对dll文件的占用, 后面跟插件的名字作为参数",
        "reloadPlugin <plugin id>" to "简写reload, 重新加载某个插件"
    )

    fun printHelp() {
        val prefixPlaceholder = CharArray(
            message.maxOfOrNull { it.first.length }!! + 3
        ) { ' ' }.concatToString()

        // From Mamoe Technologies and contributors
        fun printOption(optionName: String, value: String) {
            if (optionName == "") {
                println(value)
                return
            }
            print(optionName)
            print(prefixPlaceholder.substring(optionName.length))
            val lines = value.split('\n').iterator()
            if (lines.hasNext()) println(lines.next())
            lines.forEach { line ->
                print(prefixPlaceholder)
                println(line)
            }
        }
        message.forEach { (optionName, value) ->
            printOption(optionName, value)
        }
    }

    fun parse(order: String) {
        val re = order.split(ch)
        if (re.isEmpty()) unknown(order)
        if (preOneOrMoreParamOrder(re)) return
        when (re.size) {
            1 -> pureOrder(re[0])
            2 -> oneParamOrder(listOf(re[0], re[1]), re)
            else -> lastOneOrMoreParamOrder(re)
        }
    }

    private fun unknown(order: String) = PublicSharedData.logger.error("未知命令: '$order', 输入 \"help\" 获取命令帮助")

    private fun error(order: String, reason: String) = PublicSharedData.logger.error("命令错误: '$order', $reason")

    @OptIn(MiraiExperimentalApi::class)
    private fun login(id: Long) =
        KotlinMainData.loginAccount.first { it.id == id && (it.logined == null || it.logined == false) }.login()

    private fun oneParamOrder(order: List<String>, re: List<String>) {
        when (order[0]) {
            "login" -> {
                val id = try {
                    order[1].toLong()
                } catch (e: NumberFormatException) {
                    error(order.joinToString(" "), order[1] + "不是有效的qq号")
                    return
                }
                try {
                    login(id)
                } catch (e: NoSuchElementException) {
                    error(order.joinToString(" "), "config文件中没找到关于$id 的定义或该id已经通过自动登录登录")
                }
            }

            "loadPlugin", "load" -> {
                val f = MiraiCPFiles.create(order[1])
                when {
                    !f.isFile || !f.exists() -> {
                        error(order.joinToString(" "), order[1] + "不是一个有效的文件")
                    }

                    else -> {
                        event(Packets.Outgoing.LibLoaderEvent("LoadPlugin", f.absolutePath))
                    }
                }
            }

            "disablePlugin", "disable" -> {
                event(Packets.Outgoing.LibLoaderEvent("DisablePlugin", order[1]))
            }

            "enablePlugin", "enable" -> {
                event(Packets.Outgoing.LibLoaderEvent("EnablePlugin", order[1]))
            }

            "unloadPlugin", "unload" -> {
                event(Packets.Outgoing.LibLoaderEvent("UnloadPlugin", order[1]))
            }

            "reloadPlugin", "reload" -> {
                event(Packets.Outgoing.LibLoaderEvent("ReloadPlugin", order[1]))
            }

            else -> lastOneOrMoreParamOrder(re)
        }
    }

    private fun List<CommandBrief>.matchCommand(order: List<String>): Boolean {
        this.firstOrNull { pair ->
            pair.name == order[0] || pair.sName.contains(order[0])
        }?.let { it ->
            val mc = MessageChainBuilder()
            order.drop(1).forEach { mc.append(PlainText(it)) }
            val tmp = json.encodeToString(
                Command2C(
                    null,
                    0,
                    mc.build().serializeToJsonString(),
                    it.bid
                )
            )
            // 为什么之前直接Event了
            event(tmp)
            return true
        }
        return false
    }

    private fun preOneOrMoreParamOrder(order: List<String>): Boolean {
        return preCommand.matchCommand(order)
    }

    fun lastOneOrMoreParamOrder(order: List<String>) {
        if (!lastCommand.matchCommand(order))
            unknown(order.joinToString(" "))
    }
}