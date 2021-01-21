package org.example.mirai.plugin

import com.google.gson.Gson
import net.mamoe.mirai.Bot
import net.mamoe.mirai.console.plugin.jvm.JvmPluginDescription
import net.mamoe.mirai.console.plugin.jvm.KotlinPlugin
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.event.globalEventChannel
import java.io.File

/*
在src/main/resources/plugin.yml里改插件信息和入口点
在settings.gradle.kts里改生成的插件.jar名称
用runmiraikt这个配置可以在ide里运行，不用复制到mcl或其他启动器
 */

object PluginMain : KotlinPlugin(
    JvmPluginDescription(
        id = "org.example.miraiCP",
        version = "0.1.0"
    )
) {
    val dll_name = "mirai-demo.dll"
    private lateinit var AIbot: Bot
    suspend fun Send(message: String, id: Long) {
        //反向调用
        logger.info("Send message for $id is $message")
        AIbot.getFriend(id)?.sendMessage(message)
    }

    fun BasicSendLog(log: String) {
        logger.info(log);
    }

    suspend fun SendG(message: String, id: Long) {
        logger.info("Send message for Group($id) is $message")
        AIbot.getGroup(id)?.sendMessage(message)
    }

    fun GetN(qqid: Long): String? {
        return AIbot.getFriend(qqid)?.nick
    }
    override fun onEnable() {

        logger.info("Plugin loaded!")
        logger.info("github存储库:https://github.com/Nambers/MiraiCP")
        if(!File("${dataFolder.absoluteFile}/$dll_name").exists()){
            logger.error("文件${dataFolder.absoluteFile}/$dll_name 不存在")
        }
        val cpp = CPP_lib()
        val gson = Gson()
        logger.info(cpp.ver)//输出2333 正常
        globalEventChannel().subscribeAlways<BotOnlineEvent> {
            AIbot = this.bot
        }
        //配置文件目录 "${dataFolder.absolutePath}/"
        globalEventChannel().subscribeAlways<GroupMessageEvent> {
            cpp.Event(gson.toJson(
                Config.GroupMessage(
                    this.group.id,
                    this.sender.id,
                    this.message.contentToString())
                ))
        }
        globalEventChannel().subscribeAlways<FriendMessageEvent>{
            //好友信息
            val temp = gson.toJson(
                Config.PrivateMessage(
                    this.sender.id,
                    this.message.contentToString())
            )
            logger.info(temp)
            cpp.Event(temp)
        }
        globalEventChannel().subscribeAlways<NewFriendRequestEvent>{
            //自动同意好友申请
            accept()
        }
        globalEventChannel().subscribeAlways<BotInvitedJoinGroupRequestEvent>{
            //自动同意加群申请
            accept()
        }
    }
}
