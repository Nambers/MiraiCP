package org.example.mirai.plugin

import com.google.gson.Gson
import net.mamoe.mirai.Bot
import net.mamoe.mirai.console.plugin.jvm.JvmPluginDescription
import net.mamoe.mirai.console.plugin.jvm.KotlinPlugin
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.event.globalEventChannel
import java.io.File

/*
��src/main/resources/plugin.yml��Ĳ����Ϣ����ڵ�
��settings.gradle.kts������ɵĲ��.jar����
��runmiraikt������ÿ�����ide�����У����ø��Ƶ�mcl������������
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
        //�������
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
        logger.info("github�洢��:https://github.com/Nambers/MiraiCP")
        if(!File("${dataFolder.absoluteFile}/$dll_name").exists()){
            logger.error("�ļ�${dataFolder.absoluteFile}/$dll_name ������")
        }
        val cpp = CPP_lib()
        val gson = Gson()
        logger.info(cpp.ver)//���2333 ����
        globalEventChannel().subscribeAlways<BotOnlineEvent> {
            AIbot = this.bot
        }
        //�����ļ�Ŀ¼ "${dataFolder.absolutePath}/"
        globalEventChannel().subscribeAlways<GroupMessageEvent> {
            cpp.Event(gson.toJson(
                Config.GroupMessage(
                    this.group.id,
                    this.sender.id,
                    this.message.contentToString())
                ))
        }
        globalEventChannel().subscribeAlways<FriendMessageEvent>{
            //������Ϣ
            val temp = gson.toJson(
                Config.PrivateMessage(
                    this.sender.id,
                    this.message.contentToString())
            )
            logger.info(temp)
            cpp.Event(temp)
        }
        globalEventChannel().subscribeAlways<NewFriendRequestEvent>{
            //�Զ�ͬ���������
            accept()
        }
        globalEventChannel().subscribeAlways<BotInvitedJoinGroupRequestEvent>{
            //�Զ�ͬ���Ⱥ����
            accept()
        }
    }
}
