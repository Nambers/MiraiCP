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

package tech.eritquearcus.miraicp.loader.console

import com.google.gson.Gson
import net.mamoe.mirai.BotFactory
import net.mamoe.mirai.utils.MiraiLogger
import tech.eritquearcus.miraicp.loader.KotlinMain
import tech.eritquearcus.miraicp.loader.login
import tech.eritquearcus.miraicp.shared.CPP_lib
import tech.eritquearcus.miraicp.shared.PublicShared
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
            "disablePlugin <plugin name>" to "简写disable, 禁用插件, 后面跟插件的名字作为参数, 可用enablePlugin启用, 可能会对程序的速度造成一些影响",
            "enablePlugin <plugin name>" to "简写enable, 启用插件, 后面跟插件的名字作为参数",
            "loadPlugin <plugin path>" to "简写load, 加载某个插件, 后面跟插件的地址作为参数",
            "removePlugin <plugin name>" to "简写rm, 移除插件但不会取消对dll文件的占用, 后面跟插件的名字作为参数"
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

    private fun login(id:Long){
        KotlinMain.loginAccount.first { it.id == id && (it.logined == null || it.logined == false)}.login()
    }
    private fun removePlugin(order:String, name: String){
        try {
            PublicShared.cpp.filter {
                it.config.name == name
            }.forEach {
                PublicShared.cpp.remove(it)
                PublicShared.disablePlugins.contains(name)&&PublicShared.disablePlugins.remove(name)
                info("成功移除${name}插件")
            }
        }catch(e:NoSuchElementException){
            error(order, "已使用的插件列表中没有${name}")
        }
    }
    private fun oneParamOrder(order: Array<String>) {
        when(order[0]){
            "login" -> {
                val id = try {
                    order[1].toLong()
                }catch(e:NumberFormatException){
                    error(order.joinToString(" "),order[1] + "不是有效的qq号")
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
                when{
                    !f.isFile || !f.exists()->{
                        error(order.joinToString(" "),order[1] + "不是一个有效的文件")
                    }
                    f.extension != "dll" && f.extension != "so"->{
                        error(order.joinToString(" "),order[1] + "不是一个有效的dll或so文件")
                    }
                    else->{
                        CPP_lib(order[1]).let {cpp->
                            cpp.showInfo()
                            PublicShared.logger4plugins[cpp.config.name] = MiraiLogger.Factory.create(this::class, cpp.config.name)
                            PublicShared.cpp.add(cpp)
                        }
                        PublicShared.logger.info("加载${order[1]}成功")
                    }
                }
            }
            "disablePlugin", "disable" ->{
                try{
                    PublicShared.cpp.first { it.config.name == order[1] }
                    (!PublicShared.disablePlugins.contains(order[1]))&&PublicShared.disablePlugins.add(order[1])
                    info("禁用${order[1]}成功")
                }catch(e:NoSuchElementException){
                    error(order.joinToString(" "), "找不到${order[1]}插件, 关闭失败")
                }
            }
            "enablePlugin", "enable"->{
                if(PublicShared.disablePlugins.contains(order[1])){
                    PublicShared.disablePlugins.remove(order[1])
                    info("启用${order[1]}成功")
                }
            }
            "removePlugin", "rm"->{
                removePlugin(order.joinToString(" "), order[1])
            }
            else -> unknown(order.joinToString(" "))
        }
    }

    private fun twoParamOrder(order: Array<String>){
        when(order[0]){
            else-> unknown(order.joinToString(" "))
        }
    }
}