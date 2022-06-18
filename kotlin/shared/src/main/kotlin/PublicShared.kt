/*
 * Copyright (c) 2020 - 2022. Eritque arcus and contributors.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version(in your opinion).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

package tech.eritquearcus.miraicp.shared

import com.google.gson.Gson
import kotlinx.coroutines.TimeoutCancellationException
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.flow.toList
import kotlinx.coroutines.runBlocking
import kotlinx.serialization.json.Json
import net.mamoe.mirai.Bot
import net.mamoe.mirai.Mirai
import net.mamoe.mirai.contact.*
import net.mamoe.mirai.contact.announcement.OfflineAnnouncement
import net.mamoe.mirai.contact.announcement.OnlineAnnouncement
import net.mamoe.mirai.contact.announcement.bot
import net.mamoe.mirai.contact.announcement.buildAnnouncementParameters
import net.mamoe.mirai.contact.file.AbsoluteFile
import net.mamoe.mirai.data.RequestEventData
import net.mamoe.mirai.data.RequestEventData.Factory.toRequestEventData
import net.mamoe.mirai.event.Event
import net.mamoe.mirai.event.EventChannel
import net.mamoe.mirai.event.EventPriority
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.message.MessageSerializers
import net.mamoe.mirai.message.code.MiraiCode
import net.mamoe.mirai.message.data.*
import net.mamoe.mirai.message.data.Image.Key.isUploaded
import net.mamoe.mirai.message.data.Image.Key.queryUrl
import net.mamoe.mirai.message.data.MessageChain.Companion.serializeToJsonString
import net.mamoe.mirai.message.data.MessageSource.Key.quote
import net.mamoe.mirai.message.data.MessageSource.Key.recall
import net.mamoe.mirai.utils.ExternalResource.Companion.toExternalResource
import net.mamoe.mirai.utils.ExternalResource.Companion.uploadAsImage
import net.mamoe.mirai.utils.MiraiExperimentalApi
import net.mamoe.mirai.utils.MiraiLogger
import net.mamoe.mirai.utils.OverFileSizeMaxException
import org.json.JSONObject
import java.io.File
import java.util.*
import kotlin.concurrent.schedule


object PublicShared {
    internal val json by lazy {
        Json {
            Mirai
            serializersModule = MessageSerializers.serializersModule
            ignoreUnknownKeys = true
        }
    }
    private var friend_cache = ArrayList<NormalMember>(0)
    val cpp: ArrayList<CPPLib> = arrayListOf()
    val gson: Gson = Gson()
    lateinit var logger: MiraiLogger
    const val now_tag = "v${BuiltInConstants.version}"
    private val logger4plugins: MutableMap<String, MiraiLogger> = mutableMapOf()
    val disablePlugins = arrayListOf<String>()
    var cachePath: File = File("")
    var maxThread = Integer.MAX_VALUE
    lateinit var commandReg: CommandHandler

    fun init(l: MiraiLogger) {
        logger = l
    }

    fun nextMsg(c: Config.Contact, time: Long, halt: Boolean): String {
        return runBlocking {
            val e = try {
                when (c.type) {
                    1 -> nextMessage<FriendMessageEvent>(time, halt, EventPriority.HIGHEST) {
                        it.sender.id == c.id && it.bot.id == c.botid
                    }
                    2 -> nextMessage<GroupMessageEvent>(time, halt, EventPriority.HIGHEST) {
                        it.group.id == c.id && it.bot.id == c.botid
                    }
                    3 -> nextMessage<GroupMessageEvent>(time, halt, EventPriority.HIGHEST) {
                        it.bot.id == c.botid && it.group.id == c.groupid && it.sender.id == c.id
                    }
                    else -> throw Exception() //unreachable
                }
            } catch (e: TimeoutCancellationException) {
                return@runBlocking "E1"
            }
            gson.toJson(
                Config.Message(
                    json.encodeToString(
                        MessageSource.Serializer, e[MessageSource]!!
                    )
                )
            )
        }
    }

    private fun getPluginLogger(name: String): MiraiLogger =
        if (logger4plugins.containsKey(name))
            logger4plugins[name]!!
        else
            MiraiLogger.Factory.create(this::class, name).apply {
                logger4plugins[name] = this
            }

    //日志部分实现
    fun basicSendLog(log: String, botid: Long, name: String = "") {
        when (botid) {
            -1L -> getPluginLogger(name).info(log)
            -2L -> logger.info(log)
            else -> Bot.getInstance(botid).logger.info(log)
        }
    }

    fun sendWarning(log: String, botid: Long, name: String = "") {
        when (botid) {
            -1L -> getPluginLogger(name).warning(log)
            -2L -> logger.warning(log)
            else -> Bot.getInstance(botid).logger.warning(log)
        }
    }

    fun sendError(log: String, botid: Long, name: String = "") {
        when (botid) {
            -1L -> getPluginLogger(name).error(log)
            -2L -> logger.error(log)
            else -> Bot.getInstance(botid).logger.error(log)
        }
    }

    //发送消息部分实现 MiraiCode

    private suspend fun send0(message: Message, c: Config.Contact): String = withBot(c.botid) { AIbot ->
        val r = kotlin.run {
            when (c.type) {
                1 -> {
                    logger.info("Send message for(${c.id}) is $message")
                    AIbot.getFriend(c.id) ?: let {
                        logger.error("发送消息找不到好友，位置:K-Send()，id:${c.id}")
                        return@withBot "EF"
                    }
                }
                2 -> {
                    logger.info("Send message for Group(${c.id}) is $message")
                    AIbot.getGroup(c.id) ?: let {
                        logger.error("发送群消息异常找不到群组，位置K-SendG，gid:${c.id}")
                        return@withBot "EG"
                    }
                }
                3 -> {
                    logger.info("Send message for a member(${c.id}) is $message")
                    for (a in friend_cache) {
                        if (a.id == c.id && a.group.id == c.groupid) {
                            return@run a
                        }
                    }
                    val g = AIbot.getGroup(c.groupid) ?: let {
                        logger.error("发送消息找不到群聊，位置K-Send()，id:${c.groupid}")
                        return@withBot "EM"
                    }
                    g[c.id] ?: let {
                        logger.error("发送消息找不到群成员，位置K-Send()，id:${c.id}，gid:${c.groupid}")
                        return@withBot "EMM"
                    }
                }
                else -> return@withBot "EA"
            }
        }
        sendWithCatch { r.sendMessage(message).source }
    }

    suspend fun sendMsg(message: String, c: Config.Contact): String = send0(message.toPlainText().toMessageChain(), c)

    suspend fun sendMiraiCode(message: String, c: Config.Contact): String =
        send0(MiraiCode.deserializeMiraiCode(message), c)

    private fun OnlineAnnouncement.toOnlineA(): Config.OnlineA {
        return Config.OnlineA(
            this.content,
            this.fid,
            this.parameters.image?.id ?: let { "" },
            this.confirmedMembersCount,
            this.senderId,
            this.group.id,
            this.bot.id,
            this.publicationTime,
            Config.AP(
                this.parameters.sendToNewMember,
                this.parameters.isPinned,
                this.parameters.showEditCard,
                this.parameters.showPopup,
                this.parameters.requireConfirmation
            )
        )
    }

    @OptIn(MiraiExperimentalApi::class)
    suspend fun refreshInfo(c: Config.Contact, quit: Boolean, annoucment: Boolean): String = c.withBot { bot ->
        when (c.type) {
            1 -> c.withFriend(bot, "找不到对应好友，位置:K-GetNickOrNameCard()，id:${c.id}") { f ->
                if (quit) {
                    f.delete()
                    return "done"
                }
                gson.toJson(Config.ContactInfo(f.nick, f.avatarUrl))
            }
            2 -> c.withGroup(bot, "取群名称找不到群,位置K-GetNickOrNameCard(), gid:${c.id}") { g ->
                if (annoucment) return gson.toJson(g.announcements.toList().map { it.toOnlineA() })
                if (quit) {
                    g.quit()
                    return "done"
                }
                gson.toJson(
                    Config.ContactInfo(
                        g.name, g.avatarUrl, Config.GroupSetting(
                            g.name,
                            g.settings.isMuteAll,
                            g.settings.isAllowMemberInvite,
                            g.settings.isAutoApproveEnabled,
                            g.settings.isAnonymousChatEnabled
                        )
                    )
                )
            }
            3 -> friend_cache.firstOrNull { a ->
                a.id == c.id && a.group.id == c.groupid
            }?.let {
                gson.toJson(Config.ContactInfo(it.nameCardOrNick, it.avatarUrl))
            } ?: let {
                c.withMember(
                    bot,
                    "取群名片找不到对应群组，位置K-GetNickOrNameCard()，gid:${c.groupid}",
                    "取群名片找不到对应群成员，位置K-GetNickOrNameCard()，id:${c.id}, gid:${c.groupid}"
                ) { _, m ->
                    gson.toJson(Config.ContactInfo(m.nameCardOrNick, m.avatarUrl))
                }
            }
            4 -> gson.toJson(Config.ContactInfo(bot.nick, bot.avatarUrl))
            else -> "EA"
        }
    }

    //取群成员列表
    fun queryML(c: Config.Contact): String = c.withBot { bot ->
        c.withGroup(bot) { g ->
            gson.toJson(g.members.map { it.id })
        }
    }

    fun queryBFL(bid: Long): String = withBot(bid) { bot ->
        gson.toJson(bot.friends.map {
            it.id
        })
    }

    fun queryBGL(bid: Long): String = withBot(bid) { bot ->
        gson.toJson(bot.groups.map { it.id })
    }

    //图片部分实现
    private suspend fun uploadImgAndId(file: String, temp: Contact, err1: String = "E2", err2: String = "E3"): String =
        try {
            val img = File(file).uploadAsImage(temp)
            gson.toJson(
                Config.ImgInfo(
                    img.size,
                    img.width,
                    img.height,
                    gson.toJson(img.md5),
                    img.queryUrl(),
                    img.imageId,
                    img.imageType.ordinal
                )
            )
        } catch (e: OverFileSizeMaxException) {
            logger.error("图片文件过大超过30MB,位置:K-uploadImgGroup(),文件名:$file")
            err1
        } catch (e: NullPointerException) {
            logger.error("上传图片文件名异常,位置:K-uploadImgGroup(),文件名:$file")
            err2
        }

    suspend fun uploadImg(file: String, c: Config.Contact): String = c.withBot { bot ->
        when (c.type) {
            1 -> c.withFriend(bot, "发送图片找不到对应好友,位置:K-uploadImgFriend(),id:${c.id}") { temp ->
                uploadImgAndId(file, temp)
            }
            2 -> c.withGroup(bot, "发送图片找不到对应群组,位置:K-uploadImgGroup(),id:${c.id}") { temp ->
                uploadImgAndId(file, temp)
            }
            3 -> c.withMember(
                bot,
                "发送图片找不到对应群组,位置:K-uploadImgGroup(),id:${c.groupid}",
                "发送图片找不到目标成员,位置:K-uploadImgMember(),成员id:${c.id},群聊id:${c.groupid}"
            ) { _, temp1 ->
                uploadImgAndId(file, temp1, "E3", "E4")
            }
            else -> {
                "EA"
            }
        }
    }

    suspend fun queryImgInfo(id: String, size: Long?, width: Int?, height: Int?, type: Int?): String {
        return try {
            val tmp = Image.newBuilder(id).apply {
                this.size = size ?: 0L
                this.width = width ?: 0
                this.height = height ?: 0
                this.type = ImageType.values()[type ?: ImageType.UNKNOWN.ordinal]
            }.build()
            gson.toJson(
                Config.ImgInfo(
                    md5 = gson.toJson(tmp.md5),
                    size = tmp.size,
                    url = tmp.queryUrl(),
                    width = tmp.width,
                    height = tmp.height,
                    type = tmp.imageType.ordinal
                )
            )
        } catch (e: IllegalArgumentException) {
            "E1"
        }
    }

    //recall
    suspend fun recallMsg(a: String): String {
        val source = json.decodeFromString(MessageSource.Serializer, a)
        try {
            source.recall()
        } catch (e: PermissionDeniedException) {
            logger.error("机器人无权限撤回")
            return "EP"
        } catch (e: IllegalStateException) {
            logger.error("该消息已被撤回")
            return "E2"
        }
        return "Y"
    }

    //禁言
    suspend fun mute(time: Int, c: Config.Contact): String = c.withBot { bot ->
        c.withMember(
            bot, "禁言找不到对应群组，位置K-mute()，gid:${c.groupid}", "禁言找不到对应群成员，位置K-mute()，id:${c.id}, gid:${c.id}"
        ) { _, member ->
            try {
                if (time > 0) member.mute(time)
                else member.unmute()
            } catch (e: PermissionDeniedException) {
                logger.error("执行禁言失败机器人无权限，位置:K-mute()，目标群id:${c.groupid}，目标成员id:${c.id}")
                return "EP"
            } catch (e: IllegalStateException) {
                logger.error("执行禁言失败禁言时间超出0s~30d，位置:K-mute()，时间:$time")
                return "E4"
            }
            return "Y"
        }
    }

    suspend fun uploadVoice(path: String, c: Config.Contact): String = c.withBot { bot ->
        val file = File(path)
        if (!file.exists() || !file.isFile || !(file.extension == "amr" || file.extension == "silk")) {
            logger.error("上传的语言文件需为.amr / .silk文件, 位置: KUploadVoice")
            return "E1"
        }
        val cc = when (c.type) {
            1 -> bot.getFriend(c.id) ?: let { logger.error("上传语音找不到好友, id:${c.id}"); return "EF" }
            2 -> bot.getGroup(c.id) ?: let { logger.error("上传语音找不到群聊, gid:${c.id}"); return "EG" }
            else -> return "EA"
        }
        return file.toExternalResource().use {
            try {
                json.encodeToString(MessageSource.Serializer, cc.uploadAudio(it).sendTo(cc).source)
            } catch (e: OverFileSizeMaxException) {
                logger.error("上传语音失败, 文件应在大约1MB以内, 实际大小:${it.size}, 文件路径:${file.absolutePath}")
                "E2"
            }
        }
    }

    private suspend fun fileInfo0(temp: AbsoluteFile): String {
        return gson.toJson(
            Config.FileInfo(
                id = temp.id,
                name = temp.name,
                path = temp.absolutePath,
                dinfo = Config.DInfo(temp.getUrl() ?: "null", temp.md5.toString(), temp.sha1.toString()),
                finfo = Config.FInfo(
                    temp.size, temp.uploaderId, temp.expiryTime, temp.uploadTime, temp.lastModifiedTime
                )
            )
        )
    }

    suspend fun sendFile(path: String, file: String, c: Config.Contact): String = c.withBot { bot ->
        c.withGroup(bot, "找不到对应群组，位置K-uploadfile()，gid:${c.id}") { group ->
            val f = File(file)
            if (!f.exists() || !f.isFile) {
                return "E2"
            }
            val tmp = try {
                f.toExternalResource().use {
                    group.files.root.uploadNewFile(path, it)
                }
            } catch (e: PermissionDeniedException) {
                return "EP"
            } catch (e: IllegalStateException) {
                return "E3"
            } catch (e: Exception) {
                logger.error(e.message)
                e.printStackTrace()
                return "E3"
            }
            return fileInfo0(tmp)
        }
    }

    private suspend fun remoteFileList(path: String, c: Config.Contact): String = c.withBot { bot ->
        c.withGroup(bot, "找不到对应群组，位置K-remoteFileInfo，gid:${c.id}") { group ->
            var tmp = "["
            group.files.root.resolveFiles(path).toList().forEach {
                tmp += "[\"${it.absolutePath}\", \"${it.id}\"],"
            }
            tmp = tmp.substring(0, tmp.length - 1)
            tmp += "]"
            return tmp
        }
    }

    private suspend fun remoteFileInfo0(path: String, c: Config.Contact): String = c.withBot { bot ->
        c.withGroup(bot, "找不到对应群组，位置K-remoteFileInfo0，gid:${c.id}") { group ->
            val tmp = try {
                group.files.root.resolveFiles(path).first()
            } catch (e: NoSuchElementException) {
                logger.error("cannot find the file,位置:K-remoteFileinfo0, path: $path")
                return "E2"
            }
            if (!tmp.isFile || !tmp.exists()) {
                logger.error("cannot find the file,位置:K-remoteFileinfo0, path: $path")
                return "E2"
            }
            return fileInfo0(tmp)
        }
    }

    suspend fun remoteFileInfo(path: String, id: String, c: Config.Contact): String = c.withBot { bot ->
        if (id == "") return remoteFileInfo0(path, c)
        if (id == "-1") return remoteFileList(path, c)
        c.withGroup(bot, "找不到对应群组，位置K-remoteFileInfo，gid:${c.id}") { group ->
            group.files.root.resolveFolder(path).let {
                if (it == null) remoteFileInfo0(path, c)
                else fileInfo0(it.resolveFileById(id) ?: let {
                    logger.error("cannot find the file,位置:K-remoteFileinfo, id:$id, path:$path")
                    return "E2"
                })
            }
        }
    }

    suspend fun remoteFileInfo(id: String, c: Config.Contact): String = c.withBot { bot ->
        c.withGroup(bot, "找不到对应群组，位置K-remoteFileInfo，gid:${c.id}") { group ->
            val tmp = group.files.root.resolveFileById(id) ?: let {
                logger.error("cannot find the file,位置:K-remoteFileinfo, id:$id")
                return "E1"
            }
            return fileInfo0(tmp)
        }
    }

    //查询权限
    fun kqueryM(c: Config.Contact): String = c.withBot { bot ->
        c.withMember(
            bot, "查询权限找不到对应群组，位置K-queryM()，gid:${c.groupid}", "查询权限找不到对应群成员，位置K-queryM()，id:${c.id}, gid:${c.groupid}"
        ) { _, member ->
            return member.permission.level.toString()
        }
    }

    suspend fun kkick(message: String, c: Config.Contact): String = c.withBot { bot ->
        c.withMember(
            bot, "查询权限找不到对应群组，位置K-queryM()，gid:${c.groupid}", "查询权限找不到对应群成员，位置K-queryM()，id:${c.id}, gid:${c.id}"
        ) { _, member ->
            try {
                member.kick(message)
            } catch (e: PermissionDeniedException) {
                return "EP"
            }
            return "Y"
        }
    }

    //取群主
    fun getowner(c: Config.Contact): String = c.withBot {
        c.withGroup(it, "找不到群,位置:K-getowner,gid:${c.id}") { g ->
            return g.owner.id.toString()
        }
    }

    private fun buildForwardMsg(text: String, bid: Long, display: Config.ForwardedMessageDisplay?): ForwardMessage {
        val t = Gson().fromJson(text, Config.ForwardMessageJson.Content::class.java)
        val a = mutableListOf<ForwardMessage.Node>()
        t.value.forEach {
            if (it.isForwardedMessage != true) a.add(
                ForwardMessage.Node(
                    it.id, it.time, it.name, MiraiCode.deserializeMiraiCode(it.message)
                )
            )
            else {
                a.add(ForwardMessage.Node(it.id, it.time, it.name, buildForwardMsg(it.message, bid, it.display)))
            }
        }
        return RawForwardMessage(a).render(if (display != null) DisplayS(display) else ForwardMessage.DisplayStrategy.Default)
    }

    //构建聊天记录
    suspend fun sendForwardMsg(text: String, bid: Long): String = withBot(bid) { bot ->
        val t = Gson().fromJson(text, Config.ForwardMessageJson::class.java)
        val c: Contact = when (t.type) {
            1 -> bot.getFriend(t.id) ?: let {
                return "EF"
            }
            2 -> bot.getGroup(t.id) ?: let {
                return "EG"
            }
            3 -> (bot.getGroup(t.id) ?: let {
                return "EM"
            })[t.groupid] ?: let {
                return "EMM"
            }
            else -> return "EA"
        }
        val tmp = try {
            buildForwardMsg(gson.toJson(t.content), bid, t.display)
        } catch (err: IllegalStateException) {
            return@withBot err.message!!
        }
        sendWithCatch {
            tmp.sendTo(c).source
        }
    }

    suspend fun accpetFriendRequest(info: String, botid: Long, accept: Boolean, ban: Boolean?): String =
        withBot(botid) { bot ->
            try {
                if (accept) gson.fromJson(info, RequestEventData.NewFriendRequest::class.java).accept(bot)
                else gson.fromJson(info, RequestEventData.NewFriendRequest::class.java).reject(bot, ban ?: false)
            } catch (e: IllegalStateException) {
                return "E"
            }
            return "Y"
        }

    suspend fun accpetGroupInvite(info: String, botid: Long, accept: Boolean): String = withBot(botid) { bot ->
        try {
            if (accept) gson.fromJson(info, RequestEventData.BotInvitedJoinGroupRequest::class.java).accept(bot)
            else gson.fromJson(info, RequestEventData.BotInvitedJoinGroupRequest::class.java).reject(bot)
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
        val bot = Bot.getInstanceOrNull(source.botId) ?: let {
            return "EB"
        }
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
        return sendWithCatch { c.sendMessage(source.quote() + message).source }
    }

    fun groupSetting(c: Config.Contact, source: String): String = c.withBot {
        c.withGroup(it) { group ->
            val root = gson.fromJson(source, Config.GroupSetting::class.java)
            try {
                group.name = root.name
                group.settings.isMuteAll = root.isMuteAll
                group.settings.isAllowMemberInvite = root.isAllowMemberInvite
            } catch (e: PermissionDeniedException) {
                return "EP"
            }
            return "Y"
        }
    }

    suspend fun deleteOnlineAnnouncement(a: Config.IdentifyA): String = withBot(a.botid) { bot ->
        withGroup(bot, a.groupid) { group ->
            try {
                group.announcements.delete(a.fid!!).let {
                    if (!it) return "E1"
                }
            } catch (e: PermissionDeniedException) {
                return "EP"
            } catch (e: IllegalStateException) {
                return "E3"
            }
            return "Y"
        }
    }

    suspend fun publishOfflineAnnouncement(i: Config.IdentifyA, a: Config.BriefOfflineA): String =
        withBot(i.botid) { bot ->
            withGroup(bot, i.groupid) { g ->
                OfflineAnnouncement.create(a.content, buildAnnouncementParameters {
                    image = null
                    sendToNewMember = a.params.sendToNewMember
                    isPinned = a.params.isPinned
                    showEditCard = a.params.showEditCard
                    showPopup = a.params.showPopup
                    requireConfirmation = a.params.requireConfirmation
                }).let {
                    return try {
                        gson.toJson(it.publishTo(g).toOnlineA())
                    } catch (e: PermissionDeniedException) {
                        "EP"
                    }
                }
            }
        }

    //定时任务
    fun scheduling(time: Long, msg: String): String {
        Timer("Timer", true).schedule(time) {
            event(CPPEvent.TimeOutEvent(msg))
        }
        return "Y"
    }

    suspend fun sendNudge(c: Config.Contact): String = c.withBot { bot ->
        return when (c.type) {
            1 -> {
                c.withFriend(bot) { f ->
                    try {
                        f.nudge().sendTo(f)
                    } catch (e: UnsupportedOperationException) {
                        logger.error("发送nudge必须使用ANDROID_PHONE/ipad协议，目前协议为:" + bot.configuration.protocol.name)
                        return "E1"
                    }
                    "Y"
                }
            }
            3 -> {
                c.withMember(bot) { g, m ->
                    try {
                        m.nudge().sendTo(g)
                    } catch (e: UnsupportedOperationException) {
                        logger.error("发送nudge必须使用ANDROID_PHONE/ipad协议，目前协议为:" + bot.configuration.protocol.name)
                        return "E1"
                    }
                    "Y"
                }
            }
            else -> "EA"
        }
    }

    suspend fun modifyAdmin(c: Config.Contact, b: Boolean): String = c.withBot { bot ->
        c.withMember(bot) { _, member ->
            try {
                member.modifyAdmin(b)
            } catch (e: PermissionDeniedException) {
                return "EP"
            }
            "Y"
        }
    }

    suspend fun memberJoinRequest(source: String, b: Boolean, botid: Long, msg: String): String =
        withBot(botid) { bot ->
            return gson.fromJson(source, RequestEventData.MemberJoinRequest::class.java).let {
                if (b) it.accept(bot)
                else it.reject(bot, msg)
                "Y"
            }
        }

    suspend fun isUploaded(img: Config.ImgInfo, botid: Long): String = withBot(botid) { bot ->
        img.toImage().isUploaded(bot).toString()
    }

    fun changeNameCard(c: Config.Contact, name: String): String = c.withMiraiMember { _, _, normalMember ->
        try {
            normalMember.nameCard = name
        } catch (_: PermissionDeniedException) {
            return@withMiraiMember "EP"
        }
        "s"
    }

    fun onDisable() = cpp.forEach { it.PluginDisable() }

    @MiraiExperimentalApi
    fun onEnable(eventChannel: EventChannel<Event>) {
        //配置文件目录 "${dataFolder.absolutePath}/"
        eventChannel.subscribeAlways<FriendMessageEvent> {
            //好友信息
            event(
                CPPEvent.PrivateMessage(
                    this.sender.toContact(), this.message.serializeToMiraiCode(), json.encodeToString(
                        MessageSource.Serializer, this.message[MessageSource]!!
                    )
                )
            )
        }
        eventChannel.subscribeAlways<GroupMessageEvent> {
            //群消息
            event(
                CPPEvent.GroupMessage(
                    this.group.toContact(),
                    Config.Contact(
                        3, this.sender.id, this.group.id, this.senderName, this.bot.id, (this.sender is AnonymousMember)
                    ),
                    this.message.serializeToMiraiCode(),
                    json.encodeToString(MessageSource.Serializer, this.message[MessageSource]!!)
                )
            )
        }
        eventChannel.subscribeAlways<MemberLeaveEvent.Kick> {
            friend_cache.add(this.member)
            event(
                CPPEvent.MemberLeave(
                    this.group.toContact(),
                    this.member.id,
                    1,
                    if (this.operator?.id == null) this.bot.id else this.operator!!.id
                )

            )
            friend_cache.remove(this.member)
        }
        eventChannel.subscribeAlways<MemberLeaveEvent.Quit> {
            friend_cache.add(this.member)
            event(
                CPPEvent.MemberLeave(
                    this.group.toContact(), this.member.id, 2, this.member.id
                )

            )
            friend_cache.remove(this.member)
        }
        eventChannel.subscribeAlways<MemberJoinEvent.Retrieve> {
            event(
                CPPEvent.MemberJoin(
                    this.group.toContact(),
                    Config.Contact(3, this.member.id, this.group.id, this.member.nameCardOrNick, this.bot.id),
                    3,
                    this.member.id
                )

            )
        }
        eventChannel.subscribeAlways<MemberJoinEvent.Active> {
            event(
                CPPEvent.MemberJoin(
                    this.group.toContact(),
                    Config.Contact(3, this.member.id, this.group.id, this.member.nameCardOrNick, this.bot.id),
                    2,
                    this.member.id
                )

            )
        }
        eventChannel.subscribeAlways<MemberJoinEvent.Invite> {
            event(
                CPPEvent.MemberJoin(
                    this.group.toContact(),
                    Config.Contact(3, this.member.id, this.group.id, this.member.nameCardOrNick, this.bot.id),
                    1,
                    this.invitor.id
                )

            )
        }
        eventChannel.subscribeAlways<NewFriendRequestEvent> {
            //自动同意好友申请
            event(
                CPPEvent.NewFriendRequest(
                    CPPEvent.NewFriendRequest.NewFriendRequestSource(
                        this.bot.id, this.eventId, this.message, this.fromId, this.fromGroupId, this.fromNick
                    ), gson.toJson(this.toRequestEventData())
                )

            )

        }
        eventChannel.subscribeAlways<MessageRecallEvent.FriendRecall> {
            event(
                CPPEvent.RecallEvent(
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
        }
        eventChannel.subscribeAlways<MessageRecallEvent.GroupRecall> {
            event(
                CPPEvent.RecallEvent(
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

        }
        eventChannel.subscribeAlways<BotJoinGroupEvent.Invite> {
            event(
                CPPEvent.BotJoinGroup(
                    1, this.group.toContact(), this.invitor.id
                )

            )
        }
        eventChannel.subscribeAlways<BotJoinGroupEvent.Active> {
            event(
                CPPEvent.BotJoinGroup(
                    2, this.group.toContact(), 0
                )

            )
        }
        eventChannel.subscribeAlways<BotJoinGroupEvent.Retrieve> {
            event(
                CPPEvent.BotJoinGroup(
                    3, this.group.toContact(), 0
                )

            )
        }
        eventChannel.subscribeAlways<BotInvitedJoinGroupRequestEvent> {
            //自动同意加群申请
            event(
                CPPEvent.GroupInvite(
                    CPPEvent.GroupInvite.GroupInviteSource(
                        this.bot.id, this.eventId, this.invitorId, this.groupId, this.groupName, this.invitorNick
                    ), gson.toJson(this.toRequestEventData())
                )

            )
        }
        eventChannel.subscribeAlways<GroupTempMessageEvent> {
            //群临时会话
            event(
                CPPEvent.GroupTempMessage(
                    this.group.toContact(),
                    this.sender.toContact(),
                    this.message.serializeToMiraiCode(),
                    json.encodeToString(
                        MessageSource.Serializer, this.source
                    )
                )
            )
        }

        eventChannel.subscribeAlways<NudgeEvent> {
            event(
                CPPEvent.NugdeEvent(
                    this.from.toContact() ?: return@subscribeAlways,
                    this.target.toContact() ?: return@subscribeAlways,
                    this.subject.toContact() ?: return@subscribeAlways,
                    this.bot.id
                )

            )
        }
        eventChannel.subscribeAlways<BotLeaveEvent> {
            event(
                CPPEvent.BotLeaveEvent(
                    this.group.id, this.bot.id
                )

            )
        }
        eventChannel.subscribeAlways<MemberJoinRequestEvent> {
            event(
                CPPEvent.MemberJoinRequestEvent(
                    this.group?.toContact() ?: emptyContact(this.bot.id),
                    this.invitor?.toContact() ?: emptyContact(this.bot.id),
                    this.fromId,
                    gson.toJson(this.toRequestEventData())
                )
            )
        }
        eventChannel.subscribeAlways<MessagePreSendEvent> {
            val t = this.target.toContact() ?: return@subscribeAlways
            event(
                CPPEvent.MessagePreSendEvent(t, this.bot.id, this.message.toMessageChain().serializeToJsonString())
            )
        }
    }
}