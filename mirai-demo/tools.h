#include "pch.h"
#include <iostream>
#include <sstream>
using namespace std;
class Logger {
private:
    JNIEnv* env;
    jclass javaclass;
    jmethodID sinfo;
    jmethodID swarning;
    jmethodID serror;
public:
    ~Logger();
    void init(JNIEnv* env);
    void Info(string log);
    void Warning(string log);
    void Error(string log);
};
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
static Tools tools;
class Friend {
private:
    jclass java_first;
    jmethodID Send_Msg_id;
    jmethodID Nick_Name_id;
    JNIEnv* env;
public:
    long id;
    Friend(JNIEnv* env, jobject job, long id);
    ~Friend();
    string GetNick();
    string nick;
    void SendMsg(jstring msg) {
        this->env->CallStaticVoidMethod(this->java_first, this->Send_Msg_id, msg, (jlong)this->id);
    }
    void SendMsg(string msg) {      
        this->env->CallStaticVoidMethod(this->java_first, this->Send_Msg_id, tools.str2jstring(this->env, msg.c_str()), (jlong)this->id);
    }
};
class Group {
private:
    jclass java_first;
    jmethodID Send_Msg_id;
    JNIEnv* env;
public:
    long id;
    ~Group();
    Group(JNIEnv* env,jobject job,long id);
    void SendMsg(jstring msg) {
        this->env->CallStaticVoidMethod(java_first, Send_Msg_id, msg, (jlong)this->id);
    }
    void SendMsg(string msg) {
        SendMsg(tools.str2jstring(this->env, msg.c_str()));
    }
};
class Procession {
    //以下为目前接受的接口
public:
    //日志变量
    Logger logger;
    //初始化日志
    void init(Logger logger) {
        this->logger = logger;
    }
    //接受的值，用于GroupInvite这种，对应kotlin中的accept()
    const string accept = "true";
    //拒绝的值
    const string reject = "false";
    //群聊消息
    virtual void GroupMessage(Group, Friend, string) = 0;
    //私聊消息
    virtual void PrivateMessage(Friend, string) = 0;
    //被邀请进群
    virtual string GroupInvite(Group, Friend) { return "true"; };
    //新好友邀请
    virtual string NewFriendRequest(Friend, string) { return "true"; };
    JNIEnv* env;
    jobject job;
};