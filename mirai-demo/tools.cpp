#include "pch.h"
void Logger::init(JNIEnv* env) {
    this->env = env;
    this->javaclass = this->env->FindClass("org/example/mirai/plugin/CPP_lib");
    this->sinfo = this->env->GetStaticMethodID(this->javaclass, "SendLog", "(Ljava/lang/String;)V");
    this->swarning = this->env->GetStaticMethodID(this->javaclass, "SendW", "(Ljava/lang/String;)V");
    this->serror = this->env->GetStaticMethodID(this->javaclass, "SendE", "(Ljava/lang/String;)V");
}
Logger::~Logger() {
    this->env->DeleteLocalRef(this->javaclass);
}
void Logger::Warning(string log) {
    this->env->CallStaticVoidMethod(this->javaclass, this->swarning, tools.str2jstring(env, log.c_str()));
}
void Logger::Error(string log) {
    this->env->CallStaticVoidMethod(this->javaclass, this->serror, tools.str2jstring(env, log.c_str()));
}
void Logger::Info(string log) {
    this->env->CallStaticVoidMethod(this->javaclass, this->sinfo, tools.str2jstring(env, log.c_str()));
}
Friend::Friend (JNIEnv* env, jobject job,long id){
    this->java_first = env->FindClass("org/example/mirai/plugin/CPP_lib");
    this->Send_Msg_id = env->GetStaticMethodID(java_first, "SendPrivateMSG", "(Ljava/lang/String;J)V");
    this->NickorName_id = env->GetStaticMethodID(java_first, "GetNick", "(J)Ljava/lang/String;");
    this->id = id;
    this->env = env;
    jstring temp = (jstring)this->env->CallStaticObjectMethod(this->java_first, this->NickorName_id, (jlong)id, (jlong)id);
    this->nick = tools.jstring2str(this->env, temp);
}
Friend::~Friend() {
    this->env->DeleteLocalRef(java_first);
}
Member::Member(JNIEnv* env, jobject job, long id, long groupid) {
    this->java_first = env->FindClass("org/example/mirai/plugin/CPP_lib");
    this->Send_Msg_id = env->GetStaticMethodID(java_first, "SendPrivateM2M", "(Ljava/lang/String;JJ)V");
    this->NickorName_id = env->GetStaticMethodID(java_first, "GetNameCard", "(JJ)Ljava/lang/String;");
    this->id = id;
    this->groupid = groupid;
    this->env = env;
    jstring temp = (jstring)this->env->CallStaticObjectMethod(this->java_first, this->NickorName_id, (jlong)id, (jlong)groupid);
    this->nameCard = tools.jstring2str(this->env, temp);
}
Member::~Member() {
    this->env->DeleteLocalRef(java_first);
}
Group::Group(JNIEnv* env, jobject job, long id) {
    this->java_first = env->FindClass("org/example/mirai/plugin/CPP_lib");
    this->Send_Msg_id = env->GetStaticMethodID(java_first, "SendGroup", "(Ljava/lang/String;J)V");
    this->env = env;
    this->id = id;
}
Group::~Group() {
    env->DeleteLocalRef(java_first);

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