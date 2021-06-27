package tech.eritquearcus.miraicp.shared

import com.google.gson.Gson
import kotlinx.serialization.json.Json
import net.mamoe.mirai.Bot
import net.mamoe.mirai.LowLevelApi
import net.mamoe.mirai.Mirai
import net.mamoe.mirai.contact.Contact
import net.mamoe.mirai.contact.NormalMember
import net.mamoe.mirai.contact.PermissionDeniedException
import net.mamoe.mirai.contact.nameCardOrNick
import net.mamoe.mirai.event.Event
import net.mamoe.mirai.event.EventChannel
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.message.MessageSerializers
import net.mamoe.mirai.message.code.MiraiCode
import net.mamoe.mirai.message.data.*
import net.mamoe.mirai.message.data.Image.Key.queryUrl
import net.mamoe.mirai.message.data.MessageSource.Key.quote
import net.mamoe.mirai.message.data.MessageSource.Key.recall
import net.mamoe.mirai.utils.*
import net.mamoe.mirai.utils.ExternalResource.Companion.toExternalResource
import net.mamoe.mirai.utils.ExternalResource.Companion.uploadAsImage
import net.mamoe.mirai.utils.ExternalResource.Companion.uploadAsVoice
import net.mamoe.mirai.utils.RemoteFile.Companion.uploadFile
import org.json.JSONObject
import java.io.File

object publicShared{
    private val json = Json{
        Mirai
        serializersModule = MessageSerializers.serializersModule
        ignoreUnknownKeys = true
    }
    private var friend_cache = ArrayList<NormalMember>(0)
    private lateinit var cpp: CPP_lib
    private var gson: Gson = Gson()
    private lateinit var logger: MiraiLogger
    private const val now_tag = "v2.6.5"
    lateinit var dll_name: String

