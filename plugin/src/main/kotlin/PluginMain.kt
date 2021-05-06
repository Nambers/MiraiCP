package tech.eritquearcus.miraicp

import com.google.gson.Gson
import com.google.gson.JsonSyntaxException
import kotlinx.serialization.json.Json
import net.mamoe.mirai.Bot
import net.mamoe.mirai.Mirai
import net.mamoe.mirai.console.plugin.jvm.JvmPluginDescription
import net.mamoe.mirai.console.plugin.jvm.KotlinPlugin
import net.mamoe.mirai.contact.Contact
import net.mamoe.mirai.contact.NormalMember
import net.mamoe.mirai.contact.PermissionDeniedException
import net.mamoe.mirai.contact.nameCardOrNick
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.event.globalEventChannel
import net.mamoe.mirai.message.MessageSerializers
import net.mamoe.mirai.message.code.MiraiCode
import net.mamoe.mirai.message.data.*
import net.mamoe.mirai.message.data.Image.Key.queryUrl
import net.mamoe.mirai.message.data.MessageSource.Key.recall
import net.mamoe.mirai.utils.ExternalResource.Companion.uploadAsImage
import net.mamoe.mirai.utils.MiraiExperimentalApi
import net.mamoe.mirai.utils.MiraiInternalApi
import net.mamoe.mirai.utils.OverFileSizeMaxException
import net.mamoe.mirai.utils.RemoteFile
import net.mamoe.mirai.utils.RemoteFile.Companion.uploadFile
import java.io.File
import java.util.*
import kotlin.concurrent.schedule

/*
在settings.gradle.kts里改生成的插件.jar名称
用runmiraikt这个配置可以在ide里运行，不用复制到mcl或其他启动器
 */


