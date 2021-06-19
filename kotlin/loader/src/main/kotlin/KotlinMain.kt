package tech.eritquearcus.miraicp.loader

import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.BotFactory
import net.mamoe.mirai.alsoLogin
import net.mamoe.mirai.utils.*
import net.mamoe.mirai.utils.MiraiLogger.Companion.setDefaultLoggerCreator
import org.fusesource.jansi.AnsiConsole
import org.json.JSONObject
import tech.eritquearcus.miraicp.shared.publicShared
import java.io.File

object KotlinMain {
    @OptIn(MiraiInternalApi::class)
    suspend fun main(id: Long, pass: String, path: String,protocol:String, check:Boolean) {
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
        val p = when(protocol){
            "pad" -> BotConfiguration.MiraiProtocol.ANDROID_PAD
            "watch" -> BotConfiguration.MiraiProtocol.ANDROID_WATCH
            "phone" -> BotConfiguration.MiraiProtocol.ANDROID_PHONE
            else -> {
                println("登录协议无效, 应为pad/watch/phone其中一个,默认使用phone进行登录")
                BotConfiguration.MiraiProtocol.ANDROID_PHONE
            }
        }
        println("以${p.name}协议登录bot:$id")
        println("c++ dll地址:${path}")
        val bot = BotFactory.newBot(id, pass) {
            fileBasedDeviceInfo()
            this.protocol = p

        }.alsoLogin()
        val logger = bot.logger
        val globalEventChannel = bot.eventChannel
        publicShared.init(logger, dll_name)
        if(check)
            publicShared.CheckUpdate()
        publicShared.onEnable(globalEventChannel)
    }
}

@MiraiExperimentalApi
@MiraiInternalApi
fun main(args: Array<String>){
    // config.json path
    val path = "config.json"
    var f = File(path)
    when(args.size) {
        1 -> {
            f = File(args[0])
            if (!f.exists() || !f.isFile || !f.canRead()) {
                println("配置文件路径(${f.absolutePath})读取错误,文件不存在/不是文件/不可读, 使用默认路径重试")
                f = File(path)
                if (!f.exists() || !f.isFile || !f.canRead()) {
                    f = File(path)
                    if (!f.exists() || !f.isFile || !f.canRead()) {
                        println("默认配置文件路径(${f.absolutePath})读取错误,文件不存在/不是文件/不可读")
                        return
                    }
                }
            }
        }
        0 -> {
            if (!f.exists() || !f.isFile || !f.canRead()) {
                f = File(path)
                if (!f.exists() || !f.isFile || !f.canRead()) {
                    println("默认配置文件路径(${f.absolutePath})读取错误,文件不存在/不是文件/不可读")
                    return
                }
            }
        }
        else->{
            println("参数多余，参数仅为配置文件位置(可选, 默认位置为/config.json)")
        }
    }
    println("正在启动\n配置文件地址:${f.absolutePath}")
    val j = JSONObject(f.readText())
    listOf("id", "passwords", "cppPath").forEach {
        if (!j.has(it)) {
            println("配置出错，配置文件不包含($it)项")
            return
        }
    }
    runBlocking {
        try {
            KotlinMain.main(
                j.getLong("id"),
                j.getString("passwords"),
                j.getString("cppPath"),
                if (j.has("protocol")) j.getString("protocol") else "phone",
                if (j.has("checkUpdate")) j.getBoolean("checkUpdate") else false
            )
        } catch (e: NumberFormatException) {
            println("${args[0]}不是一个有效的qq号数字")
            return@runBlocking
        }
    }
}
