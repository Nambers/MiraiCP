package org.example.mirai.plugin
import com.google.gson.Gson
import kotlinx.coroutines.runBlocking
import kotlinx.serialization.json.Json
import net.mamoe.mirai.Bot
import net.mamoe.mirai.BotFactory
import net.mamoe.mirai.alsoLogin
import net.mamoe.mirai.contact.NormalMember
import net.mamoe.mirai.contact.PermissionDeniedException
import net.mamoe.mirai.contact.nameCardOrNick
import net.mamoe.mirai.event.broadcast
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.message.MessageSerializers
import net.mamoe.mirai.message.code.MiraiCode
import net.mamoe.mirai.message.data.*
import net.mamoe.mirai.message.data.Image.Key.queryUrl
import net.mamoe.mirai.message.data.MessageSource.Key.recall
import net.mamoe.mirai.utils.*
import net.mamoe.mirai.utils.ExternalResource.Companion.uploadAsImage
import net.mamoe.mirai.utils.MiraiLogger.Companion.setDefaultLoggerCreator
import org.example.mirai.plugin.KotlinMain.now_tag
import org.fusesource.jansi.AnsiConsole
import org.json.JSONObject
import java.io.File
import java.lang.Long.valueOf
import java.net.URL
import java.util.*
import kotlin.concurrent.schedule


object KotlinMain {
    private val json =Json{
        serializersModule = MessageSerializers.serializersModule
    }
    const val now_tag = "v2.4.4"
    private var friend_cache = ArrayList<NormalMember>(0)
    lateinit var dll_name:String
    private lateinit var AIbot: Bot
    private lateinit var cpp: CPP_lib
    private lateinit var logger:MiraiLogger
    //��־����ʵ��
    fun BasicSendLog(log: String) {
        logger.info(log)
    }

    fun SendWarning(log: String) {
        logger.warning(log)
    }

    fun SendError(log: String) {
        logger.error(log)
    }

    //������Ϣ����ʵ�� MiraiCode

    suspend fun Send(message: String, id: Long) :String{
        //�������
        logger.info("Send message for($id) is $message")
        val f = AIbot.getFriend(id) ?: let {
            logger.error("������Ϣ�Ҳ������ѣ�λ��:K-Send()��id:$id")
            return "E1"
        }
        return json.encodeToString(MessageSource.Serializer,
            f.sendMessage(MiraiCode.deserializeMiraiCode(message)).source)
    }

