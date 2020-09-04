// pch.cpp: 与预编译标头对应的源文件
#include "pch.h"


// 当使用预编译的头时，需要使用此源文件，编译才能成功。

/*
*正文开始
*/

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
* 参数:env 必备job 必备，qqid qq号，message 发送的消息
* 返回值:jstring(用str2jstring把string类型转成jsrting) 发送返回的字符串
*/
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_PrivateMessage
(JNIEnv* env, jobject job, jlong qqid, jstring message) {
    string result=reply.no;//default is no reply
    if (tools.JLongToString(qqid)== (string)"11111")result = "hi";//if the qqid equal 11111 send qqmessage "hi"
    if (tools.jstring2str(env,message).substr(0, 4) == "复读") {
        return reply.AutoReturn(env, message);//复读功能
    }
    return reply.AutoReturn(env,result);
}
/*
* 名称:Java_com_example_plugin_CPP_1lib_GroupMessage
* 作用:处理群消息
* 参数:env:JNIEnv 必备， job:jobject 必备，groupid:jlong 群号，qqid:jlong qq号，message:jstring 信息
* 返回值:jsrting (用str2jstring把string类型转成jsrting) 发送返回的字符串
*/
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_GroupMessage
(JNIEnv* env, jobject job, jlong groupid, jlong qqid, jstring message) {
    return reply.AutoReturn(env, reply.no);//no reply
}


