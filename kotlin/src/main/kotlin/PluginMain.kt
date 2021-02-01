package org.example.mirai.plugin

import com.google.gson.Gson
import net.mamoe.mirai.Bot
import net.mamoe.mirai.console.plugin.jvm.JvmPluginDescription
import net.mamoe.mirai.console.plugin.jvm.KotlinPlugin
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.event.globalEventChannel
import net.mamoe.mirai.message.code.MiraiCode
import net.mamoe.mirai.message.data.Image
import net.mamoe.mirai.message.data.Image.Key.queryUrl
import net.mamoe.mirai.utils.ExternalResource.Companion.uploadAsImage
import net.mamoe.mirai.utils.OverFileSizeMaxException
import java.io.File

/*
��src/main/resources/plugin.yml��Ĳ����Ϣ����ڵ�
��settings.gradle.kts������ɵĲ��.jar����
��runmiraikt������ÿ�����ide�����У����ø��Ƶ�mcl������������
 */


object PluginMain : KotlinPlugin(
    JvmPluginDescription(
        id = "org.example.miraiCP",
        name = "miraiCP",
        version = "2.2.0"
    )
) {
    val dll_name = "mirai-demo.dll"
    private lateinit var AIbot: Bot
    lateinit var cpp: CPP_lib
    suspend fun Send(message: String, id: Long) {
        //�������
        logger.info("Send message for($id) is $message")
        val f = AIbot.getFriend(id)?:let {
            logger.error("������Ϣ�Ҳ������ѣ�λ��:K-Send()��id:$id")
            return
        }
        f.sendMessage(MiraiCode.deserializeMiraiCode(message))
    }
    suspend fun Send(message: String, id: Long, gid: Long) {
        //�������
        logger.info("Send message for a member($id) is $message")
        val G = AIbot.getGroup(gid)?:let{
            logger.error("������Ϣ�Ҳ���Ⱥ�ģ�λ��K-Send()��id:$gid")
            return
        }
        val f = G[id]?:let{
            logger.error("������Ϣ�Ҳ���Ⱥ��Ա��λ��K-Send()��id:$id��gid:$gid")
            return
        }
        f.sendMessage(MiraiCode.deserializeMiraiCode(message))
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
        val g = AIbot.getGroup(id)?:let {
            logger.error("����Ⱥ��Ϣ�쳣�Ҳ���Ⱥ�飬λ��K-SendG��gid:$id")
            return
        }
        g.sendMessage(MiraiCode.deserializeMiraiCode(message))
    }

    fun GetNN(qqid: Long, groupid: Long): String {
        val group = AIbot.getGroup(groupid) ?: let{
            logger.error("ȡȺ��Ƭ�Ҳ�����ӦȺ�飬λ��K-GetNN()��gid:$groupid")
            return ""
        }
        val member = group[qqid] ?: let {
            logger.error("ȡȺ��Ƭ�Ҳ�����ӦȺ��Ա��λ��K-GetNN()��id:$qqid, gid:$groupid")
            return ""
        }
        return member.nameCard

    }
    suspend fun uploadImgFriend(id: Long, file: String): String {
        val temp = AIbot.getFriend(id)?:let{
            logger.error("����ͼƬ�Ҳ�����Ӧ����,λ��:K-uploadImgFriend(),id:$id")
            return ""
        }
        return try {
            File(file).uploadAsImage(temp).imageId
        }catch (e: OverFileSizeMaxException) {
            logger.error("ͼƬ�ļ����󳬹�30MB,λ��:K-uploadImgGroup(),�ļ���:$file")
            ""
        }catch (e:NullPointerException){
            logger.error("�ϴ�ͼƬ�ļ����쳣,λ��:K-uploadImgGroup(),�ļ���:$file")
            ""
        }
    }
    suspend fun uploadImgGroup(id: Long, file: String): String {
        val temp = AIbot.getGroup(id)?:let{
            logger.error("����ͼƬ�Ҳ�����ӦȺ��,λ��:K-uploadImgGroup(),id:$id")
            return ""
        }
        return try {
            File(file).uploadAsImage(temp).imageId
        }catch (e: OverFileSizeMaxException) {
            logger.error("ͼƬ�ļ����󳬹�30MB,λ��:K-uploadImgGroup(),�ļ���:$file")
            ""
        }catch (e:NullPointerException){
            logger.error("�ϴ�ͼƬ�ļ����쳣,λ��:K-uploadImgGroup(),�ļ���:$file")
            ""
        }
    }
    suspend fun uploadImgMember(id: Long,qqid: Long, file: String): String {
        val temp = AIbot.getGroup(id)?:let{
            logger.error("����ͼƬ�Ҳ�����ӦȺ��,λ��:K-uploadImgGroup(),id:$id")
            return ""
        }
        val temp1 = temp[qqid]?:let{
            logger.error("����ͼƬ�Ҳ���Ŀ���Ա,λ��:K-uploadImgMember(),��Աid:$qqid,Ⱥ��id:$id")
            return ""
        }
        return try {
            File(file).uploadAsImage(temp1).imageId
        }catch (e: OverFileSizeMaxException) {
            logger.error("ͼƬ�ļ����󳬹�30MB,λ��:K-uploadImgGroup(),�ļ���:$file")
            ""
        }catch (e:NullPointerException){
            logger.error("�ϴ�ͼƬ�ļ����쳣,λ��:K-uploadImgGroup(),�ļ���:$file")
            ""
        }
    }
    fun GetN(qqid: Long): String {
        val f = AIbot.getFriend(qqid) ?: let {
            logger.error("�Ҳ�����Ӧ���ѵ��ǳƣ�λ��:K-GetN()��id:$qqid")
            return ""
        }
        return f.nick
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
        globalEventChannel().subscribeAlways<MemberLeaveEvent.Kick> {
            cpp.Event(gson.toJson(
                Config.MemberLeave(
                    this.group.id,
                    this.member.id,
                    1,
                    if(this.operator?.id == null) this.bot.id else this.operator!!.id
                )
            ))
        }
        globalEventChannel().subscribeAlways<MemberLeaveEvent.Quit> {
            cpp.Event(gson.toJson(
                Config.MemberLeave(
                    this.group.id,
                    this.member.id,
                    2,
                    this.member.id
                )
            ))
        }
        globalEventChannel().subscribeAlways<MemberJoinEvent.Retrieve> {
            cpp.Event(gson.toJson(
                Config.MemberJoin(
                    this.group.id,
                    this.member.id,
                    3,
                    this.member.id
                )
            ))
        }
        globalEventChannel().subscribeAlways<MemberJoinEvent.Active> {
            cpp.Event(gson.toJson(
                Config.MemberJoin(
                    this.group.id,
                    this.member.id,
                    2,
                    this.member.id
                )
            ))
        }
        globalEventChannel().subscribeAlways<MemberJoinEvent.Invite> {
            cpp.Event(gson.toJson(
                Config.MemberJoin(
                    this.group.id,
                    this.member.id,
                    1,
                    this.invitor.id
                )
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
