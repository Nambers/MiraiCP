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
#include <iostream>
    JNIEXPORT jboolean JNICALL Java_com_example_plugin_CPP_1lib_FriendRequest
    (JNIEnv*, jobject, jlong, jstring, jstring) {
        return true;
    }
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
    JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_Verify(JNIEnv* e, jobject) {
        return str2jstring(e,"2333");//验证机制
    }
    JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_PrivateMessage
    (JNIEnv* e, jobject, jlong, jstring) {
        return str2jstring(e,"我们还在测试阶段");

    }
    JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_GroupMessage
    (JNIEnv* e, jobject, jlong, jlong, jstring) {
        return str2jstring(e,"CONTINUE");
    }