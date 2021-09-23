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

package tech.eritquearcus.miraicp

import com.google.gson.Gson
import net.mamoe.mirai.console.plugin.jvm.JvmPluginDescription
import net.mamoe.mirai.console.plugin.jvm.KotlinPlugin
import net.mamoe.mirai.event.GlobalEventChannel
import net.mamoe.mirai.event.events.BotOnlineEvent
import net.mamoe.mirai.utils.MiraiLogger
import tech.eritquearcus.miraicp.console.registerCommands
import tech.eritquearcus.miraicp.shared.*
import java.io.File

object PluginMain : KotlinPlugin(
    JvmPluginDescription(
        id = "tech.eritquearcus.miraiCP",
        name = "miraiCP",
        version = "2.7.1-patch-1"
    ){
        author("Eritque arcus")
    }
) {
    override fun onEnable() {
        registerCommands()
        val l = MiraiLogger.Factory.create(this::class, "MiraiCP")
        PublicShared.init(l)
        PublicShared.cachePath = this.dataFolder.resolve("cache")
        if (PublicShared.cachePath.exists()) {
            PublicShared.cachePath.deleteRecursively()
        }
        PublicShared.cachePath.mkdir()
        l.info("⭐MiraiCP启动中⭐")
        l.info("本项目github存储库:https://github.com/Nambers/MiraiCP")
        Gson().fromJson(File("${dataFolder.absoluteFile}/miraicp.json").apply {
            if (!this.exists() || !this.isFile) {
                l.error("配置文件(${this.absolutePath})不存在或错误，将结束加载")
                l.error("配置文件应该在(${this.absolutePath}), 并且拥有以下json格式(见https://github.com/Nambers/MiraiCP/blob/master/doc/config.md):")
                l.error(
                    """
                    { 
                    "pluginConfig":[
                    {
                    "path":"插件路径"
                    }
                    ]
                    }
                """.trimIndent()
                )
                return
            }
        }.readText(), CPPConfig.pluginConfig::class.java)
            .apply {
                if (this.advanceConfig != null && this.advanceConfig!!.maxThread != null) {
                    if (this.advanceConfig!!.maxThread!! <= 0)
                        PublicShared.logger.error("配置错误: 配置项AdvanceConfig.maxThread的值应该>=0, 使用默认值")
                    else
                        PublicShared.maxThread = this.advanceConfig!!.maxThread!!
                }
            }
            .pluginConfig.forEach { i ->
                val it = i.path
                val d = i.dependencies?.filter { p ->
                    File(p).let { f ->
                        f.isFile && f.exists() && (f.extension == "dll" || f.extension == "lib" || f.extension == "so")
                    }
                }
                val f = File(it)
                when {
                    !f.isFile || !f.exists() -> {
                        error(it + "不是一个有效的文件")
                }
                f.extension != "dll" && f.extension != "so" -> {
                    error(it + "不是一个有效的dll或so文件")
                }
                else -> {
                    f.loadAsCPPLib(d)
                    }
                }
            }
        logger.info("⭐已成功启动MiraiCP⭐")
        GlobalEventChannel.parentScope(this).subscribeAlways<BotOnlineEvent> {
            PublicShared.cpp.Event(
                PublicShared.gson.toJson(CPPEvent.BotOnline(this.bot.id))
            )
        }
        PublicShared.onEnable(GlobalEventChannel.parentScope(this))
    }

    override fun onDisable() {
        PublicShared.onDisable()
    }
}
