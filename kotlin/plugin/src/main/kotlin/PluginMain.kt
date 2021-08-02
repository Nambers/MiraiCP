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
import tech.eritquearcus.miraicp.shared.PublicShared
import java.io.File

object PluginMain : KotlinPlugin(
    JvmPluginDescription(
        id = "tech.eritquearcus.miraiCP",
        name = "miraiCP",
        version = "2.7-RC-dev3"
    ){
        author("Eritque arcus")
    }
) {
    override fun onEnable() {
        val l = MiraiLogger.create("MiraiCP")
        l.info("⭐MiraiCP启动中⭐")
        l.info("本项目github存储库:https://github.com/Nambers/MiraiCP")
        var dll_name = "cpp.dll"
        dll_name = "${dataFolder.absoluteFile}\\$dll_name"
        val Configuration = "${dataFolder.absoluteFile}\\miraicp.txt"
        if (!File(Configuration).exists() || !File(Configuration).isFile) {
            l.error("配置文件$Configuration 不存在，使用默认dll路径$dll_name")
        } else {
            val c = File(Configuration).readLines()
            if (c.size != 1 || !File(c[0]).exists() || !File(c[0]).isFile || File(c[0]).extension != "dll") {
                l.error("配置文件$Configuration 格式错误，应只包含一行且为一个已存在的dll文件，使用默认dll路径$dll_name")
            } else {
                dll_name = c[0]
            }
        }
        if (!File(dll_name).exists()) {
            l.error("c++文件$dll_name 不存在")
            l.info("未找到配套dll插件，本插件退出加载，可去https://github.com/Nambers/MiraiCP 了解更多")
            return
        }
        PublicShared.init(l, dll_name)
        val cpp = CPP_lib()
        logger.info("⭐已加载插件: ${cpp.config.name}")
        logger.info("⭐作者: ${cpp.config.author}")
        logger.info("⭐版本: ${cpp.config.version}")
        if(cpp.config.description!= "")
            logger.info("⭐描述: ${cpp.config.description}")
        if(cpp.config.time!="")
            logger.info("⭐发行时间: ${cpp.config.time}")
        logger.info("⭐已成功启动MiraiCP⭐")
        PublicShared.logger4plugins[cpp.config.name] = MiraiLogger.create(cpp.config.name)
        GlobalEventChannel.parentScope(this).subscribeAlways<BotOnlineEvent> {
            cpp.Event(
                PublicShared.gson.toJson(Config.BotOnline(this.bot.id))
            )
        }
        PublicShared.onEnable(GlobalEventChannel.parentScope(this))
    }

    override fun onDisable() {
        PublicShared.onDisable()
    }
}
