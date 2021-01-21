package org.example.mirai.plugin
import kotlinx.coroutines.launch
import org.example.mirai.plugin.PluginMain.BasicSendLog
import org.example.mirai.plugin.PluginMain.Send
//login 692928873 air12345
import org.example.mirai.plugin.PluginMain.dll_name
import org.example.mirai.plugin.PluginMain.dataFolder
import kotlinx.coroutines.runBlocking
import org.example.mirai.plugin.PluginMain.GetN
import org.example.mirai.plugin.PluginMain.SendG

class CPP_lib {
    var ver:String=""
    init {
        ver=Verify()//如果ver不等于2333 载入错误
    }
    //"C:\Program Files\Java\jdk1.8.0_261\bin\javah.exe" org.example.mirai.plugin.CPP_lib
    companion object{
        init {
            //这里填自己的路径
            System.load(dataFolder.absolutePath + "/$dll_name")
        }
        
        @JvmStatic
        fun SendGroup(message:String,id:Long) {
            runBlocking {
                launch {
                    SendG(message, id)
                }
            }
        }
        @JvmStatic
        fun SendLog(log:String) {
            BasicSendLog(log)
        }
        @JvmStatic
        fun SendPrivateMSG(message: String, id: Long){
            // 反向调用发送消息
            runBlocking {
                launch {
                    Send(message, id)
                }
            }
        }
        @JvmStatic
        fun GetNick(qqid:Long): String? {
            return GetN(qqid)
        }
    }


    external fun Verify(): String
    external fun Event(content: String): String
}