object PluginMain : KotlinPlugin(
    JvmPluginDescription(
        id = "tech.eritquearcus.miraiCP",
        name = "miraiCP",
        version = "2.6.1"
    )
) {
    private val json = Json{
        Mirai
        serializersModule = MessageSerializers.serializersModule
    }
    private var friend_cache = ArrayList<NormalMember>(0)
    var dll_name = "mirai-demo.dll"
    private lateinit var AIbot: Bot
    private lateinit var cpp: CPP_lib
    private lateinit var gson: Gson
    private const val now_tag = "v2.6.1"
    // 临时解决方案
    private var finvite = ArrayList<NewFriendRequestEvent>(0)
    private var ginvite = ArrayList<BotInvitedJoinGroupRequestEvent>(0)

    //日志部分实现
    fun BasicSendLog(log: String) {
        logger.info(log)
    }

    fun SendWarning(log: String) {
        logger.warning(log)
    }

    fun SendError(log: String) {
        logger.error(log)
    }

    //发送消息部分实现 MiraiCode

    suspend fun Send0(message: Message, c:Config.Contact):String{
        when(c.type){
            1->{
                logger.info("Send message for(${c.id}) is $message")
                val f = AIbot.getFriend(c.id) ?: let {
                    logger.error("发送消息找不到好友，位置:K-Send()，id:${c.id}")
                    return "E1"
                }
                return json.encodeToString(MessageSource.Serializer,
                    f.sendMessage(message).source)
            }
            2->{
                logger.info("Send message for Group(${c.id}) is $message")
                val g = AIbot.getGroup(c.id) ?: let {
                    logger.error("发送群消息异常找不到群组，位置K-SendG，gid:${c.id}")
                    return "E1"
                }
                return json.encodeToString(MessageSource.Serializer,
                    g.sendMessage(message).source)
            }
            3->{
                logger.info("Send message for a member(${c.id}) is $message")
                for (a in friend_cache) {
                    if (a.id == c.id && a.group.id == c.groupid) {
                        return json.encodeToString(MessageSource.Serializer,
                            a.sendMessage(message).source)
                    }
                }
                val G = AIbot.getGroup(c.groupid) ?: let {
                    logger.error("发送消息找不到群聊，位置K-Send()，id:${c.groupid}")
                    return "E1"
                }
                val f = G[c.id] ?: let {
                    logger.error("发送消息找不到群成员，位置K-Send()，id:${c.id}，gid:${c.groupid}")
                    return "E2"
                }
                return json.encodeToString(MessageSource.Serializer, f.sendMessage(message).source)
            }
            else->return "E2"
        }
    }

    suspend fun SendMsg(message: String, c:Config.Contact):String{
        val m = MessageChainBuilder()
        m.add(message)
        return Send0(m.asMessageChain(), c)
    }

    suspend fun SendMiraiCode(message: String, c:Config.Contact):String{
        return Send0(MiraiCode.deserializeMiraiCode(message), c)
    }

    fun RefreshInfo(c: Config.Contact): String{
        when(c.type){
            1->{
                val f = AIbot.getFriend(c.id) ?: let {
                    logger.error("找不到对应好友，位置:K-GetNickOrNameCard()，id:${c.id}")
                    return "E1"
                }
                return gson.toJson(Config.ContactInfo(f.nick, f.avatarUrl))
            }
            2->{
                val g = AIbot.getGroup(c.id)?:let{
                    logger.error("取群名称找不到群,位置K-GetNickOrNameCard(), gid:${c.id}")
                    return "E1"
                }
                return gson.toJson(Config.ContactInfo(g.name, g.avatarUrl))
            }
            3->{
                for (a in friend_cache) {
                    if (a.id == c.id && a.group.id == c.groupid) {
                        return a.nameCardOrNick
                    }
                }

                val group = AIbot.getGroup(c.groupid) ?: let {
                    logger.error("取群名片找不到对应群组，位置K-GetNickOrNameCard()，gid:${c.groupid}")
                    return "E1"
                }
                val m = group[c.id] ?: let {
                    logger.error("取群名片找不到对应群成员，位置K-GetNickOrNameCard()，id:${c.id}, gid:${c.groupid}")
                    return "E2"
                }
                return gson.toJson(Config.ContactInfo(m.nameCardOrNick, m.avatarUrl))
            }
            4->{
                return gson.toJson(Config.ContactInfo(AIbot.nick, AIbot.avatarUrl))
            }
            else->{
                return "EE"
            }
        }
    }

    //取群成员列表
    fun QueryML(groupid: Long): String {
        val g = AIbot.getGroup(groupid) ?: let {
            logger.error("取群成员找不到群,位置K-QueryML")
            return "E1"
        }
        val m = ArrayList<Long>()
        g.members.forEach{
            m.add(it.id)
        }
        return gson.toJson(m)
    }

    fun QueryBFL(): String{
        val tmp = ArrayList<Long>()
        AIbot.friends.forEach {
            tmp.add(it.id)
        }
        return gson.toJson(tmp)
    }
    fun QueryBGL(): String{
        val tmp = ArrayList<Long>()
        AIbot.groups.forEach {
            tmp.add(it.id)
        }
        return gson.toJson(tmp)
    }

    //图片部分实现

    suspend fun uploadImg(file: String, c: Config.Contact):String{
        when(c.type){
            1->{
                val temp = AIbot.getFriend(c.id) ?: let {
                    logger.error("发送图片找不到对应好友,位置:K-uploadImgFriend(),id:${c.id}")
                    return ""
                }
                return try {
                    File(file).uploadAsImage(temp).imageId
                } catch (e: OverFileSizeMaxException) {
                    logger.error("图片文件过大超过30MB,位置:K-uploadImgGroup(),文件名:$file")
                    ""
                } catch (e: NullPointerException) {
                    logger.error("上传图片文件名异常,位置:K-uploadImgGroup(),文件名:$file")
                    ""
                }
            }
            2->{
                val temp = AIbot.getGroup(c.id) ?: let {
                    logger.error("发送图片找不到对应群组,位置:K-uploadImgGroup(),id:${c.id}")
                    return ""
                }
                return try {
                    File(file).uploadAsImage(temp).imageId
                } catch (e: OverFileSizeMaxException) {
                    logger.error("图片文件过大超过30MB,位置:K-uploadImgGroup(),文件名:$file")
                    ""
                } catch (e: NullPointerException) {
                    logger.error("上传图片文件名异常,位置:K-uploadImgGroup(),文件名:$file")
                    ""
                }
            }
            3->{
                val temp = AIbot.getGroup(c.groupid) ?: let {
                    logger.error("发送图片找不到对应群组,位置:K-uploadImgGroup(),id:${c.groupid}")
                    return ""
                }
                val temp1 = temp[c.id] ?: let {
                    logger.error("发送图片找不到目标成员,位置:K-uploadImgMember(),成员id:${c.id},群聊id:${c.groupid}")
                    return ""
                }
                return try {
                    File(file).uploadAsImage(temp1).imageId
                } catch (e: OverFileSizeMaxException) {
                    logger.error("图片文件过大超过30MB,位置:K-uploadImgGroup(),文件名:$file")
                    ""
                } catch (e: NullPointerException) {
                    logger.error("上传图片文件名异常,位置:K-uploadImgGroup(),文件名:$file")
                    ""
                }
            }
            else->{
                return ""
            }
        }
    }

    suspend fun QueryImg(id: String): String {
        return Image(id).queryUrl()
    }

    //recall
    suspend fun recallMsg(a:String): String {
        val source = json.decodeFromString(MessageSource.Serializer,a)
        try{
            source.recall()
        }catch (e:PermissionDeniedException){
            logger.error("机器人无权限撤回")
            return "E1"
        }catch(e:IllegalStateException){
            logger.error("该消息已被撤回")
            return "E2"
        }
        return "Y"
    }

    //定时任务
    fun scheduling(time: Long, id: String) {
        Timer("SettingUp", false).schedule(time) {
            cpp.Event(
                Gson().toJson(
                    Config.TimeOutEvent(
                        id,
                        AIbot.id
                    )
                )
            )
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

    private suspend fun fileInfo0(temp: RemoteFile):String{
        val dinfo = temp.getDownloadInfo()!!
        val finfo = temp.getInfo()!!
        return gson.toJson(Config.FileInfo(
            id = finfo.id,
            name=finfo.name,
            path= finfo.path,
            dinfo = Config.DInfo(dinfo.url, dinfo.md5.toString(), dinfo.sha1.toString()),
            fInfo = Config.FInfo(finfo.length, finfo.uploaderId, finfo.downloadTimes, finfo.uploaderId, finfo.lastModifyTime))
        )
    }
    suspend fun uploadFile(path: String, file: String, c: Config.Contact): String {
        val group = AIbot.getGroup(c.id) ?: let {
            logger.error("找不到对应群组，位置K-uploadfile()，gid:${c.id}")
            return "E1"
        }
        val f = File(file)
        if (!f.exists() || !f.isFile) {
            return "E2"
        }
        val tmp =
            try {
                group.uploadFile(path, f)
            } catch (e: IllegalStateException) {
                return "E3"
            } catch (e: Exception){
                logger.error(e.message)
                e.printStackTrace()
                return "E3"
            }
        val temp = tmp.toRemoteFile(group)?:let{
            return "E3"
        }
        logger.info(fileInfo0(temp))
        return fileInfo0(temp)
    }

    suspend fun remoteFileInfo(path: String, id: String, c: Config.Contact):String{
        val group = AIbot.getGroup(c.id) ?: let {
            logger.error("找不到对应群组，位置K-uploadfile()，gid:${c.id}")
            return "E1"
        }
        val tmp = group.filesRoot.resolve(path).resolveById(id)?:let{
            return "E2"
        }
        return fileInfo0(tmp)
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

    suspend fun kkick(qqid: Long, groupid: Long, message: String):String{
        val group = AIbot.getGroup(groupid) ?: let {
            logger.error("查询权限找不到对应群组，位置K-queryM()，gid:$groupid")
            return "E1"
        }
        val member = group[qqid] ?: let {
            logger.error("查询权限找不到对应群成员，位置K-queryM()，id:$qqid, gid:$groupid")
            return "E2"
        }
        try {
            member.kick(message)
        }catch (e:PermissionDeniedException){
            return "E3"
        }
        return "Y"
    }

    //全员禁言
    fun muteall(groupid: Long, sign: Boolean):String{
        val g =AIbot.getGroup(groupid)?:let{
            return "E1"
        }
        try {
            g.settings.isMuteAll = sign
        }catch(e:PermissionDeniedException){
            return "E2"
        }
        return "Y"
    }

    //取群主
    fun getowner(groupid: Long):String{
        val g = AIbot.getGroup(groupid)?:let {
            return "E1"
        }
        return g.owner.id.toString()
    }

    //构建聊天记录
    suspend fun buildforwardMsg(text:String):String{
        val t = Gson().fromJson(text, Config.ForwardMessageJson::class.java)
        val c1:Contact = when(t.type) {
            1 -> AIbot.getFriend(t.id) ?: let {
                return "E1"
            }
            2 -> AIbot.getGroup(t.id) ?: let {
                return "E1"
            }
            3 -> (AIbot.getGroup(t.id) ?: let {
                return "E1"
            })[t.id2]?:let {
                return "E2"
            }
            else -> return "E3"
        }
        val c:Contact = when(t.content.type) {
            1 -> AIbot.getFriend(t.content.id) ?: let {
                return "E1"
            }
            2 -> AIbot.getGroup(t.content.id) ?: let {
                return "E1"
            }
            3 -> (AIbot.getGroup(t.content.id) ?: let {
                return "E1"
            })[t.content.id2]?:let {
                return "E2"
            }
            else -> return "E3"
        }
        val a = ForwardMessageBuilder(c)
        t.content.value.forEach {
            a.add(ForwardMessage.Node(it.id, it.time, it.name, MiraiCode.deserializeMiraiCode(it.message)))
        }
        a.build().sendTo(c1)
        return "Y"
    }

    suspend fun accpetFriendRequest(text:String): String{
        finvite[text.toInt()].accept()
        return "Y"
    }
    suspend fun rejectFriendRequest(text:String):String{
        finvite[text.toInt()].reject()
        return "Y"
    }
    suspend fun accpetGroupInvite(text:String): String{
        try {
            ginvite[text.toInt()].accept()
            ginvite.remove(ginvite[text.toInt()])
        }catch (e: JsonSyntaxException){
            return "E"
        }
        return "Y"
    }
    suspend fun rejectGroupInvite(text:String):String{
        try{
            ginvite[text.toInt()].ignore()
            ginvite.remove(ginvite[text.toInt()])
        }catch (e: JsonSyntaxException){
            return "E"
        }
        return "Y"
    }
    override fun onDisable() {
        cpp.PluginDisable()
    }
    @MiraiExperimentalApi
    @MiraiInternalApi
    override fun onEnable(){
        println("当前MiraiCP框架版本:$now_tag")
        logger.info("启动成功")
        logger.info("本项目github存储库:https://github.com/Nambers/MiraiCP")
        dll_name = "${dataFolder.absoluteFile}\\$dll_name"
        val Configuration = "${dataFolder.absoluteFile}\\miraicp.txt"
        if(!File(Configuration).exists() || !File(Configuration).isFile){
            logger.error("配置文件$Configuration 不存在，使用默认dll路径$dll_name")
        }else {
            val c = File(Configuration).readLines()
            if (c.size != 1 || !File(c[0]).exists() || !File(c[0]).isFile || File(c[0]).extension != "dll") {
                logger.error("配置文件$Configuration 格式错误，应只包含一行且为一个已存在的dll文件，使用默认dll路径$dll_name")
            }else{
                dll_name = c[0]
            }
        }
        if(!File(dll_name).exists()){
            logger.error("c++文件$dll_name 不存在")
        }
        val ec = globalEventChannel()
        cpp = CPP_lib()
        if(cpp.ver != now_tag){
            logger.error("警告:当前MiraiCP框架版本($now_tag)和加载的C++ SDK(${cpp.ver})不一致")
        }
        gson = Gson()
        ec.subscribeAlways<BotOnlineEvent> {
            AIbot = this.bot
        }
        //配置文件目录 "${dataFolder.absolutePath}/"
        ec.subscribeAlways<FriendMessageEvent> {
            //好友信息
            cpp.Event(
                gson.toJson(
                    Config.PrivateMessage(
                        this.sender.id,
                        this.message.serializeToMiraiCode(),
                        json.encodeToString(MessageSource.Serializer,
                            this.message[MessageSource]!!),
                        AIbot.id
                    )
                )
            )
        }

        ec.subscribeAlways<GroupMessageEvent> {
            //群消息
            cpp.Event(
                gson.toJson(
                    Config.GroupMessage(
                        this.group.id,
                        this.sender.id,
                        this.message.serializeToMiraiCode(),
                        json.encodeToString(MessageSource.Serializer,this.message[MessageSource]!!),
                        AIbot.id
                    )
                )
            )
        }
        ec.subscribeAlways<MessageRecallEvent.FriendRecall> {
            cpp.Event(
                gson.toJson(
                    Config.RecallEvent(
                        1,
                        this.authorId,
                        this.operatorId,
                        this.messageIds.map { it.toString() }.toTypedArray().contentToString(),
                        this.messageInternalIds.map { it.toString() }.toTypedArray().contentToString(),
                        this.messageTime,
                        0,
                        AIbot.id
                    )
                )
            )

        }
        ec.subscribeAlways<MessageRecallEvent.GroupRecall> {
            cpp.Event(
                gson.toJson(
                    Config.RecallEvent(
                        2,
                        this.authorId,
                        this.operator!!.id,
                        this.messageIds.map { it.toString() }.toTypedArray().contentToString(),
                        this.messageInternalIds.map { it.toString() }.toTypedArray().contentToString(),
                        this.messageTime,
                        this.group.id,
                        AIbot.id
                    )
                )
            )

        }
        ec.subscribeAlways<MemberLeaveEvent.Kick> {
            friend_cache.add(this.member)
            cpp.Event(gson.toJson(
                Config.MemberLeave(
                    this.group.id,
                    this.member.id,
                    1,
                    if(this.operator?.id == null) this.bot.id else this.operator!!.id,
                    AIbot.id
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
                    this.member.id,
                    AIbot.id
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
                    this.member.id,
                    AIbot.id
                )
            ))
        }
        ec.subscribeAlways<MemberJoinEvent.Active> {
            cpp.Event(gson.toJson(
                Config.MemberJoin(
                    this.group.id,
                    this.member.id,
                    2,
                    this.member.id,
                    AIbot.id
                )
            ))
        }
        ec.subscribeAlways<MemberJoinEvent.Invite> {
            cpp.Event(gson.toJson(
                Config.MemberJoin(
                    this.group.id,
                    this.member.id,
                    1,
                    this.invitor.id,
                    AIbot.id
                )
            ))
        }
        ec.subscribeAlways<NewFriendRequestEvent> {
            //自动同意好友申请
            finvite.add(this)
            cpp.Event(
                gson.toJson(
                    Config.NewFriendRequest(
                        this.fromId,
                        this.message,
                        (finvite.size - 1).toString(),
                        AIbot.id
                    )
                )
            )

        }
        ec.subscribeAlways<BotJoinGroupEvent.Invite>{
            cpp.Event(
                gson.toJson(
                    Config.BotJoinGroup(
                        1,
                        this.groupId,
                        this.invitor.id,
                        AIbot.id
                    )
                )
            )
        }
        ec.subscribeAlways<BotJoinGroupEvent.Active>{
            cpp.Event(
                gson.toJson(
                    Config.BotJoinGroup(
                        2,
                        this.groupId,
                        0,
                        AIbot.id
                    )
                )
            )
        }
        ec.subscribeAlways<BotJoinGroupEvent.Retrieve>{
            cpp.Event(
                gson.toJson(
                    Config.BotJoinGroup(
                        3,
                        this.groupId,
                        0,
                        AIbot.id
                    )
                )
            )
        }
        ec.subscribeAlways<BotInvitedJoinGroupRequestEvent> {
            //自动同意加群申请
            ginvite.add(this)
            cpp.Event(
                gson.toJson(
                    Config.GroupInvite(
                        this.groupId,
                        this.groupName,
                        this.invitorId,
                        this.invitorNick,
                        (ginvite.size).toString(),
                        AIbot.id
                    )
                )
            )
        }
        ec.subscribeAlways<GroupTempMessageEvent> {
            //群临时会话
            cpp.Event(
                gson.toJson(
                    Config.GroupTempMessage(
                        this.group.id,
                        this.sender.id,
                        this.message.serializeToMiraiCode(),
                        json.encodeToString(MessageSource.Serializer,
                            this.source
                        ),
                        AIbot.id
                    )
                )
            )
        }
    }
}
