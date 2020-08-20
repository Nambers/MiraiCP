// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "com_example_plugin_CPP_lib.h"
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

/*
*正文开始
*/
#define no "CONTINUE" //用于返回发送信息时，返回该值不会发送信息
#include <iostream>
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
    * 名称:str2jstring
    * 作用:string类型转jstring类型
    * 参数:JNIEnv,const char*(string)
    * 返回值:jstring
    */
    jstring str2jstring(JNIEnv* env, const char* pat)
    {
        //定义java String类 strClass
        jclass strClass = (env)->FindClass("Ljava/lang/String;");
        //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
        jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
        //建立byte数组
        jbyteArray bytes = (env)->NewByteArray(strlen(pat));
        //将char* 转换为byte数组
        (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
        // 设置String, 保存语言类型,用于byte数组转换至String时的参数
        jstring encoding = (env)->NewStringUTF("GB2312");
        //将byte数组转换为java String,并输出
        return (jstring)(env)->NewObject(strClass, ctorID, bytes, encoding);
    }
    /*
    * 名称:Java_com_example_plugin_CPP_1lib_Verify
    * 作用:判断是否连接上本插件，勿改
    * 参数:env 必备，job 必备
    * 返回值:jstring (用str2jstring把string类型转成jsrting) 发送返回的字符串
    */
    JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_Verify(JNIEnv* env, jobject job) {
        return str2jstring(env,"2333");//验证机制
    }
    /*
    * 名称:Java_com_example_plugin_CPP_1lib_PrivateMessage
    * 作用:处理私聊消息
    * 参数:env 必备job 必备，qqid qq号，message 发送的消息
    * 返回值:jstring(用str2jstring把string类型转成jsrting) 发送返回的字符串
    */
    JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_PrivateMessage
    (JNIEnv* env, jobject job, jlong qqid, jstring message) {
        return str2jstring(env,"我们还在测试阶段");

    }
    /*
    * 名称:Java_com_example_plugin_CPP_1lib_GroupMessage
    * 作用:处理群消息
    * 参数:env:JNIEnv 必备， job:jobject 必备，groupid:jlong 群号，qqid:jlong qq号，message:jstring 信息
    * 返回值:jsrting (用str2jstring把string类型转成jsrting) 发送返回的字符串
    */
    JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_GroupMessage
    (JNIEnv* env, jobject job, jlong groupid, jlong qqid, jstring message) {
        return str2jstring(env,no);
    }