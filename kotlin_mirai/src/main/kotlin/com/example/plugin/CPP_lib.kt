package com.example.plugin
import com.example.plugin.ExamplePluginMain.BasicSendLog
import com.example.plugin.ExamplePluginMain.GetN
import com.example.plugin.ExamplePluginMain.Send
import com.example.plugin.ExamplePluginMain.SendG
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.runBlocking

class CPP_lib {
    var ver:String=""
    init {
        ver=Verify()//如果ver不等于2333 载入错误
    }
    companion object{
        init {
            //这里填自己的路径
            System.load("D:\\Mirai\\libraries\\mirai-demo.dll")
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
            return
        }
        @JvmStatic
        fun GetNick(qqid:Long): String {
            return GetN(qqid)
        }
    }

    

    external fun Verify(): String
    external fun PrivateMessage(id:Long,message:String)
    external fun GroupMessage(groupId:Long,GroupName:String,ownerId:Long,MemberName:String,MemberGroupName:String,Memberid: Long,message:String)
    external fun FriendRequest(qqid:Long,Nick:String,message: String): Boolean
    external fun GroupNameChange(origin:String,new:String,group:Long,operate:Long)
    external fun GroupMemberLeave(Memberid:Long,Q:Boolean,Groupid:Long)
    external fun GroupMemberJoin(Memberid:Long,Driving:Boolean,Groupid:Long)
}

