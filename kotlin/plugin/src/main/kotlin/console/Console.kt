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

package tech.eritquearcus.miraicp.console

import net.mamoe.mirai.console.command.CommandManager
import net.mamoe.mirai.console.command.SimpleCommand
import net.mamoe.mirai.utils.MiraiLogger
import tech.eritquearcus.miraicp.PluginMain
import tech.eritquearcus.miraicp.shared.CPP_lib
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.PublicShared.logger
import java.io.File

object PluginList : SimpleCommand(
    PluginMain,
    "pluginList", "plist",
    description = "打印已经被加载的全部MiraiCP插件"
) {
    @Handler
    fun pluginList() {
        PublicShared.cpp.forEach {
            it.showInfo()
        }
    }
}

object DisablePlugin : SimpleCommand(
    PluginMain,
    "disablePlugin", "disable",
    description = "禁用一个已经加载的MiraiCP插件[可被enable指令启用](简写: disable)"
) {
    @Handler
    fun disable(name: String) {
        try {
            PublicShared.cpp.first { it.config.name == name }
            (!PublicShared.disablePlugins.contains(name)) && PublicShared.disablePlugins.add(name)
            logger.info("禁用${name}成功")
        } catch (e: NoSuchElementException) {
            logger.error("找不到${name}插件, 关闭失败")
        }
    }
}

object EnablePlugin : SimpleCommand(
    PluginMain,
    "enablePlugin", "enable",
    description = "启用一个被加载的MiraiCP插件[可被disable指令禁用](简写: enable)"
) {
    @Handler
    fun enable(name: String) {
        if (PublicShared.disablePlugins.contains(name)) {
            PublicShared.disablePlugins.remove(name)
            logger.info("启用${name}成功")
        } else {
            logger.warning("未在禁用插件列表中找到 $name")
        }
    }
}

object DisablePluginList : SimpleCommand(
    PluginMain,
    "disablePluginList", "dList",
    description = "打印被禁用的MiraiCP插件列表(简写: dList)"
) {
    @Handler
    fun dList() {
        PublicShared.disablePlugins.forEach {
            logger.info(it)
        }
    }
}

object LoadPlugin : SimpleCommand(
    PluginMain,
    "loadPlugin", "load",
    description = "加载一个未被加载过的MiraiCP插件(简写: load)"
) {
    @Handler
    fun load(path: String) {
        val f = File(path)
        when {
            !f.isFile || !f.exists() -> {
                logger.error("${f.absolutePath} 不是一个有效的文件")
            }
            f.extension != "dll" && f.extension != "so" -> {
                logger.error("${f.absolutePath} 不是一个有效的dll或so文件")
            }
            PublicShared.loadedPlugins.contains(f.absolutePath) -> {
                logger.error("已经加载该插件")
            }
            else -> {
                CPP_lib(f.absolutePath, emptyList()).let { cpp ->
                    cpp.showInfo()
                    PublicShared.logger4plugins[cpp.config.name] =
                        MiraiLogger.Factory.create(this::class, cpp.config.name)
                    PublicShared.cpp.add(cpp)
                }
                logger.info("加载${f.absolutePath}成功")
            }
        }
    }
}

fun registerCommands() {
    CommandManager.registerCommand(PluginList)
    CommandManager.registerCommand(DisablePlugin)
    CommandManager.registerCommand(EnablePlugin)
    CommandManager.registerCommand(DisablePluginList)
    CommandManager.registerCommand(LoadPlugin)
}
