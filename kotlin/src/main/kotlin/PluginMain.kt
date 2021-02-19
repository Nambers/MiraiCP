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
import net.mamoe.mirai.event.broadcast
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.event.globalEventChannel
import net.mamoe.mirai.message.code.MiraiCode
import net.mamoe.mirai.message.data.Image
import net.mamoe.mirai.message.data.Image.Key.queryUrl
import net.mamoe.mirai.utils.ExternalResource.Companion.uploadAsImage
import net.mamoe.mirai.utils.MiraiInternalApi
import net.mamoe.mirai.utils.OverFileSizeMaxException
import java.io.File
import java.util.*
import kotlin.concurrent.schedule

/*
在settings.gradle.kts里改生成的插件.jar名称
用runmiraikt这个配置可以在ide里运行，不用复制到mcl或其他启动器
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

    //日志部分实现
    fun BasicSendLog(log: String) {
        logger.info(log)
    }
    fun SendWarning(log: String){
        logger.warning(log)
    }
    fun SendError(log: String){
        logger.error(log)
    }

    //发送消息部分实现
    suspend fun Send(message: String, id: Long) :String{
        //反向调用
        logger.info("Send message for($id) is $message")
        val f = AIbot.getFriend(id) ?: let {
            logger.error("发送消息找不到好友，位置:K-Send()，id:$id")
            return "E1"
        }
        f.sendMessage(MiraiCode.deserializeMiraiCode(message))
        return "Y"
    }

    suspend fun Send(message: String, id: Long, gid: Long):String {
        //反向调用
        logger.info("Send message for a member($id) is $message")
        for (a in friend_cache) {
            if (a.id == id && a.group.id == gid) {
                a.sendMessage(message)
                return "Y"
            }
        }
        val G = AIbot.getGroup(gid) ?: let {
            logger.error("发送消息找不到群聊，位置K-Send()，id:$gid")
            return "E1"
        }
        val f = G[id] ?: let {
            logger.error("发送消息找不到群成员，位置K-Send()，id:$id，gid:$gid")
            return "E2"
        }
        f.sendMessage(MiraiCode.deserializeMiraiCode(message))
        return "Y"
    }

    suspend fun SendG(message: String, id: Long):String {
        logger.info("Send message for Group($id) is $message")
        val g = AIbot.getGroup(id) ?: let {
            logger.error("发送群消息异常找不到群组，位置K-SendG，gid:$id")
            return "E1"
        }
        g.sendMessage(MiraiCode.deserializeMiraiCode(message))
        return "Y"
    }

    //取昵称或名片部分
    fun GetN(qqid: Long): String {
        val f = AIbot.getFriend(qqid) ?: let {
            logger.error("找不到对应好友的昵称，位置:K-GetN()，id:$qqid")
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
            logger.error("取群名片找不到对应群组，位置K-GetNN()，gid:$groupid")
            return ""
        }
        val member = group[qqid] ?: let {
            logger.error("取群名片找不到对应群成员，位置K-GetNN()，id:$qqid, gid:$groupid")
            return ""
        }
        return member.nameCard

    }

    //图片部分实现
    suspend fun uploadImgFriend(id: Long, file: String): String {
        val temp = AIbot.getFriend(id)?:let{
            logger.error("发送图片找不到对应好友,位置:K-uploadImgFriend(),id:$id")
            return ""
        }
        return try {
            File(file).uploadAsImage(temp).imageId
        }catch (e: OverFileSizeMaxException) {
            logger.error("图片文件过大超过30MB,位置:K-uploadImgGroup(),文件名:$file")
            ""
        }catch (e:NullPointerException){
            logger.error("上传图片文件名异常,位置:K-uploadImgGroup(),文件名:$file")
            ""
        }
    }
    suspend fun uploadImgGroup(id: Long, file: String): String {
        val temp = AIbot.getGroup(id)?:let{
            logger.error("发送图片找不到对应群组,位置:K-uploadImgGroup(),id:$id")
            return ""
        }
        return try {
            File(file).uploadAsImage(temp).imageId
        }catch (e: OverFileSizeMaxException) {
            logger.error("图片文件过大超过30MB,位置:K-uploadImgGroup(),文件名:$file")
            ""
        }catch (e:NullPointerException){
            logger.error("上传图片文件名异常,位置:K-uploadImgGroup(),文件名:$file")
            ""
        }
    }
    suspend fun uploadImgMember(id: Long,qqid: Long, file: String): String {
        val temp = AIbot.getGroup(id)?:let{
            logger.error("发送图片找不到对应群组,位置:K-uploadImgGroup(),id:$id")
            return ""
        }
        val temp1 = temp[qqid]?:let{
            logger.error("发送图片找不到目标成员,位置:K-uploadImgMember(),成员id:$qqid,群聊id:$id")
            return ""
        }
        return try {
            File(file).uploadAsImage(temp1).imageId
        }catch (e: OverFileSizeMaxException) {
            logger.error("图片文件过大超过30MB,位置:K-uploadImgGroup(),文件名:$file")
            ""
        }catch (e:NullPointerException){
            logger.error("上传图片文件名异常,位置:K-uploadImgGroup(),文件名:$file")
            ""
        }
    }
    suspend fun QueryImg(id: String): String{
        return Image(id).queryUrl()
    }

    //定时任务
    fun scheduling(time: Long, id: Int){
        Timer("SettingUp", false).schedule(time) {
            cpp.ScheduleTask(id)
        }
    }

    //禁言
    suspend fun mute(qqid: Long, groupid: Long, time:Int):String{
        val group = AIbot.getGroup(groupid) ?: let{
            logger.error("禁言找不到对应群组，位置K-mute()，gid:$groupid")
            return "E1"
        }
        val member = group[qqid] ?: let {
            logger.error("禁言找不到对应群成员，位置K-mute()，id:$qqid, gid:$groupid")
            return "E2"
        }
        try {
            member.mute(time)
        }catch (e: PermissionDeniedException){
            logger.error("执行禁言失败机器人无权限，位置:K-mute()，目标群id:$groupid，目标成员id:$qqid")
            return "E3"
        }catch (e:IllegalStateException){
            logger.error("执行禁言失败禁言时间超出0s~30d，位置:K-mute()，时间:$time")
            return "E4"
        }
        return "Y"
    }

    //查询权限
    fun kqueryM(qqid: Long, groupid: Long): String{
        val group = AIbot.getGroup(groupid) ?: let {
            logger.error("查询权限找不到对应群组，位置K-queryM()，gid:$groupid")
            return "E1"
        }
        val member = group[qqid] ?: let {
            logger.error("查询权限找不到对应群成员，位置K-queryM()，id:$qqid, gid:$groupid")
            return "E2"
        }
        return member.permission.level.toString()
    }

    override fun onDisable() {
        cpp.PluginDisable()
    }
    @MiraiInternalApi
    override fun onEnable(){
        val now_tag = "v2.4.1"
        println("当前MiraiCP框架版本:$now_tag")
        logger.info("启动成功")
        logger.info("本项目github存储库:https://github.com/Nambers/MiraiCP")
        dll_name = "${dataFolder.absoluteFile}/$dll_name"
        if(!File(dll_name).exists()){
            logger.error("c++文件$dll_name 不存在")
        }
        val ec = globalEventChannel()
        cpp = CPP_lib()
        if(cpp.ver != now_tag){
            logger.error("警告:当前MiraiCP框架版本($now_tag)和转载的C++ SDK(${cpp.ver})不一致")
        }
        gson = Gson()
        ec.subscribeAlways<BotOnlineEvent> {
            AIbot = this.bot
        }
        //配置文件目录 "${dataFolder.absolutePath}/"
        ec.subscribeAlways<GroupMessageEvent> {
            //群消息
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
        ec.subscribeAlways<FriendMessageEvent> {
            //好友信息

            //针对失效功能的临时补丁
            //[mirai:service:128,<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
            //<msg serviceID="128" templateID="12345" action="native" brief="[链接]邀请你加入群聊" sourceMsgId="0" url="">
            //<item layout="2"><picture cover=""/><title>邀请你加入群聊</title><summary /></item>
            //<data groupcode="1044565129" groupname="mirai 非官方 开发群" msgseq="1613736417225458" msgtype="2"/>
            //</msg>]
            if(this.message.contentToString().startsWith("<?xml version='1.0' encoding='UTF-8' standalone='yes' ?><msg serviceID=\"128\"")){
                val mes = "<data[^>]+>".toRegex().find(this.message.contentToString())?.value
                val groupid = mes?.let { it1 -> Regex("groupcode=\"[0-9]+\"").find(it1)?.value
                    ?.replace("\"", "")
                    ?.replace("groupcode=", "") }
                val ida = mes?.let{it3->Regex("msgseq=\"[0-9]+\"").find(it3)?.value
                    ?.replace("\"", "")
                    ?.replace("msgseq=", "") }
                val groupname = mes?.let{it2->Regex("groupname=\"(.*)\" msgseq").find(it2)?.value
                    ?.replace("\"", "")
                    ?.replace("msgseq", "")
                    ?.replace("groupname=", "") }
                if (ida != null && groupid != null && groupname != null) {
                    BotInvitedJoinGroupRequestEvent(this.bot,ida.toLong(),this.sender.id,groupid.toLong(),groupname,this.sender.nick)
                        .broadcast()
                }
                return@subscribeAlways
            }
            cpp.Event(gson.toJson(
                Config.PrivateMessage(
                    this.sender.id,
                    this.message.serializeToMiraiCode())
            ))
        }
        ec.subscribeAlways<NewFriendRequestEvent>{
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
                    reject()
                }
            }
        }
        ec.subscribeAlways<BotInvitedJoinGroupRequestEvent>{
            //自动同意加群申请
            val r = cpp.Event(gson.toJson(
                Config.GroupInvite(
                    this.groupId,
                    this.invitorId
                )))
            when(r) {
                "true" -> accept()
                "false" -> ignore()
                "NULL" -> ignore()
                else -> {
                    logger.error("BotInvitedJoinGroupRequestEvent unknown return")
                    ignore()
                }
            }
        }
    }
}
