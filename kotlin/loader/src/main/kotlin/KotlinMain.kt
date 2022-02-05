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

import com.google.gson.Gson
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Job
import net.mamoe.mirai.utils.MiraiExperimentalApi
import net.mamoe.mirai.utils.MiraiInternalApi
import net.mamoe.mirai.utils.MiraiLogger
import tech.eritquearcus.miraicp.loader.console.Console
import tech.eritquearcus.miraicp.loader.console.LoaderCommandHandlerImpl
import tech.eritquearcus.miraicp.shared.BuiltInConstants
import tech.eritquearcus.miraicp.shared.CPPConfig
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.PublicShared.now_tag
import tech.eritquearcus.miraicp.shared.loadAsCPPLib
import java.io.File
import kotlin.system.exitProcess

object KotlinMain {
    private val job = Job()
    val coroutineScope = CoroutineScope(job)
    lateinit var loginAccount: List<CPPConfig.loaderConfig.Account>
    var logined = false

    fun main(j: String) {
        job.start()
        val c = Gson().fromJson(j, CPPConfig.loaderConfig::class.java)
        loginAccount = c.accounts ?: emptyList()
        Console
        val logger = MiraiLogger.Factory.create(this::class, "MiraiCP")
        PublicShared.init(logger)
        PublicShared.cachePath = File("cache")
        if (PublicShared.cachePath.exists()) PublicShared.cachePath.deleteRecursively()
        PublicShared.cachePath.mkdir()
        logger.info("⭐MiraiCP启动中⭐")
        logger.info("⭐github存储库:https://github.com/Nambers/MiraiCP")
        logger.info("⭐当前MiraiCP版本: $now_tag, 构建时间: ${BuiltInConstants.date}")
        PublicShared.commandReg = LoaderCommandHandlerImpl()
        c.apply {
            if (this.advanceConfig != null && this.advanceConfig!!.maxThread != null) {
                if (this.advanceConfig!!.maxThread!! <= 0) PublicShared.logger.error("配置错误: AdvanceConfig下maxThread项值应该>=0, 使用默认值")
                else PublicShared.maxThread = this.advanceConfig!!.maxThread!!
            }
        }.cppPaths.forEach {
            val d = it.dependencies?.filter { p ->
                File(p).let { f -> f.isFile && f.exists() }
            }
            val f = File(it.path)
            if (!f.exists() || !f.isFile) {
                logger.error("Error: dll文件${it.path} 不存在, 请检查config.json配置")
                return@forEach
            }
            f.loadAsCPPLib(d)
        }
        if (c.accounts == null || c.accounts!!.isEmpty()) {
            logger.error("Error: 无可登录账号，请检查config.json内容")
            return
        }
        c.accounts?.filter { it.autoLogin == true }?.forEach {
            it.login()
            logined = true
        }
        logger.info("⭐已成功加载MiraiCP⭐")
        Console.listen()
        // if not logged-in, wait user login in console
        while (!logined) {
        }
    }
}

fun main(args: Array<String>) {
    // config.json path
    val path = "config.json"
    var f = File(path)
    when (args.size) {
        1 -> {
            if (args[0] == "-g") {
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
        else -> {
            println("参数多余，参数仅为配置文件位置(可选, 默认位置为/config.json)")
        }
    }
    println("正在启动\n配置文件地址:${f.absolutePath}")
    KotlinMain.main(f.readText())
}
