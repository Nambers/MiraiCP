package org.example.mirai.plugin

import com.google.gson.Gson
import net.mamoe.mirai.Bot
import net.mamoe.mirai.console.plugin.jvm.JvmPluginDescription
import net.mamoe.mirai.console.plugin.jvm.KotlinPlugin
import net.mamoe.mirai.contact.NormalMember
import net.mamoe.mirai.contact.PermissionDeniedException
import net.mamoe.mirai.contact.nameCardOrNick
import net.mamoe.mirai.event.Event
import net.mamoe.mirai.event.EventChannel
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.event.globalEventChannel
import net.mamoe.mirai.message.code.MiraiCode
import net.mamoe.mirai.message.data.Image
import net.mamoe.mirai.message.data.Image.Key.queryUrl
import net.mamoe.mirai.utils.ExternalResource.Companion.uploadAsImage
import net.mamoe.mirai.utils.OverFileSizeMaxException
import java.io.File
import java.util.*
import kotlin.concurrent.schedule

/*
��settings.gradle.kts������ɵĲ��.jar����
��runmiraikt������ÿ�����ide�����У����ø��Ƶ�mcl������������
 */


object PluginMain : KotlinPlugin(
    JvmPluginDescription(
        id = "org.example.miraiCP",
        name = "miraiCP",
        version = "2.4.1"
    )
) {
    var friend_cache = ArrayList<NormalMember>(0)
    var dll_name = "mirai-demo.dll"
    lateinit var AIbot: Bot
    lateinit var cpp: CPP_lib
    lateinit var gson: Gson

    //��־����ʵ��
    fun BasicSendLog(log: String) {
        logger.info(log)
    }
    fun SendWarning(log: String){
        logger.warning(log)
    }
    fun SendError(log: String){
        logger.error(log)
    }

    //������Ϣ����ʵ��
    suspend fun Send(message: String, id: Long) :String{
        //�������
        logger.info("Send message for($id) is $message")
        val f = AIbot.getFriend(id) ?: let {
            logger.error("������Ϣ�Ҳ������ѣ�λ��:K-Send()��id:$id")
            return "E1"
        }
        f.sendMessage(MiraiCode.deserializeMiraiCode(message))
        return "Y"
    }

    suspend fun Send(message: String, id: Long, gid: Long):String {
        //�������
        logger.info("Send message for a member($id) is $message")
        for (a in friend_cache) {
            if (a.id == id && a.group.id == gid) {
                a.sendMessage(message)
                return "Y"
            }
        }
        val G = AIbot.getGroup(gid) ?: let {
            logger.error("������Ϣ�Ҳ���Ⱥ�ģ�λ��K-Send()��id:$gid")
            return "E1"
        }
        val f = G[id] ?: let {
            logger.error("������Ϣ�Ҳ���Ⱥ��Ա��λ��K-Send()��id:$id��gid:$gid")
            return "E2"
        }
        f.sendMessage(MiraiCode.deserializeMiraiCode(message))
        return "Y"
    }

    suspend fun SendG(message: String, id: Long):String {
        logger.info("Send message for Group($id) is $message")
        val g = AIbot.getGroup(id) ?: let {
            logger.error("����Ⱥ��Ϣ�쳣�Ҳ���Ⱥ�飬λ��K-SendG��gid:$id")
            return "E1"
        }
        g.sendMessage(MiraiCode.deserializeMiraiCode(message))
        return "Y"
    }

    //ȡ�ǳƻ���Ƭ����
    fun GetN(qqid: Long): String {
        val f = AIbot.getFriend(qqid) ?: let {
            logger.error("�Ҳ�����Ӧ���ѵ��ǳƣ�λ��:K-GetN()��id:$qqid")
            return ""
        }
        return f.nick
    }
    fun GetNN(qqid: Long, groupid: Long): String {
        for(a in friend_cache){
            if(a.id == qqid && a.group.id == groupid){
                return a.nameCardOrNick
            }
        }

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

    //ͼƬ����ʵ��
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
    suspend fun QueryImg(id: String): String{
        return Image(id).queryUrl()
    }

    //��ʱ����
    fun scheduling(time: Long, id: Int){
        Timer("SettingUp", false).schedule(time) {
            cpp.ScheduleTask(id)
        }
    }

    //����
    suspend fun mute(qqid: Long, groupid: Long, time:Int):String{
        val group = AIbot.getGroup(groupid) ?: let{
            logger.error("�����Ҳ�����ӦȺ�飬λ��K-mute()��gid:$groupid")
            return "E1"
        }
        val member = group[qqid] ?: let {
            logger.error("�����Ҳ�����ӦȺ��Ա��λ��K-mute()��id:$qqid, gid:$groupid")
            return "E2"
        }
        try {
            member.mute(time)
        }catch (e: PermissionDeniedException){
            logger.error("ִ�н���ʧ�ܻ�������Ȩ�ޣ�λ��:K-mute()��Ŀ��Ⱥid:$groupid��Ŀ���Աid:$qqid")
            return "E3"
        }catch (e:IllegalStateException){
            logger.error("ִ�н���ʧ�ܽ���ʱ�䳬��0s~30d��λ��:K-mute()��ʱ��:$time")
            return "E4"
        }
        return "Y"
    }

    //��ѯȨ��
    fun kqueryM(qqid: Long, groupid: Long): String{
        val group = AIbot.getGroup(groupid) ?: let {
            logger.error("��ѯȨ���Ҳ�����ӦȺ�飬λ��K-queryM()��gid:$groupid")
            return "E1"
        }
        val member = group[qqid] ?: let {
            logger.error("��ѯȨ���Ҳ�����ӦȺ��Ա��λ��K-queryM()��id:$qqid, gid:$groupid")
            return "E2"
        }
        return member.permission.level.toString()
    }

    override fun onDisable() {
        cpp.PluginDisable()
    }
    override fun onEnable(){
        val now_tag = "v2.4.1"
        println("��ǰMiraiCP��ܰ汾:$now_tag")
        logger.info("�����ɹ�")
        logger.info("����Ŀgithub�洢��:https://github.com/Nambers/MiraiCP")
        dll_name = "${dataFolder.absoluteFile}/$dll_name"
        if(!File(dll_name).exists()){
            logger.error("c++�ļ�$dll_name ������")
        }
        val ec = globalEventChannel()
        cpp = CPP_lib()
        if(cpp.ver != now_tag){
            logger.error("����:��ǰMiraiCP��ܰ汾($now_tag)��ת�ص�C++ SDK(${cpp.ver})��һ��")
        }
        gson = Gson()
        ec.subscribeAlways<BotOnlineEvent> {
            AIbot = this.bot
        }
        //�����ļ�Ŀ¼ "${dataFolder.absolutePath}/"
        ec.subscribeAlways<GroupMessageEvent> {
            //Ⱥ��Ϣ
            cpp.Event(gson.toJson(
                Config.GroupMessage(
                    this.group.id,
                    this.sender.id,
                    this.message.serializeToMiraiCode())
            ))
        }
        ec.subscribeAlways<MemberLeaveEvent.Kick> {
            friend_cache.add(this.member)
            cpp.Event(gson.toJson(
                Config.MemberLeave(
                    this.group.id,
                    this.member.id,
                    1,
                    if(this.operator?.id == null) this.bot.id else this.operator!!.id
                )
            ))
            friend_cache.remove(this.member)
        }
        ec.subscribeAlways<MemberLeaveEvent.Quit> {
            friend_cache.add(this.member)
            cpp.Event(gson.toJson(
                Config.MemberLeave(
                    this.group.id,
                    this.member.id,
                    2,
                    this.member.id
                )
            ))
            friend_cache.remove(this.member)
        }
        ec.subscribeAlways<MemberJoinEvent.Retrieve> {
            cpp.Event(gson.toJson(
                Config.MemberJoin(
                    this.group.id,
                    this.member.id,
                    3,
                    this.member.id
                )
            ))
        }
        ec.subscribeAlways<MemberJoinEvent.Active> {
            cpp.Event(gson.toJson(
                Config.MemberJoin(
                    this.group.id,
                    this.member.id,
                    2,
                    this.member.id
                )
            ))
        }
        ec.subscribeAlways<MemberJoinEvent.Invite> {
            cpp.Event(gson.toJson(
                Config.MemberJoin(
                    this.group.id,
                    this.member.id,
                    1,
                    this.invitor.id
                )
            ))
        }
        ec.subscribeAlways<FriendMessageEvent>{
            //������Ϣ
            cpp.Event(gson.toJson(
                Config.PrivateMessage(
                    this.sender.id,
                    this.message.serializeToMiraiCode())
            ))
        }
        ec.subscribeAlways<NewFriendRequestEvent>{
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
        ec.subscribeAlways<BotInvitedJoinGroupRequestEvent>{
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
