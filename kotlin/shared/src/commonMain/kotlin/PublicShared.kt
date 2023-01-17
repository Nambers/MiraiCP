/*
 * Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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

import kotlinx.coroutines.TimeoutCancellationException
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.flow.toList
import kotlinx.coroutines.runBlocking
import kotlinx.serialization.decodeFromString
import kotlinx.serialization.encodeToString
import kotlinx.serialization.serializer
import net.mamoe.mirai.Bot
import net.mamoe.mirai.contact.*
import net.mamoe.mirai.contact.announcement.OfflineAnnouncement
import net.mamoe.mirai.contact.announcement.OnlineAnnouncement
import net.mamoe.mirai.contact.announcement.bot
import net.mamoe.mirai.contact.announcement.buildAnnouncementParameters
import net.mamoe.mirai.contact.file.AbsoluteFile
import net.mamoe.mirai.data.RequestEventData
import net.mamoe.mirai.event.Event
import net.mamoe.mirai.event.EventChannel
import net.mamoe.mirai.event.EventPriority
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.message.MessageSerializers
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
import net.mamoe.mirai.utils.use
import tech.eritquearcus.miraicp.shared.Packets.Utils.toEventData
import tech.eritquearcus.miraicp.shared.PublicSharedData.logger
import tech.eritquearcus.miraicp.shared.UlitsMultiPlatform.event
import tech.eritquearcus.miraicp.uilts.MiraiCPFile
import tech.eritquearcus.miraicp.uilts.MiraiCPFiles

expect object PublicSharedMultiplatform {
    fun onDisable()
}

object PublicSharedData {
    var _logger: MiraiLogger? = null
    val logger: MiraiLogger
        get() {
            if (_logger == null) {
                _logger = MiraiLogger.Factory.create(this::class, "MiraiCP")
            }
            return _logger!!
        }

    // cannot modify after init
    lateinit var commandReg: CommandHandler
    val cachePath: MiraiCPFile by lazy {
        MiraiCPFiles.create("cache")
    }
}

object PublicShared {
    val friend_cache = ArrayList<NormalMember>(0)
    val logger4plugins: MutableMap<String, MiraiLogger> = mutableMapOf()
    const val now_tag = "v${BuiltInConstants.version}"

    fun exit() {
        onDisable()
        logger.info("Closing MiraiCP...")
        Bot.instances.forEach {
            runBlocking {
                it.closeAndJoin()
            }
            logger.info("Bot ${it.id} closed")
        }
    }

    suspend fun nextMsg(source: String): String = withData(source, Packets.Incoming.NextMsg.serializer()) { data ->
        return runBlocking {
            val e = try {
                when (data.contact.type) {
                    1 -> nextMessage<FriendMessageEvent>(data.time, data.halt, EventPriority.HIGHEST) {
                        it.sender.id == data.contact.id && it.bot.id == data.contact.botId
                    }

                    2 -> nextMessage<GroupMessageEvent>(data.time, data.halt, EventPriority.HIGHEST) {
                        it.group.id == data.contact.id && it.bot.id == data.contact.botId
                    }

                    3 -> nextMessage<GroupMessageEvent>(data.time, data.halt, EventPriority.HIGHEST) {
                        it.bot.id == data.contact.botId && it.group.id == data.contact.groupId && it.sender.id == data.contact.id
                    }

                    else -> throw Exception() //unreachable
                }
            } catch (e: TimeoutCancellationException) {
                return@runBlocking "E1"
            }
            json.encodeToString(
                Packets.Outgoing.NextMsg(
                    e.serializeToJsonString(),
                    json.encodeToString(
                        MessageSerializers.serializersModule.serializer(), e[MessageSource]!!
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

    private suspend fun send0(message: Message, c: Packets.Contact): String = c.withBot { AIbot ->
        return when (c.type) {
            1 -> {
                logger.info("Send message for(${c.id}) is $message")
                c.withFriend(AIbot, "发送消息找不到好友，位置:K-Send()，id:${c.id}") {
                    sendWithCatch { it.sendMessage(message).source }
                }

            }

            2 -> {
                logger.info("Send message for Group(${c.id}) is $message")
                c.withGroup(AIbot, "发送群消息异常找不到群组，位置K-SendG，gid:${c.id}") {
                    sendWithCatch { it.sendMessage(message).source }
                }
            }

            3 -> {
                logger.info("Send message for a member(${c.id}) is $message")
                c.withMember(
                    AIbot,
                    "发送消息找不到群聊，位置K-Send()，id:${c.groupId}",
                    "发送消息找不到群成员，位置K-Send()，id:${c.id}，gid:${c.groupId}"
                ) { _, m ->
                    sendWithCatch { m.sendMessage(message).source }
                }
            }

            else -> "EA"
        }
    }

    suspend fun sendMsg(source: String): String = withData(source, Packets.Incoming.SendMessage.serializer()) { data ->
        send0(MessageChain.deserializeFromJsonString(data.message), data.contact)
    }

    private fun OnlineAnnouncement.toOnlineA(): Packets.Incoming.OnlineAnnouncement {
        return Packets.Incoming.OnlineAnnouncement(
            this.content,
            this.fid,
            this.parameters.image?.id ?: let { "" },
            this.confirmedMembersCount,
            this.senderId,
            this.group.id,
            this.bot.id,
            this.publicationTime,
            Packets.Incoming.AnnouncementParams(
                this.parameters.sendToNewMember,
                this.parameters.isPinned,
                this.parameters.showEditCard,
                this.parameters.showPopup,
                this.parameters.requireConfirmation
            )
        )
    }

    @OptIn(MiraiExperimentalApi::class)
    suspend fun refreshInfo(source: String): String =
        withData(source, Packets.Incoming.RefreshInfo.serializer()) { data ->
            data.contact.withBot { bot ->
                when (data.contact.type) {
                    1 -> data.contact.withFriend(
                        bot,
                        "找不到对应好友，位置:K-GetNickOrNameCard()，id:${data.contact.id}"
                    ) { f ->
                        if (data.quit) {
                            f.delete()
                            return "done"
                        }
                        json.encodeToString(Packets.Incoming.RefreshInfo.ContactInfo(f.nick, f.avatarUrl))
                    }

                    2 -> data.contact.withGroup(
                        bot,
                        "取群名称找不到群,位置K-GetNickOrNameCard(), gid:${data.contact.id}"
                    ) { g ->
                        if (data.announcement) return json.encodeToString(
                            g.announcements.toList().map { it.toOnlineA() })
                        if (data.quit) {
                            g.quit()
                            return "done"
                        }
                        json.encodeToString(
                            Packets.Incoming.RefreshInfo.ContactInfo(
                                g.name, g.avatarUrl, Packets.Incoming.RefreshInfo.ContactInfo.GroupSetting(
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
                        a.id == data.contact.id && a.group.id == data.contact.groupId
                    }?.let {
                        json.encodeToString(Packets.Incoming.RefreshInfo.ContactInfo(it.nameCardOrNick, it.avatarUrl))
                    } ?: let {
                        data.contact.withMember(
                            bot,
                            "取群名片找不到对应群组，位置K-GetNickOrNameCard()，gid:${data.contact.groupId}",
                            "取群名片找不到对应群成员，位置K-GetNickOrNameCard()，id:${data.contact.id}, gid:${data.contact.groupId}"
                        ) { _, m ->
                            json.encodeToString(Packets.Incoming.RefreshInfo.ContactInfo(m.nameCardOrNick, m.avatarUrl))
                        }
                    }

                    4 -> json.encodeToString(Packets.Incoming.RefreshInfo.ContactInfo(bot.nick, bot.avatarUrl))
                    else -> "EA"
                }
        }
    }

    suspend fun queryBotList(source: String): String =
        withData(source, Packets.Incoming.GetList.serializer()) { data ->
            data.contact.withBot { bot ->
                when (data.type) {
                    0 -> json.encodeToString(bot.friends.map { it.id })
                    1 -> json.encodeToString(bot.groups.map { it.id })
                    2 -> data.contact.withGroup(bot) { g ->
                        json.encodeToString(g.members.map { it.id })
                    }

                    else -> "EA"
                }
            }
        }

    //图片部分实现
    private suspend fun uploadImgAndId(file: String, temp: Contact, err1: String = "E2", err2: String = "E3"): String =
        try {
            val f = MiraiCPFiles.create(file).toExternalResource()
            val img = f.uploadAsImage(temp)
            f.close()
            json.encodeToString(
                Packets.Outgoing.ImgInfo(
                    size = img.size,
                    width = img.width,
                    height = img.height,
                    md5 = json.encodeToString(img.md5),
                    url = img.queryUrl(),
                    imageId = img.imageId,
                    imageType = img.imageType.name,
                    isEmoji = img.isEmoji,
                )
            )
        } catch (e: OverFileSizeMaxException) {
            logger.error("图片文件过大超过30MB,位置:K-uploadImgGroup(),文件名:$file")
            err1
        } catch (e: NullPointerException) {
            logger.error("上传图片文件名异常,位置:K-uploadImgGroup(),文件名:$file")
            err2
        }

    suspend fun uploadImg(source: String): String =
        withData(source, Packets.Incoming.UploadImage.serializer()) { data ->
            data.contact.withBot { bot ->
                when (data.contact.type) {
                    1 -> data.contact.withFriend(
                        bot,
                        "发送图片找不到对应好友,位置:K-uploadImgFriend(),id:${data.contact.id}"
                    ) { temp ->
                        uploadImgAndId(data.filePath, temp)
                    }

                    2 -> data.contact.withGroup(
                        bot,
                        "发送图片找不到对应群组,位置:K-uploadImgGroup(),id:${data.contact.id}"
                    ) { temp ->
                        uploadImgAndId(data.filePath, temp)
                    }

                    3 -> data.contact.withMember(
                        bot,
                        "发送图片找不到对应群组,位置:K-uploadImgGroup(),id:${data.contact.groupId}",
                        "发送图片找不到目标成员,位置:K-uploadImgMember(),成员id:${data.contact.id},群聊id:${data.contact.groupId}"
                    ) { _, temp1 ->
                        uploadImgAndId(data.filePath, temp1, "E3", "E4")
                    }

                    else -> {
                        "EA"
                    }
                }
        }
    }

    suspend fun queryImgInfo(source: String): String =
        withData(source, Packets.Outgoing.ImgInfo.serializer()) { image ->
            return try {
                val tmp = image.toImage()
                json.encodeToString(
                    Packets.Outgoing.ImgInfo(
                        md5 = json.encodeToString(tmp.md5),
                        size = tmp.size,
                        url = tmp.queryUrl(),
                        width = tmp.width,
                        height = tmp.height,
                        imageType = tmp.imageType.name,
                        isEmoji = tmp.isEmoji,
                    )
            )
        } catch (e: IllegalArgumentException) {
            "E1"
        }
    }

    //recall
    suspend fun recallMsg(source: String): String = withData(source, MessageSource.serializer()) { ms ->
        try {
            ms.recall()
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
    suspend fun muteMember(source: String): String =
        withData(source, Packets.Incoming.MuteMember.serializer()) { data ->
            data.contact.withBot { bot ->
                data.contact.withMember(
                    bot,
                    "禁言找不到对应群组，位置K-mute()，gid:${data.contact.groupId}",
                    "禁言找不到对应群成员，位置K-mute()，id:${data.contact.id}, gid:${data.contact.id}"
                ) { _, member ->
                    try {
                        if (data.time > 0) {
                            member.mute(data.time.toInt())
                        } else {
                            member.unmute()
                        }
                    } catch (e: PermissionDeniedException) {
                        logger.error("执行禁言失败机器人无权限，位置:K-mute()，目标群id:${data.contact.groupId}，目标成员id:${data.contact.id}")
                        return "EP"
                    } catch (e: IllegalStateException) {
                        logger.error("执行禁言失败禁言时间超出0s~30d，位置:K-mute()，时间:$data.time")
                        return "E4"
                    }
                    return "Y"
                }
            }
        }

    suspend fun uploadVoice(source: String): String =
        withData(source, Packets.Incoming.SendVoice.serializer()) { data ->
            data.contact.withBot { bot ->
                val file = MiraiCPFiles.create(data.path)
                if (!file.exists() || !file.isFile || !(file.extension == "amr" || file.extension == "silk")) {
                    logger.error("上传的语言文件需为.amr / .silk文件, 位置: KUploadVoice")
                    return "E1"
                }
                val cc = when (data.contact.type) {
                    1 -> {
                        require(data.contact.id != 0L) { "id is required to upload voice" }
                        bot.getFriend(data.contact.id)
                            ?: let { logger.error("上传语音找不到好友, id:${data.contact.id}"); return "EF" }
                    }

                    2 -> {
                        require(data.contact.id != 0L) { "id is required to upload voice for group" }
                        bot.getGroup(data.contact.id)
                            ?: let { logger.error("上传语音找不到群聊, gid:${data.contact.id}"); return "EG" }
                    }

                    else -> return "EA"
                }
                return file.readByteArray().toExternalResource().use {
                    try {
                        json.encodeToString(
                            MessageSerializers.serializersModule.serializer(),
                            cc.uploadAudio(it).sendTo(cc).source as MessageSource
                        )
                    } catch (e: OverFileSizeMaxException) {
                        logger.error("上传语音失败, 文件应在大约1MB以内, 实际大小:${it.size}, 文件路径:${file.absolutePath}")
                        "E2"
                    }
                }
            }
        }

    private suspend fun fileInfo0(temp: AbsoluteFile): String {
        return json.encodeToString(
            Packets.Outgoing.FileInfo(
                id = temp.id,
                name = temp.name,
                path = temp.absolutePath,
                downloadInfo = Packets.Outgoing.FileInfo.DownloadInfo(
                    temp.getUrl() ?: "null",
                    temp.md5.toString(),
                    temp.sha1.toString()
                ),
                detailInfo = Packets.Outgoing.FileInfo.DetailInfo(
                    temp.size, temp.uploaderId, temp.expiryTime, temp.uploadTime, temp.lastModifiedTime
                )
            )
        )
    }

    suspend fun sendFile(source: String): String = withData(source, Packets.Incoming.SendFile.serializer()) { data ->
        data.contact.withBot { bot ->
            data.contact.withGroup(bot, "找不到对应群组，位置K-uploadfile()，gid:${data.contact.id}") { group ->
                val f = MiraiCPFiles.create(data.filePath)
                if (!f.exists() || !f.isFile) {
                    return "E2"
                }
                val tmp = try {
                    f.toExternalResource().use {
                        group.files.root.uploadNewFile(data.path, it)
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
    }

    private suspend fun remoteFileList(path: String, c: Packets.Contact): String = c.withBot { bot ->
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

    private suspend fun remoteFileInfoPath(path: String, c: Packets.Contact): String = c.withBot { bot ->
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

    suspend fun remoteFileInfo(source: String): String =
        withData(source, Packets.Incoming.GetFile.serializer()) { data ->
            data.contact.withBot { bot ->
                if (data.id == "") return remoteFileInfoPath(data.path, data.contact)
                if (data.id == "-1") return remoteFileList(data.path, data.contact)
                if (data.path == "-1") return remoteFileInfoId(data.id, data.contact)
                data.contact.withGroup(bot, "找不到对应群组，位置K-remoteFileInfo，gid:${data.id}") { group ->
                    group.files.root.resolveFolder(data.path).let {
                        if (it == null) remoteFileInfoPath(data.path, data.contact)
                        else fileInfo0(it.resolveFileById(data.id) ?: let {
                            logger.error("cannot find the file,位置:K-remoteFileinfo, id:${data.id}, path:${data.path}")
                            return "E2"
                        })
                    }
                }
            }
        }

    suspend fun remoteFileInfoId(id: String, c: Packets.Contact): String = c.withBot { bot ->
        c.withGroup(bot, "找不到对应群组，位置K-remoteFileInfo，gid:${c.id}") { group ->
            val tmp = group.files.root.resolveFileById(id) ?: let {
                logger.error("cannot find the file,位置:K-remoteFileinfo, id:$id")
                return "E1"
            }
            return fileInfo0(tmp)
        }
    }

    //查询权限
    suspend fun queryPermission(source: String): String = withData(source, Packets.Contact.serializer()) { c ->
        c.withBot { bot ->
            c.withMember(
                bot,
                "查询权限找不到对应群组，位置K-queryM()，gid:${c.groupId}",
                "查询权限找不到对应群成员，位置K-queryM()，id:${c.id}, gid:${c.groupId}"
            ) { _, member ->
                return member.permission.level.toString()
            }
        }
    }

    suspend fun kickMember(source: String): String =
        withData(source, Packets.Incoming.KickMember.serializer()) { data ->
            data.contact.withBot { bot ->
                data.contact.withMember(
                    bot,
                    "查询权限找不到对应群组，位置K-queryM()，gid:${data.contact.groupId}",
                    "查询权限找不到对应群成员，位置K-queryM()，id:${data.contact.id}, gid:${data.contact.id}"
                ) { _, member ->
                    try {
                        member.kick(data.message)
                    } catch (e: PermissionDeniedException) {
                        return "EP"
                    }
                    return "Y"
                }
            }
        }

    //取群主
    suspend fun getOwner(source: String): String = withData(source, Packets.Contact.serializer()) { c ->
        c.withBot {
            c.withGroup(it, "找不到群,位置:K-getowner,gid:${c.id}") { g ->
                return g.owner.id.toString()
            }
        }
    }

    //构建聊天记录
    suspend fun sendForwardMsg(source: String): String =
        withData(source, Packets.Incoming.SendForwarded.serializer()) { data ->
            withBot(data.contact.botId) { bot ->
                val tmp = MessageChain.deserializeFromJsonString(data.msg)
                val block: suspend (Contact) -> String = { c ->
                    sendWithCatch {
                        tmp.sendTo(c).source
                    }
                }
                data.contact.withContact(bot) {
                    block(it)
                }
            }
        }

    suspend fun acceptFriendRequest(source: String): String =
        withData(source, Packets.Incoming.FriendOperation.serializer()) { data ->
            withBot(data.botId) { bot ->
                try {
                    json.decodeFromString<RequestEventData.NewFriendRequest>(data.source).apply {
                        if (data.sign) accept(bot)
                        else reject(bot, data.ban)
                    }
                } catch (e: IllegalStateException) {
                    return "E"
                }
                return "Y"
            }
        }

    suspend fun acceptGroupInvite(source: String): String =
        withData(source, Packets.Incoming.InviteOrRequestOperation.serializer()) { data ->
            withBot(data.botId) { bot ->
                try {
                    json.decodeFromString<RequestEventData.BotInvitedJoinGroupRequest>(data.source).apply {
                        if (data.sign) accept(bot)
                        else reject(bot)
                    }
                } catch (e: IllegalStateException) {
                    return "E"
                }
                return "Y"
            }
        }

    suspend fun sendWithQuote(sourceStr: String): String =
        withData(sourceStr, Packets.Incoming.QuoteReply.serializer()) { data ->
            val source =
                json.decodeFromString(
                    MessageSerializers.serializersModule.serializer<MessageSource>(),
                    data.messageSource
                )
            val message = MessageChain.deserializeFromJsonString(data.msg)
            val bot = Bot.getInstanceOrNull(data.contact.botId) ?: let {
                return "EB"
            }
            val block: suspend (Contact) -> String = { c ->
                sendWithCatch { c.sendMessage(source.quote() + message).source }
            }
            data.contact.withContact(bot) {
                block(it)
            }
        }

    suspend fun groupSetting(source: String): String =
        withData(source, Packets.Incoming.GroupSetting.serializer()) { data ->
            data.contact.withBot {
                data.contact.withGroup(it) { group ->
                    try {
                        group.name = data.name
                        group.settings.isMuteAll = data.isMuteAll
                        group.settings.isAllowMemberInvite = data.isAllowMemberInvite
                    } catch (e: PermissionDeniedException) {
                        return "EP"
                    }
                    return "Y"
            }
        }
    }

    suspend fun announcementOperation(source: String): String =
        withData(source, Packets.Incoming.AnnouncementOperation.serializer()) { data ->
            withBot(data.botId) { bot ->
                withGroup(bot, data.groupId) { group ->
                    when (data.type) {
                        1 -> {
                            try {
                                group.announcements.delete(data.fid!!).let {
                                    if (!it) return "E1"
                                }
                            } catch (e: PermissionDeniedException) {
                                return "EP"
                            } catch (e: IllegalStateException) {
                                return "E3"
                            }
                            return "Y"
                        }

                        2 -> {
                            OfflineAnnouncement.create(data.source!!.content, buildAnnouncementParameters {
                                image = null
                                sendToNewMember = data.source.params.sendToNewMember
                                isPinned = data.source.params.isPinned
                                showEditCard = data.source.params.showEditCard
                                showPopup = data.source.params.showPopup
                                requireConfirmation = data.source.params.requireConfirmation
                            }).let {
                                return try {
                                    json.encodeToString(it.publishTo(group).toOnlineA())
                                } catch (e: PermissionDeniedException) {
                                    "EP"
                                }
                            }
                        }

                        else -> {
                            "EA"
                        }
                    }
                }
            }
        }

    suspend fun sendNudge(source: String): String = withData(source, Packets.Contact.serializer()) { c ->
        c.withBot { bot ->
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
    }

    suspend fun modifyAdmin(source: String): String =
        withData(source, Packets.Incoming.ModifyAdmin.serializer()) { data ->
            data.contact.withBot { bot ->
                data.contact.withMember(bot) { _, member ->
                    try {
                        member.modifyAdmin(data.admin)
                    } catch (e: PermissionDeniedException) {
                        return "EP"
                    }
                    "Y"
                }
            }
        }

    suspend fun memberJoinRequest(source: String): String =
        withData(source, Packets.Incoming.InviteOrRequestOperation.serializer()) { data ->
            withBot(data.botId) { bot ->
                return json.decodeFromString<RequestEventData.MemberJoinRequest>(data.source).let {
                    if (data.sign) it.accept(bot)
                    else it.reject(bot, data.msg!!)
                    "Y"
                }
            }
        }

    suspend fun isUploaded(source: String): String =
        withData(source, Packets.Incoming.ImageInfoWithBot.serializer()) { data ->
            withBot(data.botId) { bot ->
                data.image.toImage().isUploaded(bot).toString()
            }
        }

    suspend fun changeNameCard(source: String): String =
        withData(source, Packets.Incoming.ChangeNameCard.serializer()) { data ->
            data.contact.withMiraiMember { _, _, normalMember ->
                try {
                    normalMember.nameCard = data.newName
                } catch (_: PermissionDeniedException) {
                    return@withMiraiMember "EP"
                }
                "s"
            }
        }

    suspend fun commandReg(source: String): String {
        PublicSharedData.commandReg.register(
            json.decodeFromString(
                source
            )
        )
        return "true"
    }

    fun onDisable() = PublicSharedMultiplatform.onDisable()

    @MiraiExperimentalApi
    fun onEnable(eventChannel: EventChannel<Event>) {
        eventChannel.subscribeAlways<FriendMessageEvent> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<GroupMessageEvent> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<GroupTempMessageEvent> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<StrangerMessageEvent> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<MemberLeaveEvent> {
            friend_cache.add(this.member as NormalMember)
            event(this.toEventData())
            friend_cache.remove(this.member)
        }
        eventChannel.subscribeAlways<MemberJoinEvent.Retrieve> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<MemberJoinEvent.Active> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<MemberJoinEvent.Invite> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<NewFriendRequestEvent> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<MessageRecallEvent.FriendRecall> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<MessageRecallEvent.GroupRecall> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<BotJoinGroupEvent.Invite> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<BotJoinGroupEvent.Active> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<BotJoinGroupEvent.Retrieve> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<BotInvitedJoinGroupRequestEvent> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<NudgeEvent> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<BotLeaveEvent> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<MemberJoinRequestEvent> {
            event(toEventData())
        }
        eventChannel.subscribeAlways<MessagePreSendEvent> {
            event(toEventData())
        }
    }
}