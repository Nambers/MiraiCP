#include "pch.h"
#include "Procession.cpp"
Logger logger = Logger();
procession p = procession();
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
    logger.init(env);
    p.init(logger);
    return tools.str2jstring(env, "2333");//验证机制
}

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
        logger.Error("JSON reader error");
    }
    switch (root["type"].asInt()) {
        /*
        1- groupmessage
        2- privatemessage
        */
    case 1:
        //GroupMessage
        p.GroupMessage(Group(env,job, root["groupid"].asLargestInt()),
            Friend(env, job, root["senderid"].asLargestInt()), root["message"].asCString());
        return tools.str2jstring(env, "NULL");
    case 2:
        p.PrivateMessage(Friend(env, job, root["senderid"].asLargestInt()), root["message"].asCString());
        return tools.str2jstring(env, "NULL");
    case 3:
        return tools.str2jstring(env, p.GroupInvite(Group(env, job, root["groupid"].asLargestInt()),
            Friend(env, job, root["invitorid"].asLargestInt())).c_str());
    case 4: return tools.str2jstring(env,
        p.NewFriendRequest(
            Friend(env, job, root["friendid"].asLargestInt()), root["message"].asCString()).c_str());
    }
    logger.Error("unknown type");
    return tools.str2jstring(env, "ERROR");
}
