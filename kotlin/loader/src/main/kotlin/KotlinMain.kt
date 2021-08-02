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
import net.mamoe.mirai.event.GlobalEventChannel
import net.mamoe.mirai.event.events.BotOnlineEvent
import net.mamoe.mirai.utils.*
import org.fusesource.jansi.AnsiConsole
import tech.eritquearcus.miraicp.loader.console.Console
import tech.eritquearcus.miraicp.shared.CPP_lib
import tech.eritquearcus.miraicp.shared.Config
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.PublicShared.gson
import tech.eritquearcus.miraicp.shared.PublicShared.logger
import tech.eritquearcus.miraicp.shared.PublicShared.now_tag
import tech.eritquearcus.miraicp.shared.PublicShared.onEnable
import java.io.File

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
        val logger = MiraiLogger.create("MiraiCP")
        var dll_name = c.cppPath
        logger.info("⭐MiraiCP启动中⭐")
        logger.info("⭐github存储库:https://github.com/Nambers/MiraiCP")
        if (!File(dll_name).exists()) {
            logger.error("Error: dll文件$dll_name 不存在, 请检查config.json配置")
            return
        } else {
            dll_name = File(dll_name).absolutePath
        }
        logger.info("⭐当前MiraiCP版本: $now_tag")
        PublicShared.init(logger, dll_name)
        logger.info("⭐c++ dll地址:${dll_name}")
        val cpp = CPP_lib()
        PublicShared.cpp = cpp
        logger.info("⭐已加载插件: ${cpp.config.name}")
        logger.info("⭐作者: ${cpp.config.author}")
        logger.info("⭐版本: ${cpp.config.version}")
        if (cpp.config.description != "")
            logger.info("⭐描述: ${cpp.config.description}")
        if (cpp.config.time != "")
            logger.info("⭐发行时间: ${cpp.config.time}")
        if (c.accounts == null || c.accounts!!.isEmpty()) {
            logger.error("Error: 无可登录账号，请检查config.json内容")
            return
        }
        logger.info("⭐已成功加载MiraiCP⭐")
        PublicShared.logger4plugins[cpp.config.name] = MiraiLogger.create(cpp.config.name)
        if (cpp.config.MiraiCPversion != now_tag) {
            logger.warning("Warning: 当前MiraiCP框架版本($now_tag)和加载的插件的C++ SDK(${cpp.config.MiraiCPversion})不一致")
        }
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
            f = File(args[0])
            if (!f.exists() || !f.isFile || !f.canRead()) {
                println("配置文件路径(${f.absolutePath})读取错误,文件不存在/不是文件/不可读, 使用默认路径重试")
                f = File(path)
                if (!f.exists() || !f.isFile || !f.canRead()) {
                    f = File(path)
                    if (!f.exists() || !f.isFile || !f.canRead()) {
                        println("默认配置文件路径(${f.absolutePath})读取错误,文件不存在/不是文件/不可读")
                        return
                    }
                }
            }
        }
        0 -> {
            if (!f.exists() || !f.isFile || !f.canRead()) {
                f = File(path)
                if (!f.exists() || !f.isFile || !f.canRead()) {
                    println("默认配置文件路径(${f.absolutePath})读取错误,文件不存在/不是文件/不可读")
                    return
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
