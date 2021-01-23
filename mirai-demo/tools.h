#include "pch.h"
/*
* ��־����
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

/*
* ���Ѷ���
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
* qq��Ⱥ�Ķ���
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
    //��ԴȺ
    Group group;
    //������
    Friend sender;
    //��Ϣ����
    string message;
    GroupMessageEvent(Group g, Friend f, string s) {
        this->group = g;
        this->sender = f;
        this->message = s;
    }
};

class PrivateMessageEvent {
public:
    //������
    Friend sender;
    //������Ϣ
    string message;
    PrivateMessageEvent(Friend f, string s) {
        this->sender = f;
        this->message = s;
    }
};

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

/*
* �¼�����
*/
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
void EventRegister();
extern Event procession;
extern Logger logger;

