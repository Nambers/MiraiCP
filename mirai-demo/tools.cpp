#include "pch.h"
string Friend::GetNick() {
    return tools.jstring2str(this->env, (jstring)this->env->CallStaticObjectMethod(java_first, Send_Msg_id, this->jid));
}
Friend::Friend (JNIEnv* env, jobject job,jlong id){
    //1.首先要在C中获取jclass对象，也就是找到方法所在的类，通过完整  包名+类名

        /**2.找到该方法的方法ID
        *参数一：jclass
        *参数二：方法名
        *参数三：方法签名
        *方法签名，网上有详细说明，如果不想查，可以使用javap -s指令查询
        */

        /**3.调用静态方法
        *参数一：jclass
        *参数二：方法ID
        *参数三：参数三是可变长参数，也就是该方法声明时候对应的参数列表，相当于调用方法时的传参
        *这个方法声明的是（s: String, i: Int）
        */
    this->java_first = env->FindClass("com/example/plugin/CPP_lib");
    this->Send_Msg_id = env->GetStaticMethodID(java_first, "SendPrivate", "(Ljava/lang/String;J)V");
    this->Nick_Name_id = env->GetStaticMethodID(java_first, "GetNick", "(J)Ljava/lang/String");
    this->jid = id;
    this->id = (long)id;
    this->env = env;
    //this->nick=this->GetNick();
}
Friend::~Friend() {
    this->env->DeleteLocalRef(java_first);
    delete(this->env);
}
Group::Group(JNIEnv* env, jobject job, jlong id) {
    this->java_first = env->FindClass("com/example/plugin/CPP_lib");
    this->Send_Msg_id = env->GetStaticMethodID(java_first, "SendGroup", "(Ljava/lang/String;J)V");
    this->env = env;
    this->jid = id;
    this->id = id;
}
Group::~Group() {
    env->DeleteLocalRef(java_first);
    delete(this->env);
}
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