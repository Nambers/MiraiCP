package tech.eritquearcus.miraicp.loader
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.utils.MiraiInternalApi

@OptIn(MiraiInternalApi::class, net.mamoe.mirai.utils.MiraiExperimentalApi::class)
fun main(){
    // qqid, passworld, dllpath, checkupdate
    runBlocking {
        main(arrayOf())
    }
}