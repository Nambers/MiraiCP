#include "pch.h"
/*
*正文开始
*/
/*
* 名称:Java_com_example_plugin_CPP_1lib_GroupMemberJoin
* 作用:群成员加入
* 参数:env 必备参数，job 必备参数，memberid qq号，active 是否主动加入（非邀请）
* 返回值: jstring 发送到群里的原因
*/
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_GroupMemberJoin
(JNIEnv* env, jobject job, jlong memberid, jboolean active) {
    return reply.AutoReturn(env,memberid+"加入了本群");
}
/*
* 名称:Java_com_example_plugin_CPP_1lib_GroupMemberLeave
* 作用:群成员离开
* 参数:env 必备参数，job 必备参数，memberid qq号，active 是否退出（非踢出）
* 返回值:jstring 发送到群里的消息
*/
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_GroupMemberLeave
(JNIEnv* env, jobject job, jlong memberid, jboolean active) {
    return reply.AutoReturn(env,memberid+"离开了本群");
}
/*
* 名称:Java_com_example_plugin_CPP_1lib_GroupNameChange
* 作用:群 改变名字
* 参数:env 必备参数，job 必备参数，origin 原名，newName 新名字，Groupid 群号，operateid 操作者qq（如果是机器人则为0）
* 返回值: jstring 发送到群里的消息
*/
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_GroupNameChange
(JNIEnv* env, jobject job, jstring origin, jstring newName, jlong Groupid, jlong operateid) {
    return reply.NoReply(env);
}
/*
* 名称:Java_com_example_plugin_CPP_1lib_FriendRequest
* 作用:好友申请
* 参数:env 必备参数，job 必备参数，qqid qq号，nick 申请者昵称，message 验证消息
* 返回值:bool 是为添加 否为不添加
*/
JNIEXPORT jboolean JNICALL Java_com_example_plugin_CPP_1lib_FriendRequest
(JNIEnv* env, jobject job, jlong qqid, jstring nick, jstring message) {
    return true;
}

/*
* 名称:Java_com_example_plugin_CPP_1lib_Verify
* 作用:判断是否连接上本插件，勿改
* 参数:env 必备，job 必备
* 返回值:jstring (用str2jstring把string类型转成jsrting) 发送返回的字符串
*/
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_Verify(JNIEnv* env, jobject job) {
    return reply.AutoReturn(env, "2333");//验证机制
}
/*
* 名称:Java_com_example_plugin_CPP_1lib_PrivateMessage
* 作用:处理私聊消息
* 参数:env 必备，job 必备，senderid qq号，sendername 发送者昵称，HeadImageDownloadUrl 发送者头像地址，message 发送的消息
* 返回值:jstring(用str2jstring把string类型转成jsrting) 发送返回的字符串
*/
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_PrivateMessage
(JNIEnv* env, jobject job, jlong SenderId, jstring SenderName, jstring HeadImageDownloadUrl, jstring Message) {
    string result = "我还在测试中";//default is no reply
    if (tools.JLongToString(SenderId) == (string)"1930893235")result = "hi";//if the qqid equal 11111 send qqmessage "hi"
    /*jclass cls = env->GetObjectClass(job);  //获得JAVA对象在C++中的对应对象  jclass
    jmethodID  id = env->GetStaticMethodID(cls, "showStatic", "()V");  //通过 JNIEnv类 也就是java环境获得  静态方法的 jmethodID          
    env->CallStaticVoidMethod(cls, id,"a");  //INVOKE  STATIC  METHOD   执行java静态方法 */
    if (tools.jstring2str(env, Message).substr(0, 2) == "复读") {
        return reply.AutoReturn(env, Message);//复读功能
    }
    if (result == reply.no)return reply.NoReply(env);
    return reply.AutoReturn(env, result);
}
/**
* 名称:Java_com_example_plugin_CPP_1lib_GroupMessage
* 作用:处理群消息
* 参数: env:JNIEnv 必备， job:jobject 必备，groupid:jlong 群号，GroupName:jstring 群名，GroupOwnerId:jlong 群主qq号，sendernick:jstring 发送者昵称，sendergroupname:jstring 发送者群名称，senderid:jlong qq号，message:jstring 信息
* 返回值:jsrting (用str2jstring把string类型转成jsrting) 发送返回的字符串
**/
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_GroupMessage
(JNIEnv* env, jobject job, jlong GroupId, jstring GroupName, jlong GroupOwnerId, jstring SenderNick, jstring SenderGroupName, jlong SenderId, jstring Message){
    return reply.AutoReturn(env, reply.no);//no reply
}


