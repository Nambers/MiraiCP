package org.example.mirai.plugin
import kotlinx.coroutines.launch
import org.example.mirai.plugin.PluginMain.BasicSendLog
import org.example.mirai.plugin.PluginMain.Send
import org.example.mirai.plugin.PluginMain.dll_name
import org.example.mirai.plugin.PluginMain.dataFolder
import kotlinx.coroutines.runBlocking
import org.example.mirai.plugin.PluginMain.GetN
import org.example.mirai.plugin.PluginMain.GetNN
import org.example.mirai.plugin.PluginMain.QueryImg
import org.example.mirai.plugin.PluginMain.SendError
import org.example.mirai.plugin.PluginMain.SendG
import org.example.mirai.plugin.PluginMain.SendWarning

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
        fun QueryImgUrl(id:String): String{
            var temp = ""
            runBlocking {
                launch {
                    temp = QueryImg(id)
                }
            }
            return temp
        }
        @JvmStatic
        fun SendLog(log:String) {
            BasicSendLog(log)
        }
        @JvmStatic
        fun SendW(log:String) {
            SendWarning(log)
        }
        @JvmStatic
        fun SendE(log:String) {
            SendError(log)
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
        fun SendPrivateM2M(message: String, id: Long, gid: Long){
            // 反向调用发送消息
            runBlocking {
                launch {
                    Send(message, id, gid)
                }
            }
        }
        @JvmStatic
        fun GetNick(qqid:Long): String {
            return GetN(qqid)
        }
        @JvmStatic
        fun GetNameCard(qqid:Long, groupid:Long): String {
            return GetNN(qqid, groupid);
        }
    }


    external fun Verify(): String
    external fun Event(content: String): String
    external fun PluginDisable(): Void
}

