#include "pch.h"
string Friend::GetNick() {
    return tools.jstring2str(this->env, (jstring)this->env->CallStaticObjectMethod(java_first, Send_Msg_id, this->jid));
}
Friend::Friend (JNIEnv* env, jobject job,jlong id){
    //1.����Ҫ��C�л�ȡjclass����Ҳ�����ҵ��������ڵ��࣬ͨ������  ����+����

        /**2.�ҵ��÷����ķ���ID
        *����һ��jclass
        *��������������
        *������������ǩ��
        *����ǩ������������ϸ˵�����������飬����ʹ��javap -sָ���ѯ
        */

        /**3.���þ�̬����
        *����һ��jclass
        *������������ID
        *���������������ǿɱ䳤������Ҳ���Ǹ÷�������ʱ���Ӧ�Ĳ����б��൱�ڵ��÷���ʱ�Ĵ���
        *��������������ǣ�s: String, i: Int��
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