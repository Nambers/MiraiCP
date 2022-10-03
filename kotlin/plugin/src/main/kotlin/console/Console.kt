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

@file:Suppress("UnusedReceiverParameter")

package tech.eritquearcus.miraicp.console

import net.mamoe.mirai.console.command.CommandManager
import net.mamoe.mirai.console.command.ConsoleCommandSender
import net.mamoe.mirai.console.command.SimpleCommand
import tech.eritquearcus.miraicp.PluginMain
import tech.eritquearcus.miraicp.shared.CPPEvent
import tech.eritquearcus.miraicp.shared.UlitsMultiPlatform.event

object PluginList : SimpleCommand(
    PluginMain,
    "pluginList", "plist",
    description = "打印已经被加载的全部MiraiCP插件"
) {
    @Handler
    fun ConsoleCommandSender.pluginList() {
        event(CPPEvent.LibLoaderEvent("PluginList"))
    }
}

object DisablePlugin : SimpleCommand(
    PluginMain,
    "disablePlugin", "disable",
    description = "禁用一个已经加载的MiraiCP插件[可被enable指令启用](简写: disable)"
) {
    @Handler
    fun ConsoleCommandSender.disable(id: String) {
        event(CPPEvent.LibLoaderEvent("DisablePlugin", id))
//        try {
//            PublicShared.cpp.first { it.config.name == name }
//            (!PublicShared.disablePlugins.contains(name)) && PublicShared.disablePlugins.add(name)
//            sendMessage("禁用${name}成功")
//        } catch (e: NoSuchElementException) {
//            sendMessage("找不到${name}插件, 关闭失败")
//        }
    }
}

object EnablePlugin : SimpleCommand(
    PluginMain,
    "enablePlugin", "enable",
    description = "启用一个被加载的MiraiCP插件[可被disable指令禁用](简写: enable)"
) {
    @Handler
    fun ConsoleCommandSender.enable(id: String) {
        event(CPPEvent.LibLoaderEvent("EnablePlugin", id))
//        if (PublicShared.disablePlugins.contains(name)) {
//            PublicShared.disablePlugins.remove(name)
//            sendMessage("启用${name}成功")
//        } else {
//            sendMessage("未在禁用插件列表中找到 $name")
//        }
    }
}

object DisablePluginList : SimpleCommand(
    PluginMain,
    "disablePluginList", "dList",
    description = "打印被禁用的MiraiCP插件列表(简写: dList)"
) {
    @Handler
    fun ConsoleCommandSender.dList() {
        event(CPPEvent.LibLoaderEvent("DisablePluginList"))
//        PublicShared.disablePlugins.forEach {
//            sendMessage(it)
//        }
    }
}

object LoadPlugin : SimpleCommand(
    PluginMain,
    "loadPlugin", "load",
    description = "加载一个未被加载过的MiraiCP插件(简写: load)"
) {
    @Handler
    fun ConsoleCommandSender.load(path: String) {
        event(CPPEvent.LibLoaderEvent("LoadPlugin", path))
//        val f = File(path)
//        when {
//            !f.isFile || !f.exists() -> {
//                sendMessage("Err:${f.absolutePath} 不是一个有效的文件")
//            }
//            else -> {
//                f.loadAsCPPLib(emptyList())
//            }
//        }
    }
}

object UnloadPlugin : SimpleCommand(
    PluginMain,
    "unloadPlugin", "unload",
    description = "加载一个未被加载过的MiraiCP插件(简写: unload)"
) {
    @Handler
    fun ConsoleCommandSender.load(id: String) {
        event(CPPEvent.LibLoaderEvent("UnloadPlugin", id))
    }
}

object ReloadPlugin : SimpleCommand(
    PluginMain,
    "reloadPlugin", "reload",
    description = "重新加载一个未被加载过的MiraiCP插件(简写: reload)"
) {
    @Handler
    fun ConsoleCommandSender.load(id: String) {
        event(CPPEvent.LibLoaderEvent("ReloadPlugin", id))
    }
}

fun registerCommands() {
    CommandManager.registerCommand(PluginList)
    CommandManager.registerCommand(DisablePlugin)
    CommandManager.registerCommand(EnablePlugin)
    CommandManager.registerCommand(DisablePluginList)
    CommandManager.registerCommand(LoadPlugin)
    CommandManager.registerCommand(UnloadPlugin)
    CommandManager.registerCommand(ReloadPlugin)
}
