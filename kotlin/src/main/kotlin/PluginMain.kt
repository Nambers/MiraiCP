package org.example.mirai.plugin

import com.google.gson.Gson
import net.mamoe.mirai.Bot
import net.mamoe.mirai.Mirai
import net.mamoe.mirai.console.plugin.jvm.JvmPluginDescription
import net.mamoe.mirai.console.plugin.jvm.KotlinPlugin
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.event.globalEventChannel
import net.mamoe.mirai.message.code.MiraiCode
import net.mamoe.mirai.message.data.Image
import net.mamoe.mirai.message.data.Image.Key.queryUrl
import net.mamoe.mirai.message.data.MessageChain.Companion.serializeToJsonString
import java.io.File

/*
��src/main/resources/plugin.yml��Ĳ����Ϣ����ڵ�
��settings.gradle.kts������ɵĲ��.jar����
��runmiraikt������ÿ�����ide�����У����ø��Ƶ�mcl������������
 */

object PluginMain : KotlinPlugin(
    JvmPluginDescription(
        id = "org.example.miraiCP",
        version = "1.0.2"
    )
) {
    val dll_name = "mirai-demo.dll"
    private lateinit var AIbot: Bot
    lateinit var cpp: CPP_lib
    suspend fun Send(message: String, id: Long) {
        //�������
        logger.info("Send message for($id) is $message")
        AIbot.getFriend(id)?.sendMessage(MiraiCode.deserializeMiraiCode(message))
    }
    suspend fun Send(message: String, id: Long, gid: Long) {
        //�������
        logger.info("Send message for a member($id) is $message")
        AIbot.getGroup(gid)?.get(id)?.sendMessage(MiraiCode.deserializeMiraiCode(message))
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
        AIbot.getGroup(id)?.sendMessage(MiraiCode.deserializeMiraiCode(message))
    }

    fun GetNN(qqid: Long, groupid: Long): String {
        val group = AIbot.getGroup(groupid) ?: return ""
        val member = group[qqid] ?: return ""
        return member.nameCard
    }

    fun GetN(qqid: Long): String {
        return AIbot.getFriend(qqid)?.nick ?: return ""
    }

    suspend fun QueryImg(id: String): String{
        return Image(id).queryUrl()
    }

    override fun onDisable() {
        super.onDisable()
        cpp.PluginDisable()
    }
    override fun onEnable() {
        super.onEnable()
        logger.info("Plugin loaded!")
        logger.info("github�洢��:https://github.com/Nambers/MiraiCP")
        if(!File("${dataFolder.absoluteFile}/$dll_name").exists()){
            logger.error("�ļ�${dataFolder.absoluteFile}/$dll_name ������")
        }
        cpp = CPP_lib()
        val gson = Gson()
        logger.info(cpp.ver)//���2333 ����
        globalEventChannel().subscribeAlways<BotOnlineEvent> {
            AIbot = this.bot
        }
        //�����ļ�Ŀ¼ "${dataFolder.absolutePath}/"
        globalEventChannel().subscribeAlways<GroupMessageEvent> {
            //Ⱥ��Ϣ
            cpp.Event(gson.toJson(
                Config.GroupMessage(
                    this.group.id,
                    this.sender.id,
                    this.message.serializeToMiraiCode())
                ))
        }
        globalEventChannel().subscribeAlways<FriendMessageEvent>{
            //������Ϣ
            cpp.Event(gson.toJson(
                Config.PrivateMessage(
                    this.sender.id,
                    this.message.serializeToMiraiCode())
            ))
        }
        globalEventChannel().subscribeAlways<NewFriendRequestEvent>{
            //�Զ�ͬ���������
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
                    reject()
                }
            }
        }
        globalEventChannel().subscribeAlways<BotInvitedJoinGroupRequestEvent>{
            //�Զ�ͬ���Ⱥ����
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
                    ignore()
                }
            }
        }
    }
}
