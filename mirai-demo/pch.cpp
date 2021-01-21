#include "pch.h"
#include "Procession.cpp"

void SendLog(JNIEnv* env, jobject job, string log) {
    //1.首先要在C中获取jclass对象，也就是找到方法所在的类，通过完整  包名+类名
    jclass java_first = env->FindClass("org/example/mirai/plugin/CPP_lib");

    /**2.找到该方法的方法ID
    *参数一：jclass
    *参数二：方法名
    *参数三：方法签名
    *方法签名，网上有详细说明，如果不想查，可以使用javap -s指令查询
    */
    jmethodID static_method_id = env->GetStaticMethodID(java_first, "SendLog", "(Ljava/lang/String;)V");

    /**3.调用静态方法
    *参数一：jclass
    *参数二：方法ID
    *参数三：参数三是可变长参数，也就是该方法声明时候对应的参数列表，相当于调用方法时的传参
    *这个方法声明的是（s: String, i: Int）
    */
    env->CallStaticVoidMethod(java_first, static_method_id, tools.str2jstring(env, log.c_str()));

    //4.删除掉本地引用
    env->DeleteLocalRef(java_first);
}
/*
*正文开始
*/
/*
* 名称:Java_com_example_plugin_CPP_1lib_Verify
* 作用:判断是否连接上本插件，勿改
* 参数:env 必备，job 必备
* 返回值:jstring (用str2jstring把string类型转成jsrting) 发送返回的字符串
*/
JNIEXPORT jstring JNICALL Java_org_example_mirai_plugin_CPP_1lib_Verify(JNIEnv* env, jobject job) {
    //Friend(env, job, (jlong)1930893235).SendMsg("H");
    return tools.str2jstring(env, "2333");//验证机制
}
procession p = procession();
JNIEXPORT jstring JNICALL Java_org_example_mirai_plugin_CPP_1lib_Event
(JNIEnv* env, jobject job, jstring content) {
    string Rcontent = tools.jstring2str(env, content);
    const auto rawJsonLength = static_cast<int>(Rcontent.length());
    JSONCPP_STRING err;
    Json::Value root;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(Rcontent.c_str(), Rcontent.c_str() + rawJsonLength, &root,
        &err)) {
        //error
        SendLog(env, job, "JSON reader error");
    }
    switch (root["type"].asInt()) {
        /*
        1- groupmessage
        2- privatemessage
        */
    case 1:
        //GroupMessage
        p.GroupMessage(Group(env,job, root["groupid"].asLargestInt()),
            Friend(env, job, root["senderid"].asLargestInt()));
        return tools.str2jstring(env, "NULL");
    case 2:
        p.PrivateMessage(Friend(env, job, root["senderid"].asLargestInt()));
        return tools.str2jstring(env, "NULL");
    }
    SendLog(env, job, "unknown type");
    return tools.str2jstring(env, "ERROR");
}
