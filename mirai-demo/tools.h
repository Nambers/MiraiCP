#include "pch.h"
/*日志类定义*/
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

/*工具类定义*/
class Tools {
public:
    bool replace(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }
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

/*静态声明工具对象*/
static Tools tools;

/*图像类声明*/
class Image {
private:
    JNIEnv* env;
    jclass java_class;
    jmethodID Query;
public:
    string id = "";
    /*
    * 从图片id构造，适用于服务器上已经有的图片，即接收到的
    * 图片miraiCode例子: [mirai:image:{图片id}.jpg]
    * 可以用这个正则表达式找出id ` \\[mirai:image:(.*?)\\] `
    */
    Image(JNIEnv*, string);
    /*
    * 获取图片下载url
    */
    string queryURL();

    /*取全部的图片id*/
    static vector<string> GetImgIdFromMiraiCode(string);
};

/*好友类声明*/
class Friend {
private:
    jclass java_first;
    jmethodID Send_Msg_id;
    jmethodID NickorName_id;
    JNIEnv* env;
public:
    long id;
    Friend(JNIEnv*, long);
    Friend() {};
    ~Friend();
    string nick;
    void SendMsg(jstring msg) {
        this->env->CallStaticVoidMethod(this->java_first, this->Send_Msg_id, msg, (jlong)this->id);
    }
    void SendMsg(string msg) {
        this->env->CallStaticVoidMethod(this->java_first, this->Send_Msg_id, tools.str2jstring(this->env, msg.c_str()), (jlong)this->id);
    }
};

/*群成员类声明*/
class Member{
private:
    jclass java_first;
    jmethodID Send_Msg_id;
    jmethodID NickorName_id;
    JNIEnv* env;
public:
    long groupid;
    long id;
    Member(JNIEnv*, long, long);
    Member() {};
    ~Member();
    string nameCard;
    string at() {
        return "[mirai:at:"+ to_string(this->id) + "] ";
    }
    void SendMsg(jstring msg) {
        this->env->CallStaticVoidMethod(this->java_first, this->Send_Msg_id, msg, (jlong)this->id, (jlong)this->groupid);
    }
    void SendMsg(string msg) {
        this->env->CallStaticVoidMethod(this->java_first, this->Send_Msg_id, tools.str2jstring(this->env, msg.c_str()), (jlong)this->id, (jlong)this->groupid);
    }
};

/*群聊类声明*/
class Group {
private:
    jclass java_first;
    jmethodID Send_Msg_id;
    JNIEnv* env;
public:
    long id;
    ~Group();
    Group(JNIEnv*, long);
    Group() {};
    void SendMsg(jstring msg) {
        this->env->CallStaticVoidMethod(java_first, Send_Msg_id, msg, (jlong)this->id);
    }
    void SendMsg(string msg) {
        SendMsg(tools.str2jstring(this->env, msg.c_str()));
    }
};

/*群消息事件声明*/
class GroupMessageEvent {
public:
    /*jni参数，可以不用管*/
    JNIEnv* env;
    //来源群
    Group group;
    //发送人
    Member sender;
    //信息本体
    string message;
    GroupMessageEvent(JNIEnv* env, Group g, Member f, string s) {
        this->env = env;
        this->group = g;
        this->sender = f;
        this->message = s;
    }
};

/*私聊消息事件类声明*/
class PrivateMessageEvent {
public:
    /*jni参数，可以不用管*/
    JNIEnv* env;
    //发起人
    Friend sender;
    //附带消息
    string message;
    PrivateMessageEvent(JNIEnv* env, Friend f, string s) {
        this->env = env;
        this->sender = f;
        this->message = s;
    }
};

/*群聊邀请事件类声明*/
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

/*好友申请事件声明*/
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

/*监听类声明*/
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

/*声明注册监听事件*/
void EventRegister();

/*声明全局监听对象(广播源)*/
extern Event* procession;

/*声明全局日志对象*/
extern Logger* logger;

/*声明插件启用和结束函数*/
void onDisable();
void onEnable();