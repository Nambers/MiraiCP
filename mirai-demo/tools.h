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
class Reply {
public:
    string no = "CONTINUE"; //���ڷ��ط�����Ϣʱ�����ظ�ֵ���ᷢ����Ϣ
    /*
    * ����:�Զ�ʶ�����ͷ���
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