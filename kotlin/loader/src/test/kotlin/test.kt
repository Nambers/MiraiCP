package tech.eritquearcus.miraicp.loader
import kotlinx.coroutines.runBlocking
import net.mamoe.mirai.utils.MiraiExperimentalApi
import net.mamoe.mirai.utils.MiraiInternalApi
import tech.eritquearcus.miraicp.loader.testConfig.code
import tech.eritquearcus.miraicp.loader.testConfig.id
import tech.eritquearcus.miraicp.loader.testConfig.path

@MiraiExperimentalApi
@MiraiInternalApi
fun main(){
    // qqid, passworld, dllpath, checkupdate
    runBlocking {
        KotlinMain.main(id, code, path, false)
    }
}