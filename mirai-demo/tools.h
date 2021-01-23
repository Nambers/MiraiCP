#include "pch.h"
/*
* 日志对象
*/
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

/*
* 好友对象
*/
class Friend {
private:
    jclass java_first;
    jmethodID Send_Msg_id;
    //jmethodID Nick_Name_id;
    JNIEnv* env;
public:
    long id;
    Friend(JNIEnv* env, jobject job, long id);
    Friend() {};
    ~Friend();
    //string GetNick();
    //string nick;
    void SendMsg(jstring msg) {
        this->env->CallStaticVoidMethod(this->java_first, this->Send_Msg_id, msg, (jlong)this->id);
    }
    void SendMsg(string msg) {
        this->env->CallStaticVoidMethod(this->java_first, this->Send_Msg_id, tools.str2jstring(this->env, msg.c_str()), (jlong)this->id);
    }
};

/*
* qq中群聊对象
*/
class Group {
private:
    jclass java_first;
    jmethodID Send_Msg_id;
    JNIEnv* env;
public:
    long id;
    ~Group();
    Group(JNIEnv* env, jobject job, long id);
    Group() {};
    void SendMsg(jstring msg) {
        this->env->CallStaticVoidMethod(java_first, Send_Msg_id, msg, (jlong)this->id);
    }
    void SendMsg(string msg) {
        SendMsg(tools.str2jstring(this->env, msg.c_str()));
    }
};

class GroupMessageEvent {
public:
    //来源群
    Group group;
    //发送人
    Friend sender;
    //信息本体
    string message;
    GroupMessageEvent(Group g, Friend f, string s) {
        this->group = g;
        this->sender = f;
        this->message = s;
    }
};

class PrivateMessageEvent {
public:
    //发起人
    Friend sender;
    //附带消息
    string message;
    PrivateMessageEvent(Friend f, string s) {
        this->sender = f;
        this->message = s;
    }
};

class GroupInviteEvent {
public:
    //发起人
    Friend sender;
    //被邀请进的组
    Group group;
    GroupInviteEvent(Group g, Friend f) {
        this->sender = f;
        this->group = g;
    }
};

class NewFriendRequestEvent {
public:
    //发起者
    Friend sender;
    //附加信息
    string message;
    NewFriendRequestEvent(Friend f, string s) {
        this->sender = f;
        this->message = s;
    }
};

/*
* 事件监听
*/
class Event {
private:
    typedef std::function<void(GroupMessageEvent param)> GME;
    typedef std::function<void(PrivateMessageEvent param)> PME;
    typedef std::function<bool(GroupInviteEvent param)> GI;
    typedef std::function<bool(NewFriendRequestEvent param)> NFRE;
    vector<GME> GMEa;
    vector<PME> PMEa;
    // 邀请的处理函数的唯一的
    NFRE NFREf = NFRE();
    GI GIf = GI();
public:
    /*
    * 广播函数重载
    */

    void broadcast(GroupMessageEvent g) {
        //如果处理长度为0，即未注册，不处理
        if (GMEa.size() <= 0) {
            return;
        }
        for (int i = 0; i < GMEa.size(); i++) {
            this->GMEa[i](g);
        }
    }
    void broadcast(PrivateMessageEvent p) {
        //如果处理长度为0，即未注册，不处理
        if (PMEa.size()<= 0) {
            return;
        }
        for (int i = 0; i < PMEa.size(); i++) {
            this->PMEa[i](p);
        }
    }
    string broadcast(GroupInviteEvent g) {
        if (GIf == NULL) {
            //默认自动拒绝
            return "false";
        }
        return (this->GIf(g) ? "true" : "false");
    }

    string broadcast(NewFriendRequestEvent g) {
        if (GIf == NULL) {
            //默认自动拒绝
            return "false";
        }
        return (this->NFREf(g) ? "true" : "false");
    }

    /*
    * 监听函数重载
    */

    void registerEvent(GME f) {
        GMEa.push_back(move(f));
    }
    void registerEvent(PME f) {
        PMEa.push_back(move(f));
    }
    void registerEvent(GI f) {
        this->GIf = std::move(f);
    }
    void registerEvent(NFRE f) {
        this->NFREf = std::move(f);
    }
};
void EventRegister();
extern Event procession;
extern Logger logger;

