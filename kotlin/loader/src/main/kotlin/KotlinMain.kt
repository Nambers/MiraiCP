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

import com.google.gson.Gson
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Job
import net.mamoe.mirai.utils.MiraiExperimentalApi
import net.mamoe.mirai.utils.MiraiInternalApi
import net.mamoe.mirai.utils.MiraiLogger
import tech.eritquearcus.miraicp.loader.console.Console
import tech.eritquearcus.miraicp.shared.CPP_lib
import tech.eritquearcus.miraicp.shared.Config
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.PublicShared.now_tag
import java.io.File
import kotlin.system.exitProcess

object KotlinMain {
    private val job = Job()
    val coroutineScope = CoroutineScope(job)
    lateinit var loginAccount: List<Config.accounts.Account>
    var logined = false
    @OptIn(MiraiInternalApi::class)
    fun main(j: String) {
        job.start()
        val c = Gson().fromJson(j, Config.accounts::class.java)
        loginAccount = c.accounts?: emptyList()
        Console
        val logger = MiraiLogger.Factory.create(this::class, "MiraiCP")
        PublicShared.init(logger)
        logger.info("⭐MiraiCP启动中⭐")
        logger.info("⭐github存储库:https://github.com/Nambers/MiraiCP")
        if (c.accounts == null || c.accounts!!.isEmpty()) {
            logger.error("Error: 无可登录账号，请检查config.json内容")
            return
        }
        PublicShared.threadNum = c.config?.threadNum ?: let { PublicShared.threadNum }
        c.accounts?.filter { it.autoLogin == true }?.forEach {
            it.login()
            logined = true
        }
        logger.info("⭐当前MiraiCP版本: $now_tag")
        c.cppPaths.forEach {
            val d = it.dependencies?.filter { p ->
                File(p).let { f ->
                    f.isFile && f.exists() && (f.extension == "dll" || f.extension == "lib" || f.extension == "so")
                }
            }
            val f = File(it.path)
            if (!f.exists() || !f.isFile) {
                logger.error("Error: dll文件${it.path} 不存在, 请检查config.json配置")
                return@forEach
            }
            logger.info("加载dll:${f.absolutePath}")
            val cpp = CPP_lib(f.absolutePath, d)
            cpp.showInfo()
            PublicShared.cpp.add(cpp)
            if (PublicShared.logger4plugins.contains(cpp.config.name))
                logger.warning("检测到列表已经有重复的${cpp.config.name}, 请检测配置文件中是否重复或提醒开发者改插件名称，但该插件还是会加载")
            PublicShared.logger4plugins[cpp.config.name] = MiraiLogger.Factory.create(this::class, cpp.config.name)
        }
        logger.info("⭐已成功加载MiraiCP⭐")
        Console.listen()
        while(!logined){}
    }
}

@MiraiExperimentalApi
@MiraiInternalApi
fun main(args: Array<String>){
    // config.json path
    val path = "config.json"
    var f = File(path)
    when(args.size) {
        1 -> {
            if(args[0] == "-g"){
                File("config.json").writeText(
                    """
                        {
                          "accounts": [{
                            "id": qqid,
                            "passwords": "passwords密码",
                            "protocol":  "pad",
                            "heatBeat": "STAT_HB",
                            "md5": false,
                            "autoLogin": false
                          }],
                          "cppPaths": {
                          "path":"dll路径",
                          "dependence":[
                            "依赖的dll路径"
                          ]
                          }
                        }

                    """.trimIndent()
                )
                println("生成成功")
                exitProcess(0)
            }
            f = File(args[0])
            if (!f.exists() || !f.isFile || !f.canRead()) {
                println("配置文件路径(${f.absolutePath})读取错误,文件不存在/不是文件/不可读, 使用默认路径(./config.json)重试")
                f = File(path)
                if (!f.exists() || !f.isFile || !f.canRead()) {
                    f = File(path)
                    if (!f.exists() || !f.isFile || !f.canRead()) {
                        println("默认配置文件路径(${f.absolutePath})读取错误,文件不存在/不是文件/不可读")
                        println("使用 -g 可以生成config.json模板(java -jar MiraiCP-loader-<version>.jar -g)")
                        exitProcess(1)
                    }
                }
            }
        }
        0 -> {
            if (!f.exists() || !f.isFile || !f.canRead()) {
                f = File(path)
                if (!f.exists() || !f.isFile || !f.canRead()) {
                    println("默认配置文件路径(${f.absolutePath})读取错误,文件不存在/不是文件/不可读")
                    println("使用 -g 可以生成config.json模板(java -jar MiraiCP-loader-<version>.jar -g)")
                    System.`in`.read()
                    exitProcess(1)
                }
            }
        }
        else->{
            println("参数多余，参数仅为配置文件位置(可选, 默认位置为/config.json)")
        }
    }
    println("正在启动\n配置文件地址:${f.absolutePath}")
    KotlinMain.main(f.readText())
}
