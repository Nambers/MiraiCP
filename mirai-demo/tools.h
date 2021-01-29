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
    * 示例:
        vector<string> temp = Image::GetImgIdFromMiraiCode(param.message);
		for (int i = 0; i < temp.size(); i++) {
			logger->Info(temp[i]);
			logger->Info("图片下载地址:" + Image(param.env, temp[i]).queryURL());
		}
    */
    Image(JNIEnv*, string);
    /*
    * 上传本地图片，务必要用绝对路径
    * 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到好友
    * 最大支持图片大小为30MB
    * 可能抛出invalid_argument异常代表路径无效
    * 示例:
        	param.sender.SendMsg(Image::uploadImg2Friend(param.env, param.sender.id, "C:\\Users\\***\\Desktop\\a.jpg").toMiraiCode());
    */
    static Image uploadImg2Friend(JNIEnv*,long, string);
    /*
    * 上传本地图片，务必要用绝对路径
    * 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
    * 最大支持图片大小为30MB
    * 可能抛出invalid_argument异常代表路径无效
    * 示例:
            param.group.SendMsg(Image::uploadImg2Group(param.env, param.group.id, "C:\\Users\\***\\Desktop\\a.jpg").toMiraiCode());
    */
    static Image uploadImg2Group(JNIEnv*, long, string);
    /*
   * 上传本地图片，务必要用绝对路径
   * 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
   * 最大支持图片大小为30MB
   * 可能抛出invalid_argument异常代表路径无效
   * 示例:
           param.sender.SendMsg(Image::uploadImg2Member(param.env, param.group.id, param.sender.id, "C:\\Users\\***\\Desktop\\a.jpg").toMiraiCode());
   */
    static Image uploadImg2Member(JNIEnv*,long, long, string);

    /*
    * 获取图片下载url
    */
    string queryURL();

    /*取全部的图片id，详情见Image*/
    static vector<string> GetImgIdFromMiraiCode(string);

    /*取图片Mirai码*/
    string toMiraiCode();
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
    // env, qqid, groupid
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

/*新群成员加入*/
class MemberJoinEvent {
public:
    /*
    事件类型
    1 - 被邀请进来
    2 - 主动加入
    3 - 原群主通过 https://huifu.qq.com/ 恢复原来群主身份并入群
    */
    int type;
    //新进入的成员
    Member member;
    //目标群
    Group group;
    //邀请人, 当type = 1时存在，否则是一位id为0的member，务必不要调用当invitor不存在，否则可能报错
    Member invitor;
    MemberJoinEvent(int t, Member m, Group g, Member i) {
        this->type = t;
        this->member = m;
        this->group = g;
        this->invitor = i;
    }
};

/*监听类声明*/
class Event {
private:
    typedef std::function<void(GroupMessageEvent param)> GME;
    typedef std::function<void(PrivateMessageEvent param)> PME;
    typedef std::function<bool(GroupInviteEvent param)> GI;
    typedef std::function<bool(NewFriendRequestEvent param)> NFRE;
    typedef std::function<void(MemberJoinEvent param)> MJ;
    GME GMEf;
    PME PMEf;
    // 邀请的处理函数的唯一的
    NFRE NFREf;
    GI GIf;
    MJ MJf;
public:
    /*
    * 广播函数重载
    */

    void broadcast(GroupMessageEvent g) {
        //如果处理长度为0，即未注册，不处理
        if (GMEf == NULL) {
            return;
        }
         this->GMEf(g);
    }
    void broadcast(PrivateMessageEvent p) {
        //如果处理长度为0，即未注册，不处理
        if (PMEf == NULL) {
            return;
        }
        this->PMEf(p);
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
    void broadcast(MemberJoinEvent g) {
        this->MJf(g);
    }

    /*
    * 监听函数重载
    */

    void registerEvent(GME f) {
        this->GMEf = move(f);
    }
    void registerEvent(PME f) {
        this->PMEf = move(f);
    }
    void registerEvent(GI f) {
        this->GIf = std::move(f);
    }
    void registerEvent(NFRE f) {
        this->NFREf = std::move(f);
    }
    void registerEvent(MJ f) {
        this->MJf = f;
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