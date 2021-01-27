#include "pch.h"
/*��־�ඨ��*/
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

/*�����ඨ��*/
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

/*��̬�������߶���*/
static Tools tools;

/*ͼ��������*/
class Image {
private:
    JNIEnv* env;
    jclass java_class;
    jmethodID Query;
public:
    string id = "";
    /*
    * ��ͼƬid���죬�����ڷ��������Ѿ��е�ͼƬ�������յ���
    * ͼƬmiraiCode����: [mirai:image:{ͼƬid}.jpg]
    * ���������������ʽ�ҳ�id ` \\[mirai:image:(.*?)\\] `
    */
    Image(JNIEnv*, string);
    /*
    * ��ȡͼƬ����url
    */
    string queryURL();

    /*ȡȫ����ͼƬid*/
    static vector<string> GetImgIdFromMiraiCode(string);
};

/*����������*/
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

/*Ⱥ��Ա������*/
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

/*Ⱥ��������*/
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

/*Ⱥ��Ϣ�¼�����*/
class GroupMessageEvent {
public:
    /*jni���������Բ��ù�*/
    JNIEnv* env;
    //��ԴȺ
    Group group;
    //������
    Member sender;
    //��Ϣ����
    string message;
    GroupMessageEvent(JNIEnv* env, Group g, Member f, string s) {
        this->env = env;
        this->group = g;
        this->sender = f;
        this->message = s;
    }
};

/*˽����Ϣ�¼�������*/
class PrivateMessageEvent {
public:
    /*jni���������Բ��ù�*/
    JNIEnv* env;
    //������
    Friend sender;
    //������Ϣ
    string message;
    PrivateMessageEvent(JNIEnv* env, Friend f, string s) {
        this->env = env;
        this->sender = f;
        this->message = s;
    }
};

/*Ⱥ�������¼�������*/
class GroupInviteEvent {
public:
    //������
    Friend sender;
    //�����������
    Group group;
    GroupInviteEvent(Group g, Friend f) {
        this->sender = f;
        this->group = g;
    }
};

/*���������¼�����*/
class NewFriendRequestEvent {
public:
    //������
    Friend sender;
    //������Ϣ
    string message;
    NewFriendRequestEvent(Friend f, string s) {
        this->sender = f;
        this->message = s;
    }
};

/*����������*/
class Event {
private:
    typedef std::function<void(GroupMessageEvent param)> GME;
    typedef std::function<void(PrivateMessageEvent param)> PME;
    typedef std::function<bool(GroupInviteEvent param)> GI;
    typedef std::function<bool(NewFriendRequestEvent param)> NFRE;
    vector<GME> GMEa;
    vector<PME> PMEa;
    // ����Ĵ�������Ψһ��
    NFRE NFREf = NFRE();
    GI GIf = GI();
public:
    /*
    * �㲥��������
    */

    void broadcast(GroupMessageEvent g) {
        //���������Ϊ0����δע�ᣬ������
        if (GMEa.size() <= 0) {
            return;
        }
        for (int i = 0; i < GMEa.size(); i++) {
            this->GMEa[i](g);
        }
    }
    void broadcast(PrivateMessageEvent p) {
        //���������Ϊ0����δע�ᣬ������
        if (PMEa.size()<= 0) {
            return;
        }
        for (int i = 0; i < PMEa.size(); i++) {
            this->PMEa[i](p);
        }
    }
    string broadcast(GroupInviteEvent g) {
        if (GIf == NULL) {
            //Ĭ���Զ��ܾ�
            return "false";
        }
        return (this->GIf(g) ? "true" : "false");
    }

    string broadcast(NewFriendRequestEvent g) {
        if (GIf == NULL) {
            //Ĭ���Զ��ܾ�
            return "false";
        }
        return (this->NFREf(g) ? "true" : "false");
    }

    /*
    * ������������
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

/*����ע������¼�*/
void EventRegister();

/*����ȫ�ּ�������(�㲥Դ)*/
extern Event* procession;

/*����ȫ����־����*/
extern Logger* logger;

/*����������úͽ�������*/
void onDisable();
void onEnable();