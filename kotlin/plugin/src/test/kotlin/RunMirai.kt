package tech.eritquearcus.miraicp.plugin

import net.mamoe.mirai.alsoLogin
import net.mamoe.mirai.console.MiraiConsole
import net.mamoe.mirai.console.plugin.PluginManager.INSTANCE.enable
import net.mamoe.mirai.console.plugin.PluginManager.INSTANCE.load
import net.mamoe.mirai.console.terminal.MiraiConsoleTerminalLoader
import tech.eritquearcus.miraicp.PluginMain
import tech.eritquearcus.miraicp.plugin.testConfig.code
import tech.eritquearcus.miraicp.plugin.testConfig.id

suspend fun main() {
    MiraiConsoleTerminalLoader.startAsDaemon()

    PluginMain.load()
    PluginMain.enable()

    val bot = MiraiConsole.addBot(id, code) {
        fileBasedDeviceInfo()
    }.alsoLogin()

    MiraiConsole.job.join()
}
