#include "tools.h"
Tools tools;
    string Tools::jstring2str(JNIEnv* env, jstring jstr)
    {
        char* rtn = NULL;
        jclass   clsstring = env->FindClass("java/lang/String");
        jstring   strencode = env->NewStringUTF("GB2312");
        jmethodID   mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
        jbyteArray   barr = (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
        jsize   alen = env->GetArrayLength(barr);
        jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
        if (alen > 0)
        {
            rtn = (char*)malloc(alen + 1);
            memcpy(rtn, ba, alen);
            rtn[alen] = 0;
        }
        env->ReleaseByteArrayElements(barr, ba, 0);
        string stemp(rtn);
        free(rtn);
        return stemp;
    }
    
    jstring Tools::str2jstring(JNIEnv* env, const char* pat)
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
    string Tools::JLongToString(jlong qqid) {
        auto id = [qqid]() -> string {
            stringstream stream;
            stream << qqid;
            string a;
            stream >> a;
            stream.clear();
            return a;
        };
        return id();
    }

    jstring Reply::AutoReturn(JNIEnv* env, string message) {
        return tools.str2jstring(env, message.c_str());
    }
    jstring Reply::AutoReturn(JNIEnv* env, const char* message) {
        return tools.str2jstring(env, message);
    }
    jstring Reply::AutoReturn(JNIEnv* env, const char* message) {
        return tools.str2jstring(env, message);
    }
    jstring Reply::NoReply(JNIEnv* env) {
        return tools.str2jstring(env, no.c_str());
    }
    jstring Reply::AutoReturn(JNIEnv* env, jstring message) {
        return message;
    }
    jstring Reply::AutoReturn(JNIEnv* env, int message) {
        return tools.str2jstring(env, to_string(message).c_str());
    }
    jstring Reply::AutoReturn(JNIEnv* env, long message) {
        return tools.str2jstring(env, to_string(message).c_str());
    }
    jstring Reply::AutoReturn(JNIEnv* env, float message) {
        return tools.str2jstring(env, to_string(message).c_str());
    }
    jstring Reply::AutoReturn(JNIEnv* env, double message) {
        return tools.str2jstring(env, to_string(message).c_str());
    }
