#include "pch.h"
JNIEXPORT void SendLog(JNIEnv* env, jobject job, string log) {
    //1.首先要在C中获取jclass对象，也就是找到方法所在的类，通过完整  包名+类名
    jclass java_first = env->FindClass("com/example/plugin/CPP_lib");

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
* 名称:Java_com_example_plugin_CPP_1lib_GroupMemberJoin
* 作用:群成员加入
* 参数:env 必备参数，job 必备参数，memberid qq号，active 是否主动加入（非邀请）
* 返回值: void
*/
JNIEXPORT void JNICALL Java_com_example_plugin_CPP_1lib_GroupMemberJoin
(JNIEnv* env, jobject job, jlong memberid, jboolean active,jlong groupid) {
    Group g(env, job, groupid);
    g.SendMsg(tools.JLongToString(memberid) + "加入了本群");
}
/*
* 名称:Java_com_example_plugin_CPP_1lib_GroupMemberLeave
* 作用:群成员离开
* 参数:env 必备参数，job 必备参数，memberid qq号，active 是否退出（非踢出）
* 返回值:void
*/
JNIEXPORT void JNICALL Java_com_example_plugin_CPP_1lib_GroupMemberLeave
(JNIEnv* env, jobject job, jlong memberid, jboolean active,jlong groupid) {
    Group g(env, job, groupid);
    Friend f(env, job, memberid);
    g.SendMsg(tools.JLongToString(memberid) + "离开了本群");
}
/*
* 名称:Java_com_example_plugin_CPP_1lib_GroupNameChange
* 作用:群 改变名字
* 参数:env 必备参数，job 必备参数，origin 原名，newName 新名字，Groupid 群号，operateid 操作者qq（如果是机器人则为0）
* 返回值: void
*/
JNIEXPORT void JNICALL Java_com_example_plugin_CPP_1lib_GroupNameChange
(JNIEnv* env, jobject job, jstring origin, jstring newName, jlong Groupid, jlong operateid) {
    //不做反应
}
/*
* 名称:Java_com_example_plugin_CPP_1lib_FriendRequest
* 作用:好友申请
* 参数:env 必备参数，job 必备参数，qqid qq号，nick 申请者昵称，message 验证消息
* 返回值:bool 是为添加 否为不添加
*/
JNIEXPORT jboolean JNICALL Java_com_example_plugin_CPP_1lib_FriendRequest
(JNIEnv* env, jobject job, jlong qqid, jstring nick, jstring message) {
    Friend object(env, job, qqid);
    object.SendMsg("Hi");
    return true;
}

/*
* 名称:Java_com_example_plugin_CPP_1lib_Verify
* 作用:判断是否连接上本插件，勿改
* 参数:env 必备，job 必备
* 返回值:jstring (用str2jstring把string类型转成jsrting) 发送返回的字符串
*/
JNIEXPORT jstring JNICALL Java_com_example_plugin_CPP_1lib_Verify(JNIEnv* env, jobject job) {
    //Friend(env, job, (jlong)1930893235).SendMsg("H");
    return tools.str2jstring(env, "2333");//验证机制
}
/*
* 名称:Java_com_example_plugin_CPP_1lib_PrivateMessage
* 作用:处理私聊消息
* 参数:env 必备，job 必备，senderid qq号，sendername 发送者昵称，HeadImageDownloadUrl 发送者头像地址，message 发送的消息
* 返回值: void
*/
JNIEXPORT void JNICALL Java_com_example_plugin_CPP_1lib_PrivateMessage
(JNIEnv* env, jobject job, jlong SenderId, jstring Message) {
    Friend object(env,job, SenderId);
    string result = "我还在测试中";//default is no reply
    if (object.id== 1930893235) {
        object.SendMsg("Hi"+object.GetNick());//发送消息 Hi
        SendLog(env, job, "Send MSG");//发送日志
    }
    else {
        object.SendMsg(result);
    }
}
/**
* 名称:Java_com_example_plugin_CPP_1lib_GroupMessage
* 作用:处理群消息
* 参数: env:JNIEnv 必备， job:jobject 必备，groupid:jlong 群号，GroupName:jstring 群名，GroupOwnerId:jlong 群主qq号，


ernick:jstring 发送者昵称，sendergroupname:jstring 发送者群名称，senderid:jlong qq号，message:jstring 信息
* 返回值:void
**/
JNIEXPORT void JNICALL Java_com_example_plugin_CPP_1lib_GroupMessage
(JNIEnv* env, jobject job, jlong GroupId, jstring GroupName, jlong GroupOwnerId, jstring SenderNick, jstring SenderGroupName, jlong SenderId, jstring Message){
    Group object(env, job, GroupId);
    Friend Sender(env, job, SenderId);
    if (Sender.id == 1930893235) {
        object.SendMsg("hi");
    }
    //no reply
}