    suspend fun Send(message: String, id: Long, gid: Long):String {
        //�������
        logger.info("Send message for a member($id) is $message")
        for (a in friend_cache) {
            if (a.id == id && a.group.id == gid) {
                a.sendMessage(MiraiCode.deserializeMiraiCode(message, a))
                return json.encodeToString(MessageSource.Serializer,
                    a.sendMessage(MiraiCode.deserializeMiraiCode(message)).source)
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
        return json.encodeToString(MessageSource.Serializer, f.sendMessage(MiraiCode.deserializeMiraiCode(message)).source)
    }

    suspend fun SendG(message: String, id: Long):String {
        logger.info("Send message for Group($id) is $message")
        val g = AIbot.getGroup(id) ?: let {
            logger.error("����Ⱥ��Ϣ�쳣�Ҳ���Ⱥ�飬λ��K-SendG��gid:$id")
            return "E1"
        }
        return json.encodeToString(MessageSource.Serializer,
            g.sendMessage(MiraiCode.deserializeMiraiCode(message)).source)
    }

    //Msg

    suspend fun SendM(message: String, id: Long) :String{
        //�������
        logger.info("Send message for($id) is $message")
        val f = AIbot.getFriend(id) ?: let {
            logger.error("������Ϣ�Ҳ������ѣ�λ��:K-Send()��id:$id")
            return "E1"
        }
        return json.encodeToString(MessageSource.Serializer, f.sendMessage(message).source)
    }

    suspend fun SendM(message: String, id: Long, gid: Long):String {
        //�������
        logger.info("Send message for a member($id) is $message")
        for (a in friend_cache) {
            if (a.id == id && a.group.id == gid) {
                return json.encodeToString(MessageSource.Serializer, a.sendMessage(message).source)
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
        return json.encodeToString(MessageSource.Serializer, f.sendMessage(message).source)
    }

    suspend fun SendGM(message: String, id: Long):String {
        logger.info("Send message for Group($id) is $message")
        val g = AIbot.getGroup(id) ?: let {
            logger.error("����Ⱥ��Ϣ�쳣�Ҳ���Ⱥ�飬λ��K-SendG��gid:$id")
            return "E1"
        }
        return json.encodeToString(MessageSource.Serializer, g.sendMessage(message).source)
    }

    //ȡ�ǳƻ���Ƭ����
    fun GetN(qqid: Long): String {
        val f = AIbot.getFriend(qqid) ?: let {
            logger.error("�Ҳ�����Ӧ���ѣ�λ��:K-GetN()��id:$qqid")
            return "E1"
        }
        return f.nick
    }

    fun GetNN(qqid: Long, groupid: Long): String {
        for (a in friend_cache) {
            if (a.id == qqid && a.group.id == groupid) {
                return a.nameCardOrNick
            }
        }

        val group = AIbot.getGroup(groupid) ?: let {
            logger.error("ȡȺ��Ƭ�Ҳ�����ӦȺ�飬λ��K-GetNN()��gid:$groupid")
            return "E1"
        }
        val member = group[qqid] ?: let {
            logger.error("ȡȺ��Ƭ�Ҳ�����ӦȺ��Ա��λ��K-GetNN()��id:$qqid, gid:$groupid")
            return "E2"
        }
        return member.nameCard

    }

    //ȡȺ����
    fun QueryNG(groupid: Long): String {
        val g = AIbot.getGroup(groupid)?:let{
            logger.error("�Ҳ���Ⱥ")
            return "E1"
        }
        return g.name
    }

    //ȡȺ��Ա�б�
    fun QueryML(groupid: Long): String {
        val g = AIbot.getGroup(groupid) ?: let {
            logger.error("�Ҳ���Ⱥ")
            return "E1"
        }
        val m = ArrayList<Long>()
        g.members.forEach{
            m.add(it.id)
        }
        return Gson().toJson(m)
    }

    //ͼƬ����ʵ��
    suspend fun uploadImgFriend(id: Long, file: String): String {
        val temp = AIbot.getFriend(id) ?: let {
            logger.error("����ͼƬ�Ҳ�����Ӧ����,λ��:K-uploadImgFriend(),id:$id")
            return ""
        }
        return try {
            File(file).uploadAsImage(temp).imageId
        } catch (e: OverFileSizeMaxException) {
            logger.error("ͼƬ�ļ����󳬹�30MB,λ��:K-uploadImgGroup(),�ļ���:$file")
            ""
        } catch (e: NullPointerException) {
            logger.error("�ϴ�ͼƬ�ļ����쳣,λ��:K-uploadImgGroup(),�ļ���:$file")
            ""
        }
    }

    suspend fun uploadImgGroup(id: Long, file: String): String {
        val temp = AIbot.getGroup(id) ?: let {
            logger.error("����ͼƬ�Ҳ�����ӦȺ��,λ��:K-uploadImgGroup(),id:$id")
            return ""
        }
        return try {
            File(file).uploadAsImage(temp).imageId
        } catch (e: OverFileSizeMaxException) {
            logger.error("ͼƬ�ļ����󳬹�30MB,λ��:K-uploadImgGroup(),�ļ���:$file")
            ""
        } catch (e: NullPointerException) {
            logger.error("�ϴ�ͼƬ�ļ����쳣,λ��:K-uploadImgGroup(),�ļ���:$file")
            ""
        }
    }

    suspend fun uploadImgMember(id: Long, qqid: Long, file: String): String {
        val temp = AIbot.getGroup(id) ?: let {
            logger.error("����ͼƬ�Ҳ�����ӦȺ��,λ��:K-uploadImgGroup(),id:$id")
            return ""
        }
        val temp1 = temp[qqid] ?: let {
            logger.error("����ͼƬ�Ҳ���Ŀ���Ա,λ��:K-uploadImgMember(),��Աid:$qqid,Ⱥ��id:$id")
            return ""
        }
        return try {
            File(file).uploadAsImage(temp1).imageId
        } catch (e: OverFileSizeMaxException) {
            logger.error("ͼƬ�ļ����󳬹�30MB,λ��:K-uploadImgGroup(),�ļ���:$file")
            ""
        } catch (e: NullPointerException) {
            logger.error("�ϴ�ͼƬ�ļ����쳣,λ��:K-uploadImgGroup(),�ļ���:$file")
            ""
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
            logger.error("��������Ȩ�޳���")
            return "E1"
        }catch(e:IllegalStateException){
            logger.error("����Ϣ�ѱ�����")
            return "E2"
        }
        return "Y"
    }

    //��ʱ����
    fun scheduling(time: Long, id: Int) {
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

    suspend fun kkick(qqid: Long, groupid: Long, message: String):String{
        val group = AIbot.getGroup(groupid) ?: let {
            logger.error("��ѯȨ���Ҳ�����ӦȺ�飬λ��K-queryM()��gid:$groupid")
            return "E1"
        }
        val member = group[qqid] ?: let {
            logger.error("��ѯȨ���Ҳ�����ӦȺ��Ա��λ��K-queryM()��id:$qqid, gid:$groupid")
            return "E2"
        }
        try {
            member.kick(message)
        }catch (e:PermissionDeniedException){
            return "E3"
        }
        return "Y"
    }

    @MiraiInternalApi
    suspend fun main(id:Long, pass:String, path:String){
        println("��ǰMiraiCP��ܰ汾:$now_tag")
        setDefaultLoggerCreator { identity ->
            PlatformLogger(identity, AnsiConsole.out::println, true)
        }
        dll_name = path
        println("�����ɹ�!")
        println("github�洢��:https://github.com/Nambers/MiraiCP")
        if (!File(dll_name).exists()) {
            println("�ļ�$dll_name ������")
            return
        }else{
            dll_name = File(dll_name).absolutePath
        }
        val bot = BotFactory.newBot(id, pass) {
            fileBasedDeviceInfo()
        }.alsoLogin()
        cpp = CPP_lib()
        AIbot = bot
        logger=bot.logger
        if(cpp.ver != now_tag){
            logger.error("����:��ǰMiraiCP��ܰ汾($now_tag)�ͼ��ص�C++ SDK(${cpp.ver})��һ��")
        }
        val gson = Gson()
        val globalEventChannel = bot.eventChannel
        logger.info(cpp.ver)//����汾
        //�����ļ�Ŀ¼ "${dataFolder.absolutePath}/"
        globalEventChannel.subscribeAlways<FriendMessageEvent> {
            //������Ϣ
            //���ʧЧ���ܵ���ʱ����
            //[mirai:service:128,<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
            //<msg serviceID="128" templateID="12345" action="native" brief="[����]���������Ⱥ��" sourceMsgId="0" url="">
            //<item layout="2"><picture cover=""/><title>���������Ⱥ��</title><summary /></item>
            //<data groupcode="1044565129" groupname="mirai �ǹٷ� ����Ⱥ" msgseq="1613736417225458" msgtype="2"/>
            //</msg>]
            if(this.message.contentToString().startsWith("<?xml version='1.0' encoding='UTF-8' standalone='yes' ?><msg serviceID=\"128\"")) {
                val mes = "<data[^>]+>".toRegex().find(this.message.contentToString())?.value
                val groupid = mes?.let { it1 ->
                    Regex("groupcode=\"[0-9]+\"").find(it1)?.value
                        ?.replace("\"", "")
                        ?.replace("groupcode=", "")
                }
                val ida = mes?.let { it3 ->
                    Regex("msgseq=\"[0-9]+\"").find(it3)?.value
                        ?.replace("\"", "")
                        ?.replace("msgseq=", "")
                }
                val groupname = mes?.let { it2 ->
                    Regex("groupname=\"(.*)\" msgseq").find(it2)?.value
                        ?.replace("\"", "")
                        ?.replace("msgseq", "")
                        ?.replace("groupname=", "")
                }
                if (ida != null && groupid != null && groupname != null) {
                    BotInvitedJoinGroupRequestEvent(
                        this.bot,
                        ida.toLong(),
                        this.sender.id,
                        groupid.toLong(),
                        groupname,
                        this.sender.nick
                    )
                        .broadcast()
                }
                return@subscribeAlways
            }
            cpp.Event(
                gson.toJson(
                    Config.PrivateMessage(
                        this.sender.id,
                        this.message.serializeToMiraiCode(),
                        json.encodeToString(MessageSource.Serializer,
                            this.message[MessageSource]!!)
                    )
                )
            )
        }

        globalEventChannel.subscribeAlways<GroupMessageEvent> {
            //Ⱥ��Ϣ
            cpp.Event(
                gson.toJson(
                    Config.GroupMessage(
                        this.group.id,
                        this.sender.id,
                        this.message.serializeToMiraiCode(),
                        json.encodeToString(MessageSource.Serializer,this.message[MessageSource]!!)
                    )
                )
            )
        }
        globalEventChannel.subscribeAlways<MemberLeaveEvent.Kick> {
            friend_cache.add(this.member)
            cpp.Event(
                gson.toJson(
                    Config.MemberLeave(
                        this.group.id,
                        this.member.id,
                        1,
                        if (this.operator?.id == null) this.bot.id else this.operator!!.id
                    )
                )
            )
            friend_cache.remove(this.member)
        }
        globalEventChannel.subscribeAlways<MemberLeaveEvent.Quit> {
            friend_cache.add(this.member)
            cpp.Event(
                gson.toJson(
                    Config.MemberLeave(
                        this.group.id,
                        this.member.id,
                        2,
                        this.member.id
                    )
                )
            )
            friend_cache.remove(this.member)
        }
        globalEventChannel.subscribeAlways<MemberJoinEvent.Retrieve> {
            cpp.Event(
                gson.toJson(
                    Config.MemberJoin(
                        this.group.id,
                        this.member.id,
                        3,
                        this.member.id
                    )
                )
            )
        }
        globalEventChannel.subscribeAlways<MemberJoinEvent.Active> {
            cpp.Event(
                gson.toJson(
                    Config.MemberJoin(
                        this.group.id,
                        this.member.id,
                        2,
                        this.member.id
                    )
                )
            )
        }
        globalEventChannel.subscribeAlways<MemberJoinEvent.Invite> {
            cpp.Event(
                gson.toJson(
                    Config.MemberJoin(
                        this.group.id,
                        this.member.id,
                        1,
                        this.invitor.id
                    )
                )
            )
        }
        globalEventChannel.subscribeAlways<NewFriendRequestEvent> {
            //�Զ�ͬ���������
            val r = cpp.Event(
                gson.toJson(
                    Config.NewFriendRequest(
                        this.fromId,
                        this.message
                    )
                )
            )
            when (r) {
                "true" -> accept()
                "false" -> reject()
                else -> {
                    logger.error("NewFriendRequestEvent return unknown")
                    reject()
                }
            }
        }
        globalEventChannel.subscribeAlways<MessageRecallEvent.FriendRecall> {
            cpp.Event(
                gson.toJson(
                    Config.RecallEvent(
                        1,
                        this.authorId,
                        this.operatorId,
                        this.messageIds.toString(),
                        this.messageInternalIds.toString(),
                        this.messageTime
                    )
                )
            )

        }
        globalEventChannel.subscribeAlways<MessageRecallEvent.GroupRecall> {
            cpp.Event(
                gson.toJson(
                    Config.RecallEvent(
                        2,
                        this.authorId,
                        this.operator!!.id,
                        this.messageIds.toString(),
                        this.messageInternalIds.toString(),
                        this.messageTime,
                        this.group.id
                    )
                )
            )

        }
        globalEventChannel.subscribeAlways<BotInvitedJoinGroupRequestEvent> {
            //�Զ�ͬ���Ⱥ����
            val r = cpp.Event(
                gson.toJson(
                    Config.GroupInvite(
                        this.groupId,
                        this.invitorId
                    )
                )
            )
            when (r) {
                "true" -> accept()
                "false" -> ignore()
                "NULL" -> ignore()
                else -> {
                    logger.error("BotInvitedJoinGroupRequestEvent return unknown")
                    ignore()
                }
            }
        }

    }
}
fun CheckUpdate(){
    val tag = JSONObject(URL("https://api.github.com/repos/Nambers/MiraiCP/releases/latest").readText()).getString("tag_name")
    if(tag != now_tag)println("�����¿��ð�:$tag��ǰ��:https://github.com/Nambers/MiraiCP/releases/latest����")
}

@MiraiExperimentalApi
@MiraiInternalApi
fun main(args: Array<String>){
    // qqid, passworld, dllpath, checkupdate
    if(args.size != 3 && args.size != 4){
        println("�����������࣬���ṩ[qq�˺� - long, qq���� - string, dll���λ�� - string] �Լ���ѡ��[�Ƿ�����û�п������� - 1 �� ����]")
        return
    }
    println("��������\n������qqid:${args[0]}\n������qq����:${args[1]}\nc++����dll��ŵ�ַ${args[2]}")
    if(args.size == 4 && args[3] == "1"){
        CheckUpdate()
    }
    runBlocking {
        try {
            KotlinMain.main(valueOf(args[0]), args[1], args[2])
        }catch (e:NumberFormatException){
            println("${args[0]}����һ����Ч��qq������")
            return@runBlocking
        }
    }
}
