#include "pch.h"
#include <iostream>
#include <sstream>
using namespace std;
class Tools {
public:
    
    /*
    * ����:jstring2str
    * ����:jstring��stringת��
    * ����:env:JNIEnv ����,jstr:jstring ת�����ı�
    * ����ֵ:string
    */
	string jstring2str(JNIEnv* env, jstring jstr);
    /*
    * ����:str2jstring
    * ����:string����תjstring����
    * ����:JNIEnv,const char*(string)
    * ����ֵ:jstring
    */
	jstring str2jstring(JNIEnv* env, const char* pat);
    /*
    * ����:JLongToString
    * ����:jlong����תstring����
    * ����:jlong qqid
    * ����ֵ:string
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
    //����ΪĿǰ���ܵĽӿ�
public:
    virtual void GroupMessage(Group, Friend) = 0 {};
    virtual void PrivateMessage(Friend) = 0 {};
};