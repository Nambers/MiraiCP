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
        //����java String�� strClass
        jclass strClass = (env)->FindClass("Ljava/lang/String;");
        //��ȡString(byte[],String)�Ĺ�����,���ڽ�����byte[]����ת��Ϊһ����String
        jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
        //����byte����
        jbyteArray bytes = (env)->NewByteArray(strlen(pat));
        //��char* ת��Ϊbyte����
        (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
        // ����String, ������������,����byte����ת����Stringʱ�Ĳ���
        jstring encoding = (env)->NewStringUTF("GB2312");
        //��byte����ת��Ϊjava String,�����
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
