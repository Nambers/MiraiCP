package tech.eritquearcus.miraicp.loader

import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.BotFactory
import net.mamoe.mirai.alsoLogin
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.message.data.*
import net.mamoe.mirai.utils.*
import net.mamoe.mirai.utils.MiraiLogger.Companion.setDefaultLoggerCreator
import org.fusesource.jansi.AnsiConsole
import org.json.JSONObject
import tech.eritquearcus.miraicp.shared.publicShared
import java.io.File
import java.lang.Long.valueOf
import java.net.URL
import java.util.*

object KotlinMain {
    @OptIn(MiraiInternalApi::class)
    suspend fun main(id: Long, pass: String, path: String, check:Boolean) {
        val now_tag = "v2.6.4"
        println("当前MiraiCP框架版本:$now_tag")
        setDefaultLoggerCreator { identity ->
            AnsiConsole.systemInstall()
            PlatformLogger(identity, AnsiConsole.out()::println, true)
        }
        var dll_name = path
        println("启动成功!")
        println("github存储库:https://github.com/Nambers/MiraiCP")
        if (!File(dll_name).exists()) {
            println("文件$dll_name 不存在")
            return
        } else {
            dll_name = File(dll_name).absolutePath
        }
        val bot = BotFactory.newBot(id, pass) {
            fileBasedDeviceInfo()
        }.alsoLogin()
        val logger = bot.logger
        val globalEventChannel = bot.eventChannel
        publicShared.init(logger, now_tag, dll_name)
        if(check)
            publicShared.CheckUpdate()
        publicShared.onEnable(globalEventChannel)
    }
}

@MiraiExperimentalApi
@MiraiInternalApi
fun main(args: Array<String>){
    // qqid, passworld, dllpath, checkupdate
    if(args.size != 3 && args.size != 4){
        println("参数不足或多余，请提供[qq账号 - long, qq密码 - string, dll存放位置 - string] 以及可选的[是否检测有没有可用升级 - 1 或 不填]")
        return
    }
    println("正在启动\n机器人qqid:${args[0]}\n机器人qq密码:${args[1]}\nc++部分dll存放地址${args[2]}")
    runBlocking {
        try {
            if(args.size == 4 && args[3] == "1")
                KotlinMain.main(valueOf(args[0]), args[1], args[2], true)
            else
                KotlinMain.main(valueOf(args[0]), args[1], args[2], false)
        }catch (e:NumberFormatException){
            println("${args[0]}不是一个有效的qq号数字")
            return@runBlocking
        }
    }
}
