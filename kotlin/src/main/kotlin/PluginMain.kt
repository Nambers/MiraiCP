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
    lateinit var cpp: CPP_lib
    suspend fun Send(message: String, id: Long) {
        //反向调用
        logger.info("Send message for($id) is $message")
        AIbot.getFriend(id)?.sendMessage(message)
    }
    suspend fun Send(message: String, id: Long, gid: Long) {
        //反向调用
        logger.info("Send message for a member($id) is $message")
        AIbot.getGroup(gid)?.get(id)?.sendMessage(message)
    }

    fun BasicSendLog(log: String) {
        logger.info(log)
    }

    fun SendWarning(log: String){
        logger.warning(log)
    }

    fun SendError(log: String){
        logger.error(log)
    }

    suspend fun SendG(message: String, id: Long) {
        logger.info("Send message for Group($id) is $message")
        AIbot.getGroup(id)?.sendMessage(message)
    }

    fun GetNN(qqid: Long, groupid: Long): String {
        val group = AIbot.getGroup(groupid) ?: return ""
        val member = group[qqid] ?: return ""
        return member.nameCard
    }

    fun GetN(qqid: Long): String {
        return AIbot.getFriend(qqid)?.nick ?: return ""
    }

    override fun onDisable() {
        super.onDisable()
        cpp.PluginDisable()
    }
    override fun onEnable() {
        super.onEnable()
        logger.info("Plugin loaded!")
        logger.info("github存储库:https://github.com/Nambers/MiraiCP")
        if(!File("${dataFolder.absoluteFile}/$dll_name").exists()){
            logger.error("文件${dataFolder.absoluteFile}/$dll_name 不存在")
        }
        cpp = CPP_lib()
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
            cpp.Event(gson.toJson(
                Config.PrivateMessage(
                    this.sender.id,
                    this.message.contentToString())
            ))
        }
        globalEventChannel().subscribeAlways<NewFriendRequestEvent>{
            //自动同意好友申请
            val r = cpp.Event(gson.toJson(
                Config.NewFriendRequest(
                    this.fromId,
                    this.message
                )))
            when(r) {
                "true" -> accept()
                "false" -> reject()
                else -> {
                    logger.error("NewFriendRequestEvent unknown return")
                }
            }
        }
        globalEventChannel().subscribeAlways<BotInvitedJoinGroupRequestEvent>{
            //自动同意加群申请
            val r = cpp.Event(gson.toJson(
                Config.GroupInvite(
                    this.groupId,
                    this.invitorId
                )))
            when(r) {
                "true" -> accept()
                "false" -> ignore()
                else -> {
                    logger.error("BotInvitedJoinGroupRequestEvent unknown return")
                }
            }
        }
    }
}
