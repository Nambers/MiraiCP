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

@file:Suppress("PackageDirectoryMismatch")

package tech.eritquearcus.miraicp.loader

import kotlinx.serialization.decodeFromString
import net.mamoe.mirai.utils.MiraiExperimentalApi
import tech.eritquearcus.miraicp.loader.console.Console
import tech.eritquearcus.miraicp.loader.console.LoaderCommandHandlerImpl
import tech.eritquearcus.miraicp.shared.*
import tech.eritquearcus.miraicp.shared.PublicSharedData.logger
import tech.eritquearcus.miraicp.uilts.MiraiCPFiles
import kotlin.native.concurrent.ThreadLocal
import kotlin.system.exitProcess

@ThreadLocal
actual object KotlinMain {
    actual var exit: () -> Unit = {
        PublicShared.exit()
        KotlinMainData.job.cancel()
        KotlinMainData.alive = false
        exitProcess(0)
    }

    @OptIn(MiraiExperimentalApi::class)
    actual fun main(j: String, path: String) {
        @Suppress("INVISIBLE_MEMBER", "INVISIBLE_REFERENCE") // 必要
        net.mamoe.mirai._MiraiInstance.set(net.mamoe.mirai.internal.MiraiImpl())
        KotlinMainData.job.start()
        Console
        val c = json.decodeFromString<CPPConfig.LoaderConfig>(j)
        KotlinMainData.loginAccount = c.accounts ?: emptyList()
        if (PublicSharedData.cachePath.exists()) PublicSharedData.cachePath.deleteRecursively()
        PublicSharedData.cachePath.mkdir()
        logger.info("⭐MiraiCP启动中⭐")
        logger.info("⭐github存储库:https://github.com/Nambers/MiraiCP")
        logger.info("⭐MiraiCP-plugin 版本: ${PublicShared.now_tag}, 构建时间: ${BuiltInConstants.date}, mirai版本: ${BuiltInConstants.miraiVersion}")
        PublicSharedData.commandReg = LoaderCommandHandlerImpl()
        val tmp = if (c.advanceConfig?.libLoaderPath != null) {
            val tmp2 = MiraiCPFiles.create(c.advanceConfig?.libLoaderPath!!)
            if (tmp2.exists() && tmp2.name.startsWith("libLoader") && tmp2.isFile)
                listOf(tmp2.parentPath)
            else {
                logger.error("AdvanceConfig 中的 libLoaderPath(${c.advanceConfig?.libLoaderPath ?: "\"\""}) 无效或不存在, 使用缺省路径")
                emptyList()
            }
        } else emptyList()


        CPPLib.init(libPath = tmp, cfgPath = path)
        if (c.accounts == null || c.accounts!!.isEmpty()) {
            logger.error("Error: 无可登录账号，请检查config.json内容")
            exit()
        }
        c.accounts?.filter { it.autoLogin == true }?.forEach {
            it.login()
            // logined = true
        }
        logger.info("⭐已成功加载MiraiCP⭐")
        Console.listen()
        // if not logged-in, wait user login in console
        while (KotlinMainData.alive) {
        }
    }
}

actual object KotlinMainEntry {
    actual fun main(args: Array<String>) {
        // config.json path
        val path = "config.json"
        var f = MiraiCPFiles.create(path)
        when (args.size) {
            1 -> {
                if (args[0] == "-g") {
                    MiraiCPFiles.create("config.json").writeText(
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
                f = MiraiCPFiles.create(args[0])
                if (!f.exists() || !f.isFile || !f.canRead()) {
                    println("配置文件路径(${f.absolutePath})读取错误,文件不存在/不是文件/不可读, 使用默认路径(./config.json)重试")
                    f = MiraiCPFiles.create(path)
                    if (!f.exists() || !f.isFile || !f.canRead()) {
                        f = MiraiCPFiles.create(path)
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
                    f = MiraiCPFiles.create(path)
                    if (!f.exists() || !f.isFile || !f.canRead()) {
                        println("默认配置文件路径(${f.absolutePath})读取错误,文件不存在/不是文件/不可读")
                        println("使用 -g 可以生成config.json模板(java -jar MiraiCP-loader-<version>.jar -g)")
//                        System.`in`.read()
                        exitProcess(1)
                    }
                }
            }

            else -> {
                println("参数多余，参数仅为配置文件位置(可选, 默认位置为/config.json)")
            }
        }
        println("正在启动\n配置文件地址:${f.absolutePath}")
        KotlinMain.main(f.readText(), f.absolutePath)
    }
}