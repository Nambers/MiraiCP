/*
 * Copyright (c) 2020 - 2021. Eritque arcus and contributors.
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

import com.google.gson.Gson
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.Bot
import tech.eritquearcus.miraicp.loader.KotlinMain
import tech.eritquearcus.miraicp.loader.login
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.loadAsCPPLib
import java.io.File
import java.time.Duration
import java.time.LocalDateTime
import kotlin.system.exitProcess

object Command {
    private const val ch = " "
    private fun printHelp() {
        val message = listOf(
            "exit" to "退出",
            "status" to "查看loader状态",
            "login <qqid>" to "登录已经配置在配置文件的qq",
            "accountList" to "简写aList, 查看配置文件里的qq",
            "pluginList" to "简写pList, 输出全部插件信息",
            "disablePluginList" to "简写dList, 输出全部被禁用的插件名称",
            "disablePlugin <plugin id>" to "简写disable, 禁用插件, 后面跟插件的名字作为参数, 可用enablePlugin启用, 可能会对程序的速度造成一些影响",
            "enablePlugin <plugin id>" to "简写enable, 启用插件, 后面跟插件的名字作为参数",
            "loadPlugin <plugin path>" to "简写load, 加载某个插件, 后面跟插件的地址作为参数",
            "removePlugin <plugin id>" to "简写rm, 移除插件但不会取消对dll文件的占用, 后面跟插件的名字作为参数",
            "reloadPlugin <plugin path>" to "简写reload, 加载某个插件(位于<plugin path>), 如果已经加载则`removePlugin`那个插件, 如果未加载过则效果类似`load`"
        )
        val prefixPlaceholder = String(CharArray(
            message.maxOfOrNull { it.first.length }!! + 3
        ) { ' ' })

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
        when (re.size) {
            0 -> unknown(order)
            1 -> pureOrder(re[0])
            2 -> oneParamOrder(arrayOf(re[0], re[1]))
            3 -> twoParamOrder(arrayOf(re[0], re[1], re[2]))
            else -> unknown(order)
        }
    }

    private fun unknown(order: String) {
        PublicShared.logger.error("未知命令: '$order', 输入 \"help\" 获取命令帮助")
    }

    private fun error(order: String, reason: String){
        PublicShared.logger.error("命令错误: '$order', $reason")
    }
    private fun info(msg:String) = println(msg)

    private fun pureOrder(order: String) {
        when (order) {
            "exit" -> {
                PublicShared.onDisable()
                PublicShared.logger.info("Closing MiraiCP...")
                Bot.instances.forEach {
                    runBlocking {
                        it.closeAndJoin()
                    }
                    PublicShared.logger.info("Bot ${it.id} closed")
                }
                exitProcess(0)
            }
            "help" -> printHelp()
            "status" -> {
                val s = Duration.between(Console.start, LocalDateTime.now()).seconds
                println("该Loader已经持续运行 " + String.format("%d:%02d:%02d", s / 3600, (s % 3600) / 60, (s % 60)) + " 啦")
            }
            "accountList", "aList" -> KotlinMain.loginAccount.let { acs->
                val gson = Gson()
                acs.forEach { println(gson.toJson(it)) }
            }
            "pluginList", "pList" ->{
                PublicShared.cpp.forEach {
                    it.showInfo()
                }
            }
            "disablePluginList", "dList"->{
                PublicShared.disablePlugins.forEach {
                    println(it)
                }
            }
            else -> unknown(order)
        }
    }

    private fun login(id: Long) {
        KotlinMain.loginAccount.first { it.id == id && (it.logined == null || it.logined == false) }.login()
    }

    private fun removePlugin(order: String, id: String) {
        try {
            PublicShared.cpp.filter {
                it.config.id == id
            }.forEach {
                PublicShared.cpp.remove(it)
                PublicShared.disablePlugins.contains(id) && PublicShared.disablePlugins.remove(id)
                info("成功移除(${id})${it.config.name}插件")
            }
        } catch (e: NoSuchElementException) {
            error(order, "已使用的插件列表中没有${id}")
        }
    }

    private fun reload(order: String, path: String) {
        File(path).let { plugin ->
            when {
                (!plugin.isFile || !plugin.exists()) -> error(order, "插件(${path})不存在")
                (plugin.extension != "dll" && plugin.extension != "so") -> error(order, "插件(${path})不是dll或so文件")
                else -> {
                    val p = plugin.loadAsCPPLib(emptyList(), true)
                    PublicShared.cpp.filter { it.config.id == p.config.id }.apply {
                        if (this.isEmpty())
                            PublicShared.logger.warning("重载未找到id为(${p.config.id}), 但会继续执行, 效果类似`load`")
                    }.forEach {
                        PublicShared.cpp.remove(it)
                        PublicShared.disablePlugins.contains(it.config.id) && PublicShared.disablePlugins.remove(it.config.id)
                    }
                    PublicShared.cpp.add(p)
                }
            }
        }

    }

    private fun oneParamOrder(order: Array<String>) {
        when (order[0]) {
            "login" -> {
                val id = try {
                    order[1].toLong()
                } catch (e: NumberFormatException) {
                    error(order.joinToString(" "), order[1] + "不是有效的qq号")
                    return
                }
                try{
                    login(id)
                }catch(e:NoSuchElementException ){
                    error(order.joinToString(" "), "config文件中没找到关于$id 的定义或该id已经通过自动登录登录")
                }
                KotlinMain.logined = true
            }
            "loadPlugin", "load" ->{
                val f = File(order[1])
                when {
                    !f.isFile || !f.exists() -> {
                        error(order.joinToString(" "), order[1] + "不是一个有效的文件")
                    }
                    else -> {
                        f.loadAsCPPLib(emptyList())
                    }
                }
            }
            "disablePlugin", "disable" ->{
                try{
                    PublicShared.cpp.first { it.config.id == order[1] }
                    (!PublicShared.disablePlugins.contains(order[1])) && PublicShared.disablePlugins.add(order[1])
                    info("禁用${order[1]}成功")
                }catch(e:NoSuchElementException){
                    error(order.joinToString(" "), "找不到${order[1]}插件, 关闭失败")
                }
            }
            "enablePlugin", "enable" -> {
                if (PublicShared.disablePlugins.contains(order[1])) {
                    PublicShared.disablePlugins.remove(order[1])
                    info("启用${order[1]}成功")
                }
            }
            "removePlugin", "rm" -> {
                removePlugin(order.joinToString(" "), order[1])
            }
            "reloadPlugin", "reload" -> {
                reload(order.joinToString(" "), order[1])
            }
            else -> unknown(order.joinToString(" "))
        }
    }

    private fun twoParamOrder(order: Array<String>){
        unknown(order.joinToString(" "))
    }
}