    fun init(l: MiraiLogger, path: String){
        logger = l
        dll_name = path
    }

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
        val AIbot = Bot.getInstance(c.botid)
        when(c.type){
            1->{
                logger.info("Send message for(${c.id}) is $message")
                val f = AIbot.getFriend(c.id) ?: let {
                    logger.error("发送消息找不到好友，位置:K-Send()，id:${c.id}")
                    return "EF"
                }
                return json.encodeToString(
                    MessageSource.Serializer,
                    f.sendMessage(message).source)
            }
            2->{
                logger.info("Send message for Group(${c.id}) is $message")
                val g = AIbot.getGroup(c.id) ?: let {
                    logger.error("发送群消息异常找不到群组，位置K-SendG，gid:${c.id}")
                    return "EG"
                }
                return json.encodeToString(
                    MessageSource.Serializer,
                    g.sendMessage(message).source)
            }
            3->{
                logger.info("Send message for a member(${c.id}) is $message")
                for (a in friend_cache) {
                    if (a.id == c.id && a.group.id == c.groupid) {
                        return json.encodeToString(
                            MessageSource.Serializer,
                            a.sendMessage(message).source)
                    }
                }
                val G = AIbot.getGroup(c.groupid) ?: let {
                    logger.error("发送消息找不到群聊，位置K-Send()，id:${c.groupid}")
                    return "EM"
                }
                val f = G[c.id] ?: let {
                    logger.error("发送消息找不到群成员，位置K-Send()，id:${c.id}，gid:${c.groupid}")
                    return "EMM"
                }
                return json.encodeToString(MessageSource.Serializer, f.sendMessage(message).source)
            }
            else -> return "EA"
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

    @OptIn(MiraiExperimentalApi::class, LowLevelApi::class)
    suspend fun RefreshInfo(c: Config.Contact, quit: Boolean): String{
        val AIbot = Bot.getInstance(c.botid)
        when(c.type){
            1->{
                val f = AIbot.getFriend(c.id) ?: let {
                    logger.error("找不到对应好友，位置:K-GetNickOrNameCard()，id:${c.id}")
                    return "EF"
                }
                if(quit) {
                    f.delete()
                    return "done"
                }
                return gson.toJson(Config.ContactInfo(f.nick, f.avatarUrl))
            }
            2->{
                val g = AIbot.getGroup(c.id)?:let{
                    logger.error("取群名称找不到群,位置K-GetNickOrNameCard(), gid:${c.id}")
                    return "EG"
                }
                if(quit){
                    g.quit()
                    return "done"
                }
                return gson.toJson(Config.ContactInfo(g.name, g.avatarUrl,
                    Config.GroupSetting(g.name, "", g.settings.isMuteAll, g.settings.isAllowMemberInvite, g.settings.isAutoApproveEnabled, g.settings.isAnonymousChatEnabled)
                ))
            }
            3->{
                for (a in friend_cache) {
                    if (a.id == c.id && a.group.id == c.groupid) {
                        return a.nameCardOrNick
                    }
                }
                val group = AIbot.getGroup(c.groupid) ?: let {
                    logger.error("取群名片找不到对应群组，位置K-GetNickOrNameCard()，gid:${c.groupid}")
                    return "EM"
                }
                val m = group[c.id] ?: let {
                    logger.error("取群名片找不到对应群成员，位置K-GetNickOrNameCard()，id:${c.id}, gid:${c.groupid}")
                    return "EMM"
                }
                return gson.toJson(Config.ContactInfo(m.nameCardOrNick, m.avatarUrl))
            }
            4->{
                return gson.toJson(Config.ContactInfo(AIbot.nick, AIbot.avatarUrl))
            }
            else->{
                return "EA"
            }
        }
    }

    //取群成员列表
    fun QueryML(c: Config.Contact): String {
        val AIbot = Bot.getInstance(c.botid)
        val g = AIbot.getGroup(c.id) ?: let {
            logger.error("取群成员列表找不到群,位置K-QueryML")
            return "EG"
        }
        val m = java.util.ArrayList<Long>()
        g.members.forEach{
            m.add(it.id)
        }
        return gson.toJson(m)
    }

    fun QueryBFL(bid: Long): String{
        val AIbot = Bot.getInstance(bid)
        val tmp = java.util.ArrayList<Long>()
        AIbot.friends.forEach {
            tmp.add(it.id)
        }
        return gson.toJson(tmp)
    }
    fun QueryBGL(bid: Long): String{
        val AIbot = Bot.getInstance(bid)
        val tmp = java.util.ArrayList<Long>()
        AIbot.groups.forEach {
            tmp.add(it.id)
        }
        return gson.toJson(tmp)
    }

    //图片部分实现

    suspend fun uploadImg(file: String, c: Config.Contact):String{
        val AIbot = Bot.getInstance(c.botid)
        when(c.type){
            1->{
                val temp = AIbot.getFriend(c.id) ?: let {
                    logger.error("发送图片找不到对应好友,位置:K-uploadImgFriend(),id:${c.id}")
                    return "EF"
                }
                return try {
                    File(file).uploadAsImage(temp).imageId
                } catch (e: OverFileSizeMaxException) {
                    logger.error("图片文件过大超过30MB,位置:K-uploadImgGroup(),文件名:$file")
                    "E2"
                } catch (e: NullPointerException) {
                    logger.error("上传图片文件名异常,位置:K-uploadImgGroup(),文件名:$file")
                    "E3"
                }
            }
            2->{
                val temp = AIbot.getGroup(c.id) ?: let {
                    logger.error("发送图片找不到对应群组,位置:K-uploadImgGroup(),id:${c.id}")
                    return "EG"
                }
                return try {
                    File(file).uploadAsImage(temp).imageId
                } catch (e: OverFileSizeMaxException) {
                    logger.error("图片文件过大超过30MB,位置:K-uploadImgGroup(),文件名:$file")
                    "E2"
                } catch (e: NullPointerException) {
                    logger.error("上传图片文件名异常,位置:K-uploadImgGroup(),文件名:$file")
                    "E3"
                }
            }
            3->{
                val temp = AIbot.getGroup(c.groupid) ?: let {
                    logger.error("发送图片找不到对应群组,位置:K-uploadImgGroup(),id:${c.groupid}")
                    return "EM"
                }
                val temp1 = temp[c.id] ?: let {
                    logger.error("发送图片找不到目标成员,位置:K-uploadImgMember(),成员id:${c.id},群聊id:${c.groupid}")
                    return "EMM"
                }
                return try {
                    File(file).uploadAsImage(temp1).imageId
                } catch (e: OverFileSizeMaxException) {
                    logger.error("图片文件过大超过30MB,位置:K-uploadImgGroup(),文件名:$file")
                    "E3"
                } catch (e: NullPointerException) {
                    logger.error("上传图片文件名异常,位置:K-uploadImgGroup(),文件名:$file")
                    "E4"
                }
            }
            else->{
                return "EA"
            }
        }
    }

    suspend fun QueryImg(id: String): String {
        return try {
            Image(id).queryUrl()
        } catch (e: IllegalArgumentException) {
            "E1"
        }
    }

    //recall
    suspend fun recallMsg(a:String): String {
        val source = json.decodeFromString(MessageSource.Serializer,a)
        try{
            source.recall()
        }catch (e: PermissionDeniedException){
            logger.error("机器人无权限撤回")
            return "E1"
        }catch(e:IllegalStateException){
            logger.error("该消息已被撤回")
            return "E2"
        }
        return "Y"
    }

    //禁言
    suspend fun mute(time:Int, c: Config.Contact):String{
        val AIbot = Bot.getInstance(c.botid)
        val group = AIbot.getGroup(c.groupid) ?: let{
            logger.error("禁言找不到对应群组，位置K-mute()，gid:${c.groupid}")
            return "EM"
        }
        val member = group[c.id] ?: let {
            logger.error("禁言找不到对应群成员，位置K-mute()，id:${c.id}, gid:${c.id}")
            return "EMM"
        }
        try {
            member.mute(time)
        }catch (e: PermissionDeniedException){
            logger.error("执行禁言失败机器人无权限，位置:K-mute()，目标群id:${c.groupid}，目标成员id:${c.id}")
            return "E3"
        }catch (e:IllegalStateException){
            logger.error("执行禁言失败禁言时间超出0s~30d，位置:K-mute()，时间:$time")
            return "E4"
        }
        return "Y"
    }

    suspend fun uploadVoice(path: String, oc: Config.Contact): String {
        val AIbot = Bot.getInstance(oc.botid)
        val file = File(path)
        if (!file.exists() || !file.isFile || !(file.extension == "amr" || file.extension == "silk")) {
            logger.error("上传的语言文件需为.amr / .silk文件, 位置: KUploadVoice")
            return "E1"
        }
        val c = when (oc.type) {
            1 -> AIbot.getFriend(oc.id) ?: let { logger.error("上传语音找不到好友, id:${oc.id}"); return "EF" }
            2 -> AIbot.getGroup(oc.id) ?: let {logger.error("上传语音找不到群聊, gid:${oc.id}"); return "EG" }
            3 -> (AIbot.getGroup(oc.groupid) ?: let {logger.error("上传语音找不到群聊, gid:${oc.groupid}"); return "EM" })[oc.id] ?: let {logger.error("上传语音找不到群成员, gid:${oc.id}"); return "EMM" }
            else -> return "EA"
        }
        return file.toExternalResource().use {
            try {
                json.encodeToString(MessageSource.Serializer,  it.uploadAsVoice(c).sendTo(c).source)
            }catch(e:OverFileSizeMaxException){
                logger.error("上传语音失败, 文件应在1MB以内, 实际大小:${it.size}, 文件路径:${file.absolutePath}")
                ""
            }
        }
    }

    private suspend fun fileInfo0(temp: RemoteFile):String{
        val dinfo = temp.getDownloadInfo()!!
        val finfo = temp.getInfo()!!
        return gson.toJson(Config.FileInfo(
            id = finfo.id,
            name = finfo.name,
            path = finfo.path,
            dinfo = Config.DInfo(dinfo.url, dinfo.md5.toString(), dinfo.sha1.toString()),
            finfo = Config.FInfo(
                finfo.length,
                finfo.uploaderId,
                finfo.downloadTimes,
                finfo.uploaderId,
                finfo.lastModifyTime
            )
        )
        )
    }

    suspend fun sendFile(path: String, file: String, c: Config.Contact): String {
        val AIbot = Bot.getInstance(c.botid)
        val group = AIbot.getGroup(c.id) ?: let {
            logger.error("找不到对应群组，位置K-uploadfile()，gid:${c.id}")
            return "EG"
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
            } catch (e: Exception) {
                logger.error(e.message)
                e.printStackTrace()
                return "E3"
            }
        tmp.sendTo(group)
        val temp = group.filesRoot.resolveById(tmp.id) ?: let {
            logger.error("cannot find the file, 位置:K-uploadFile, id:${tmp.id}")
            return "E3"
        }
        return fileInfo0(temp)
    }

    private suspend fun remoteFileList(path: String, c: Config.Contact):String{
        val AIbot = Bot.getInstance(c.botid)
        val group = AIbot.getGroup(c.id) ?: let {
            logger.error("找不到对应群组，位置K-remoteFileInfo，gid:${c.id}")
            return "EG"
        }
        var tmp = "["
        group.filesRoot.resolve(path).listFilesCollection().forEach {
            tmp += "[\"${it.path}\", \"${it.id}\"],"
        }
        tmp = tmp.substring(0, tmp.length - 1)
        tmp += "]"
        return tmp
    }

    private suspend fun remoteFileInfo0(path: String, c:Config.Contact):String {
        val AIbot = Bot.getInstance(c.botid)
        val group = AIbot.getGroup(c.id) ?: let {
            logger.error("找不到对应群组，位置K-remoteFileInfo0，gid:${c.id}")
            return "EG"
        }
        val tmp = group.filesRoot.resolve(path)
        if (!tmp.isFile() || !tmp.exists()) {
            logger.error("cannot find the file,位置:K-remoteFileinfo0, path: $path")
            return "E2"
        }
        return fileInfo0(tmp)
    }

    suspend fun remoteFileInfo(path: String, id: String, c: Config.Contact):String{
        val AIbot = Bot.getInstance(c.botid)
        if(id == "")
            return remoteFileInfo0(path, c)
        if(id == "-1")
            return remoteFileList(path, c)
        val group = AIbot.getGroup(c.id) ?: let {
            logger.error("找不到对应群组，位置K-remoteFileInfo，gid:${c.id}")
            return "EG"
        }
        val tmp = group.filesRoot.resolve(path).resolveById(id)?:let{
            logger.error("cannot find the file,位置:K-remoteFileinfo, id:$id")
            return "E2"
        }
        return fileInfo0(tmp)
    }

    //查询权限
    fun kqueryM(c: Config.Contact): String{
        val AIbot = Bot.getInstance(c.botid)
        val group = AIbot.getGroup(c.groupid) ?: let {
            logger.error("查询权限找不到对应群组，位置K-queryM()，gid:${c.groupid}")
            return "EM"
        }
        val member = group[c.id] ?: let {
            logger.error("查询权限找不到对应群成员，位置K-queryM()，id:${c.id}, gid:${c.groupid}")
            return "EMM"
        }
        return member.permission.level.toString()

    }

    suspend fun kkick(message: String, c: Config.Contact):String{
        val AIbot = Bot.getInstance(c.botid)
        val group = AIbot.getGroup(c.groupid) ?: let {
            logger.error("查询权限找不到对应群组，位置K-queryM()，gid:${c.groupid}")
            return "EM"
        }
        val member = group[c.id] ?: let {
            logger.error("查询权限找不到对应群成员，位置K-queryM()，id:${c.id}, gid:${c.id}")
            return "EMM"
        }
        try {
            member.kick(message)
        }catch (e: PermissionDeniedException){
            return "E3"
        }
        return "Y"
    }

    //取群主
    fun getowner(c: Config.Contact):String{
        val AIbot = Bot.getInstance(c.botid)
        val g = AIbot.getGroup(c.id)?:let {
            logger.error("找不到群,位置:K-getowner,gid:${c.id}")
            return "EG"
        }
        return g.owner.id.toString()
    }

    //构建聊天记录
    suspend fun buildforwardMsg(text:String, bid: Long):String{
        val AIbot = Bot.getInstance(bid)
        val t = Gson().fromJson(text, Config.ForwardMessageJson::class.java)
        val c1: Contact = when(t.type) {
            1 -> AIbot.getFriend(t.id) ?: let {
                return "EF"
            }
            2 -> AIbot.getGroup(t.id) ?: let {
                return "EG"
            }
            3 -> (AIbot.getGroup(t.id) ?: let {
                return "EM"
            })[t.id2]?:let {
                return "EMM"
            }
            else -> return "EA"
        }
        val c: Contact = when(t.content.type) {
            1 -> AIbot.getFriend(t.content.id) ?: let {
                return "EF"
            }
            2 -> AIbot.getGroup(t.content.id) ?: let {
                return "EG"
            }
            3 -> (AIbot.getGroup(t.content.id) ?: let {
                return "EM"
            })[t.content.id2] ?: let {
                return "EMM"
            }
            else -> return "EA"
        }
        val a = ForwardMessageBuilder(c)
        t.content.value.forEach {
            a.add(ForwardMessage.Node(it.id, it.time, it.name, MiraiCode.deserializeMiraiCode(it.message)))
        }
        val re = a.build().sendTo(c1)
        //TODO:https://github.com/mamoe/mirai/issues/1371
        //return json.encodeToString(MessageSource.Serializer, re.source)
        return "Y"
    }

    @Suppress("INVISIBLE_MEMBER")
    suspend fun accpetFriendRequest(info: Config.NewFriendRequestSource): String {
        try {
            NewFriendRequestEvent(
                Bot.getInstance(info.botid),
                info.eventid,
                info.message,
                info.fromid,
                info.fromgroupid,
                info.fromnick
            ).accept()
        } catch (e: IllegalStateException) {
            return "E"
        }
        return "Y"
    }

    @Suppress("INVISIBLE_MEMBER")
    suspend fun rejectFriendRequest(info: Config.NewFriendRequestSource): String {
        try {
            NewFriendRequestEvent(
                Bot.getInstance(info.botid),
                info.eventid,
                info.message,
                info.fromid,
                info.fromgroupid,
                info.fromnick
            ).reject()
        } catch (e: IllegalStateException) {
            return "E"
        }
        return "Y"
    }

    @OptIn(MiraiInternalApi::class)
    @Suppress("INVISIBLE_MEMBER")
    suspend fun accpetGroupInvite(info: Config.GroupInviteSource): String {
        try {
            BotInvitedJoinGroupRequestEvent(
                Bot.getInstance(info.botid),
                info.eventid,
                info.inviterid,
                info.groupid,
                info.groupname,
                info.inviternick
            ).accept()
        } catch (e: IllegalStateException) {
            return "E"
        }
        return "Y"
    }

    @OptIn(MiraiInternalApi::class)
    @Suppress("INVISIBLE_MEMBER")
    suspend fun rejectGroupInvite(info: Config.GroupInviteSource): String {
        try {
            BotInvitedJoinGroupRequestEvent(
                Bot.getInstance(info.botid),
                info.eventid,
                info.inviterid,
                info.groupid,
                info.groupname,
                info.inviternick
            ).ignore()
        } catch (e: IllegalStateException) {
            return "E"
        }
        return "Y"
    }

    suspend fun sendWithQuote(messageSource: String, msg: String, sign: String): String {
        val source = json.decodeFromString(MessageSource.Serializer, messageSource)
        val obj = JSONObject(sign)
        val message = if (obj.getBoolean("MiraiCode")) {
            MiraiCode.deserializeMiraiCode(msg)
        } else {
            PlainText(msg)
        }
        val bot = Bot.getInstance(source.botId)
        val c = when (source.kind) {
            MessageSourceKind.FRIEND -> {
                bot.getFriend(source.fromId) ?: let {
                    logger.error("找不到好友,位置:K-sendWithQuote,id:${source.fromId}")
                    return "EF"
                }
            }
            MessageSourceKind.GROUP -> {
                bot.getGroup(source.targetId) ?: let {
                    logger.error("找不到群,位置:K-sendWithQuote,gid:${source.targetId}")
                    return "EG"
                }
            }
            MessageSourceKind.TEMP -> {
                val tmp = bot.getGroup(obj.getLong("groupid")) ?: let {
                    logger.error("找不到群,位置:K-sendWithQuote,gid:${obj.getLong("groupid")}")
                    return "EM"
                }
                tmp[source.fromId] ?: let {
                    logger.error("找不到群成员,位置:K-sendWithQuote,gid:${obj.getLong("groupid")}, id:${source.fromId}")
                    return "EMM"
                }
            }
            else -> {
                logger.error("类型出错, 位置:K-sendWithQuote, messageSource:${messageSource}")
                return "EA"
            }
        }
        return json.encodeToString(MessageSource.Serializer, c.sendMessage(source.quote() + message).source)
    }

    fun groupSetting(contact: Config.Contact, source: String): String{
        val aibot = Bot.getInstance(contact.botid)
        val group = aibot.getGroup(contact.id)?:let{
            return "EG"
        }
        val root = gson.fromJson(source, Config.GroupSetting::class.java)
        try {
            group.name = root.name
            group.settings.entranceAnnouncement = root.entranceAnnouncement
            group.settings.isMuteAll = root.isMuteAll
            group.settings.isAllowMemberInvite = root.isAllowMemberInvite
        }catch(e: PermissionDeniedException){
            return "E1"
        }
        return "Y"
    }

    fun onDisable() {
        cpp.PluginDisable()
    }

    @OptIn(MiraiExperimentalApi::class)
    fun onEnable(eventChannel: EventChannel<Event>){
        logger.info("当前MiraiCP版本: $now_tag")
        cpp = CPP_lib()
        if(cpp.ver != now_tag){
            logger.error("警告:当前MiraiCP框架版本($now_tag)和加载的C++ SDK(${cpp.ver})不一致")
        }
        //配置文件目录 "${dataFolder.absolutePath}/"
        eventChannel.subscribeAlways<FriendMessageEvent> {
            //好友信息
            cpp.Event(
                gson.toJson(
                    Config.PrivateMessage(
                        Config.Contact(1, this.sender.id, 0, this.senderName, this.bot.id),
                        this.message.serializeToMiraiCode(),
                        json.encodeToString(
                            MessageSource.Serializer,
                            this.message[MessageSource]!!
                        )
                    )
                )
            )
        }
        eventChannel.subscribeAlways<GroupMessageEvent> {
            //群消息
            cpp.Event(
                gson.toJson(
                    Config.GroupMessage(
                        Config.Contact(2, this.group.id, 0, this.group.name, this.bot.id),
                        Config.Contact(3, this.sender.id, this.group.id, this.senderName, this.bot.id),
                        this.message.serializeToMiraiCode(),
                        json.encodeToString(MessageSource.Serializer, this.message[MessageSource]!!)
                    )
                )
            )
        }
        eventChannel.subscribeAlways<MemberLeaveEvent.Kick> {
            friend_cache.add(this.member)
            cpp.Event(
                gson.toJson(
                    Config.MemberLeave(
                        Config.Contact(2, this.group.id, 0, this.group.name, this.bot.id),
                        this.member.id,
                        1,
                        if (this.operator?.id == null) this.bot.id else this.operator!!.id
                    )
                )
            )
            friend_cache.remove(this.member)
        }
        eventChannel.subscribeAlways<MemberLeaveEvent.Quit> {
            friend_cache.add(this.member)
            cpp.Event(
                gson.toJson(
                    Config.MemberLeave(
                        Config.Contact(2, this.group.id, 0, this.group.name, this.bot.id),
                        this.member.id,
                        2,
                        this.member.id
                    )
                )
            )
            friend_cache.remove(this.member)
        }
        eventChannel.subscribeAlways<MemberJoinEvent.Retrieve> {
            cpp.Event(
                gson.toJson(
                    Config.MemberJoin(
                        Config.Contact(2, this.group.id, 0, this.group.name, this.bot.id),
                        Config.Contact(3, this.member.id, this.groupId, this.member.nameCardOrNick, this.bot.id),
                        3,
                        this.member.id
                    )
                )
            )
        }
        eventChannel.subscribeAlways<MemberJoinEvent.Active> {
            cpp.Event(
                gson.toJson(
                    Config.MemberJoin(
                        Config.Contact(2, this.group.id, 0, this.group.name, this.bot.id),
                        Config.Contact(3, this.member.id, this.groupId, this.member.nameCardOrNick, this.bot.id),
                        2,
                        this.member.id
                    )
                )
            )
        }
        eventChannel.subscribeAlways<MemberJoinEvent.Invite> {
            cpp.Event(
                gson.toJson(
                    Config.MemberJoin(
                        Config.Contact(2, this.group.id, 0, this.group.name, this.bot.id),
                        Config.Contact(3, this.member.id, this.groupId, this.member.nameCardOrNick, this.bot.id),
                        1,
                        this.invitor.id
                    )
                )
            )
        }
        eventChannel.subscribeAlways<NewFriendRequestEvent> {
            //自动同意好友申请
            cpp.Event(
                gson.toJson(
                    Config.NewFriendRequest(
                        Config.NewFriendRequestSource(
                            this.bot.id,
                            this.eventId,
                            this.message,
                            this.fromId,
                            this.fromGroupId,
                            this.fromNick
                        )
                    )
                )
            )

        }
        eventChannel.subscribeAlways<MessageRecallEvent.FriendRecall> {
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
                        this.bot.id
                    )
                )
            )

        }
        eventChannel.subscribeAlways<MessageRecallEvent.GroupRecall> {
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
                        this.bot.id
                    )
                )
            )

        }
        eventChannel.subscribeAlways<BotJoinGroupEvent.Invite>{
            cpp.Event(
                gson.toJson(
                    Config.BotJoinGroup(
                        1,
                        Config.Contact(2, this.group.id, 0, this.group.name, this.bot.id),
                        this.invitor.id
                    )
                )
            )
        }
        eventChannel.subscribeAlways<BotJoinGroupEvent.Active>{
            cpp.Event(
                gson.toJson(
                    Config.BotJoinGroup(
                        2,
                        Config.Contact(2, this.group.id, 0, this.group.name, this.bot.id),
                        0
                    )
                )
            )
        }
        eventChannel.subscribeAlways<BotJoinGroupEvent.Retrieve>{
            cpp.Event(
                gson.toJson(
                    Config.BotJoinGroup(
                        3,
                        Config.Contact(2, this.group.id, 0, this.group.name, this.bot.id),
                        0
                    )
                )
            )
        }
        eventChannel.subscribeAlways<BotInvitedJoinGroupRequestEvent> {
            //自动同意加群申请
            cpp.Event(
                gson.toJson(
                    Config.GroupInvite(
                        Config.GroupInviteSource(
                            this.bot.id,
                            this.eventId,
                            this.invitorId,
                            this.groupId,
                            this.groupName,
                            this.invitorNick
                        )
                    )
                )
            )
        }
        eventChannel.subscribeAlways<GroupTempMessageEvent> {
            //群临时会话
            cpp.Event(
                gson.toJson(
                    Config.GroupTempMessage(
                        Config.Contact(2, this.group.id, 0, this.group.name, this.bot.id),
                        Config.Contact(3, this.sender.id, this.group.id, this.sender.nameCardOrNick, this.bot.id),
                        this.message.serializeToMiraiCode(),
                        json.encodeToString(
                            MessageSource.Serializer,
                            this.source
                        )
                    )
                )
            )
        }
    }
}