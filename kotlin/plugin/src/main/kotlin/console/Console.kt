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
import tech.eritquearcus.miraicp.PluginMain
import tech.eritquearcus.miraicp.shared.PublicShared
import tech.eritquearcus.miraicp.shared.PublicShared.logger
import tech.eritquearcus.miraicp.shared.loadAsCPPLib
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
            else -> {
                f.loadAsCPPLib(emptyList())
            }
        }
    }
}

object ReLoadPlugin : SimpleCommand(
    PluginMain,
    "reLoadPlugin", "reload",
    description = "重新加载一个被加载过的MiraiCP插件(简写: reload)"
) {
    @Handler
    fun reload(path: String) {
        val plugin = File(path)
        when {
            (!plugin.isFile || !plugin.exists()) -> logger.error("插件(${path})不存在")
            (plugin.extension != "dll" && plugin.extension != "so") -> logger.error("插件(${path})不是dll或so文件")
            else -> {
                val p = plugin.loadAsCPPLib(emptyList(), true)
                PublicShared.cpp.filter { it.config.id == p.config.id }.apply {
                    if (this.isEmpty())
                        logger.warning("重载未找到id为(${p.config.id}), 但会继续执行, 效果类似`load`")
                }.forEach {
                    PublicShared.cpp.remove(it)
                    PublicShared.disablePlugins.contains(it.config.id) && PublicShared.disablePlugins.remove(it.config.id)
                }
                PublicShared.cpp.add(p)
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
    CommandManager.registerCommand(ReLoadPlugin)
}
