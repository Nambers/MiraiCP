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

import net.mamoe.mirai.console.plugin.jvm.JvmPluginDescription
import net.mamoe.mirai.console.plugin.jvm.KotlinPlugin
import net.mamoe.mirai.event.GlobalEventChannel
import net.mamoe.mirai.event.events.BotOnlineEvent
import net.mamoe.mirai.utils.MiraiLogger
import tech.eritquearcus.miraicp.shared.CPP_lib
import tech.eritquearcus.miraicp.shared.Config
import tech.eritquearcus.miraicp.shared.Event
import tech.eritquearcus.miraicp.shared.PublicShared
import java.io.File

object PluginMain : KotlinPlugin(
    JvmPluginDescription(
        id = "tech.eritquearcus.miraiCP",
        name = "miraiCP",
        version = "2.7-RC-dev5"
    ){
        author("Eritque arcus")
    }
) {
    override fun onEnable() {
        val l = MiraiLogger.Factory.create(this::class, "MiraiCP")
        PublicShared.init(l)
        l.info("⭐MiraiCP启动中⭐")
        l.info("本项目github存储库:https://github.com/Nambers/MiraiCP")
        File("${dataFolder.absoluteFile}\\miraicp.txt").apply{
            if(!this.exists() || !this.isFile) {
                l.error("配置文件(${this.absolutePath})不存在或错误，将结束加载")
                return
            }
        }.readLines().forEach {
                val f = File(it)
                when{
                    !f.isFile || !f.exists()->{
                        error(it + "不是一个有效的文件")
                    }
                    f.extension != "dll" && f.extension != "so"->{
                        error(it + "不是一个有效的dll或so文件")
                    }
                    else->{
                        PublicShared.logger.info("加载${it}成功")
                        CPP_lib(it).let {cpp->
                            cpp.showInfo()
                            PublicShared.logger4plugins[cpp.config.name] = MiraiLogger.Factory.create(this::class, cpp.config.name)
                            PublicShared.cpp.add(cpp)
                        }

                    }
                }
            }
        logger.info("⭐已成功启动MiraiCP⭐")
        GlobalEventChannel.parentScope(this).subscribeAlways<BotOnlineEvent> {
            PublicShared.cpp.Event(
                PublicShared.gson.toJson(Config.BotOnline(this.bot.id))
            )
        }
        PublicShared.onEnable(GlobalEventChannel.parentScope(this))
    }

    override fun onDisable() {
        PublicShared.onDisable()
    }
}
