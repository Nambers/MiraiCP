/*
*tools class
*/
#include "jni.h"
#include <iostream>
#include <sstream>
using namespace std;
class Tools {
public:
    
    /*
    * 名称:jstring2str
    * 作用:jstring到string转换
    * 参数:env:JNIEnv 必须,jstr:jstring 转换的文本
    * 返回值:string
    */
	string jstring2str(JNIEnv* env, jstring jstr);
    /*
    * 名称:str2jstring
    * 作用:string类型转jstring类型
    * 参数:JNIEnv,const char*(string)
    * 返回值:jstring
    */
	jstring str2jstring(JNIEnv* env, const char* pat);
    /*
    * 名称:JLongToString
    * 作用:jlong类型转string类型
    * 参数:jlong qqid
    * 返回值:string
    */
    string JLongToString(jlong qqid);
};
class Reply {
public:
    string no = "CONTINUE"; //用于返回发送信息时，返回该值不会发送信息
    /*
    * 作用:自动识别类型返回
    */
    jstring AutoReturn(JNIEnv* env, string message);
    jstring AutoReturn(JNIEnv* env, int message);
    jstring AutoReturn(JNIEnv* env, long message);
    jstring AutoReturn(JNIEnv* env, float message);
    jstring AutoReturn(JNIEnv* env, double message);
    jstring AutoReturn(JNIEnv* env, const char* message);
    jstring AutoReturn(JNIEnv* env, jstring message);
    jstring NoReply(JNIEnv* env);
};