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
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.BotFactory
import net.mamoe.mirai.event.events.BotOnlineEvent
import net.mamoe.mirai.utils.BotConfiguration
import net.mamoe.mirai.utils.MiraiExperimentalApi
import net.mamoe.mirai.utils.MiraiInternalApi
import net.mamoe.mirai.utils.MiraiLogger
import tech.eritquearcus.miraicp.loader.console.Console
import tech.eritquearcus.miraicp.shared.CPP_lib
import tech.eritquearcus.miraicp.shared.Config
import tech.eritquearcus.miraicp.shared.Event
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.PublicShared.gson
import tech.eritquearcus.miraicp.shared.PublicShared.logger
import tech.eritquearcus.miraicp.shared.PublicShared.now_tag
import tech.eritquearcus.miraicp.shared.PublicShared.onEnable
import java.io.File
import kotlin.system.exitProcess

private fun String.decodeHex(): ByteArray {
    check(length % 2 == 0) { "Must have an even length" }

    return chunked(2)
        .map { it.toInt(16).toByte() }
        .toByteArray()
}

fun Config.accounts.Account.login(){
    val it = this
    this.logined = true
    val p = when (it.protocol?.uppercase()) {
        "PAD" -> BotConfiguration.MiraiProtocol.ANDROID_PAD
        "WATCH" -> BotConfiguration.MiraiProtocol.ANDROID_WATCH
        "PHONE" -> BotConfiguration.MiraiProtocol.ANDROID_PHONE
        null -> BotConfiguration.MiraiProtocol.ANDROID_PHONE
        else -> {
            logger.warning("Warning: 登录协议无效, 应为PAD/WATCH/PHONE其中一个,使用默认的PHONE进行登录")
            BotConfiguration.MiraiProtocol.ANDROID_PHONE
        }
    }
    val h = when (it.heatBeat?.uppercase()) {
        "STAT_HB" -> BotConfiguration.HeartbeatStrategy.STAT_HB
        "REGISTER" -> BotConfiguration.HeartbeatStrategy.REGISTER
        "NONE" -> BotConfiguration.HeartbeatStrategy.NONE
        null -> BotConfiguration.HeartbeatStrategy.STAT_HB
        else -> {
            logger.warning("Warning: 心跳策略无效, 应为\"STAT_HB\"\\\"REGISTER\"\\\"None\"其中一个，使用默认的STAT_HB登录")
            BotConfiguration.HeartbeatStrategy.STAT_HB
        }
    }
    logger.info("登录bot:${it.id}")
    logger.info("协议:${p.name}")
    logger.info("心跳策略:${h.name}")
    val b = if (it.md5 == null || !it.md5!!) {
        BotFactory.newBot(it.id, it.passwords) {
            fileBasedDeviceInfo()
            this.protocol = p
            this.heartbeatStrategy = h
        }
    } else {
        BotFactory.newBot(it.id, it.passwords.decodeHex()) {
            fileBasedDeviceInfo()
            this.protocol = p
            this.heartbeatStrategy = h
        }
    }
    b.eventChannel.subscribeAlways<BotOnlineEvent> {
        PublicShared.cpp.Event(
            gson.toJson(Config.BotOnline(this.bot.id))
        )
    }
    runBlocking {
        b.login()
    }
    onEnable(b.eventChannel)
}

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
        logger.info("⭐MiraiCP启动中⭐")
        logger.info("⭐github存储库:https://github.com/Nambers/MiraiCP")
        c.cppPath.forEach {
            var dll_name = it
            if (!File(dll_name).exists()) {
                logger.error("Error: dll文件$dll_name 不存在, 请检查config.json配置")
                return@forEach
            } else {
                dll_name = File(dll_name).absolutePath
            }
            logger.info("⭐当前MiraiCP版本: $now_tag")
            PublicShared.init(logger)
            logger.info("⭐c++ dll地址:${dll_name}")
            val cpp = CPP_lib(dll_name)
            cpp.showInfo()
            PublicShared.cpp.add(cpp)
            if(PublicShared.logger4plugins.contains(cpp.config.name))
                logger.warning("检测到列表已经有重复的${cpp.config.name}, 请检测配置文件中是否重复或提醒开发者改插件名称，但该插件还是会加载")
            PublicShared.logger4plugins[cpp.config.name] = MiraiLogger.Factory.create(this::class, cpp.config.name)
        }
        if (c.accounts == null || c.accounts!!.isEmpty()) {
            logger.error("Error: 无可登录账号，请检查config.json内容")
            return
        }
        logger.info("⭐已成功加载MiraiCP⭐")
        Console.listen()
        c.accounts?.filter { it.autoLogin == true }?.forEach {
            it.login()
            logined = true
        }
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
                          "cppPath": "dll路径"
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
