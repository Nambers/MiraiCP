package tech.eritquearcus.miraicp

import net.mamoe.mirai.BotFactory
import net.mamoe.mirai.alsoLogin
import net.mamoe.mirai.console.plugin.jvm.JvmPluginDescription
import net.mamoe.mirai.console.plugin.jvm.KotlinPlugin
import net.mamoe.mirai.event.GlobalEventChannel
import net.mamoe.mirai.event.events.BotOnlineEvent
import net.mamoe.mirai.utils.MiraiLogger
import tech.eritquearcus.miraicp.shared.CPP_lib
import tech.eritquearcus.miraicp.shared.Config
import tech.eritquearcus.miraicp.shared.publicShared
import java.io.File

object PluginMain : KotlinPlugin(
    JvmPluginDescription(
        id = "tech.eritquearcus.miraiCP",
        name = "miraiCP",
        version = "2.6.5"
    ){
        author("Eritque arcus")
    }
) {
    override fun onEnable() {
        val l = MiraiLogger.create("MiraiCP")
        l.info("启动成功")
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
        }
        publicShared.init(l, dll_name)
        val cpp = CPP_lib()
        GlobalEventChannel.parentScope(this).subscribeAlways<BotOnlineEvent> {
            cpp.Event(
                publicShared.gson.toJson(Config.BotOnline(this.bot.id))
            )
        }
        publicShared.onEnable(GlobalEventChannel.parentScope(this), cpp)
    }

    override fun onDisable() {
        publicShared.onDisable()
    }
}
