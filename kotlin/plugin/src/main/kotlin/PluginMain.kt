package tech.eritquearcus.miraicp

import net.mamoe.mirai.console.plugin.jvm.JvmPluginDescription
import net.mamoe.mirai.console.plugin.jvm.KotlinPlugin
import net.mamoe.mirai.event.GlobalEventChannel
import net.mamoe.mirai.event.globalEventChannel
import net.mamoe.mirai.utils.MiraiExperimentalApi
import net.mamoe.mirai.utils.MiraiInternalApi
import tech.eritquearcus.miraicp.shared.publicShared
import java.io.File

object PluginMain : KotlinPlugin(
    JvmPluginDescription(
        id = "tech.eritquearcus.miraiCP",
        name = "miraiCP",
        version = "2.6.3"
    )
) {
    @MiraiExperimentalApi
    @MiraiInternalApi
    override fun onEnable() {
        logger.info("启动成功")
        logger.info("本项目github存储库:https://github.com/Nambers/MiraiCP")
        var dll_name = "cpp.dll"
        dll_name = "${dataFolder.absoluteFile}\\$dll_name"
        val Configuration = "${dataFolder.absoluteFile}\\miraicp.txt"
        if (!File(Configuration).exists() || !File(Configuration).isFile) {
            logger.error("配置文件$Configuration 不存在，使用默认dll路径$dll_name")
        } else {
            val c = File(Configuration).readLines()
            if (c.size != 1 || !File(c[0]).exists() || !File(c[0]).isFile || File(c[0]).extension != "dll") {
                logger.error("配置文件$Configuration 格式错误，应只包含一行且为一个已存在的dll文件，使用默认dll路径$dll_name")
            } else {
                dll_name = c[0]
            }
        }
        if (!File(dll_name).exists()) {
            logger.error("c++文件$dll_name 不存在")
        }
        publicShared.init(logger, dll_name)
        publicShared.onEnable(GlobalEventChannel.parentScope(this))
    }

    override fun onDisable() {
        publicShared.onDisable()
    }
}
