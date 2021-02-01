#include "pch.h"

//ȫ��env����
extern JNIEnv* genv;

/*��־������*/
class Logger {
private:
	jclass CPP_lib = NULL;
	jmethodID sinfo = NULL;
	jmethodID swarning = NULL;
	jmethodID serror = NULL;
public:
	void init();
	/*������ͨ��־*/
	void Info(string log);
	/*���;���*/
	void Warning(string log);
	/*���ʹ���*/
	void Error(string log);
	~Logger();
};

/*����ȫ����־����*/
extern Logger* logger;

/*����������*/
class Config {
public:
	jclass CPP_lib = NULL;
	/*ͼ����*/
	jmethodID Query = NULL;
	/*������*/
	jmethodID SendMsg2F = NULL;
	jmethodID NickorNameF = NULL;
	/*Ⱥ�ĳ�Ա��*/
	jmethodID SendMsg2M = NULL;
	jmethodID NickorNameM = NULL;
	/*Ⱥ����*/
	jmethodID SendMsg2G = NULL;
	/*��ʱ����*/
	jmethodID Schedule = NULL;
	Config() {};
	void Init();
	~Config();
};

extern Config* config;

/*����������*/
class Tools {
public:
	/*
	* ����:jstring2str
	* ����:jstring��stringת��
	* ����:env:JNIEnv ����,jstr:jstring ת�����ı�
	* ����ֵ:string
	*/
	string jstring2str(jstring jstr);
	/*
	* ����:str2jstring
	* ����:string����תjstring����
	* ����:JNIEnv,const char*(string)
	* ����ֵ:jstring
	*/
	jstring str2jstring(const char* pat);
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
	jmethodID Query = NULL;
public:
	//ͼƬid����ʽ:` {xxx}.xx `
	string id = "";
	/*
	* ��ͼƬid���죬�����ڷ��������Ѿ��е�ͼƬ�������յ���
	* ͼƬmiraiCode����: [mirai:image:{ͼƬid}.jpg]
	* ���������������ʽ�ҳ�id ` \\[mirai:image:(.*?)\\] `
	* ʾ��:
		vector<string> temp = Image::GetImgIdFromMiraiCode(param.message);
		for (int i = 0; i < temp.size(); i++) {
			logger->Info(temp[i]);
			logger->Info("ͼƬ���ص�ַ:" + Image(param.env, temp[i]).queryURL());
		}
	*/
	Image(string);
	/*
	* �ϴ�����ͼƬ�����Ҫ�þ���·��
	* ����miraiҪ����ͼƬ���Ͷ�������ʹ�ñ������ϴ���ͼƬֻ�ܷ�������
	* ���֧��ͼƬ��СΪ30MB
	* �����׳�invalid_argument�쳣����·����Ч
	* ʾ��:
			param.sender.SendMsg(Image::uploadImg2Friend(param.env, param.sender.id, "C:\\Users\\***\\Desktop\\a.jpg").toMiraiCode());
	*/
	static Image uploadImg2Friend(unsigned long, string);
	/*
	* �ϴ�����ͼƬ�����Ҫ�þ���·��
	* ����miraiҪ����ͼƬ���Ͷ�������ʹ�ñ������ϴ���ͼƬֻ�ܷ���Ⱥ
	* ���֧��ͼƬ��СΪ30MB
	* �����׳�invalid_argument�쳣����·����Ч
	* ʾ��:
			param.group.SendMsg(Image::uploadImg2Group(param.env, param.group.id, "C:\\Users\\***\\Desktop\\a.jpg").toMiraiCode());
	*/
	static Image uploadImg2Group(unsigned long, string);
	/*
   * �ϴ�����ͼƬ�����Ҫ�þ���·��
   * ����miraiҪ����ͼƬ���Ͷ�������ʹ�ñ������ϴ���ͼƬֻ�ܷ���Ⱥ
   * ���֧��ͼƬ��СΪ30MB
   * �����׳�invalid_argument�쳣����·����Ч
   * ʾ��:
		   param.sender.SendMsg(Image::uploadImg2Member(param.env, param.group.id, param.sender.id, "C:\\Users\\***\\Desktop\\a.jpg").toMiraiCode());
   */
	static Image uploadImg2Member(unsigned long, unsigned long, string);

	/*
	* ��ȡͼƬ����url
	*/
	string queryURL();

	/*ȡȫ����ͼƬid�������Image*/
	static vector<string> GetImgIdFromMiraiCode(string);

	/*ȡͼƬMirai��*/
	string toMiraiCode();
};

/*����������*/
class Friend {
private:
	jmethodID Send_Msg_id = NULL;
	jmethodID NickorName_id = NULL;
public:
	unsigned long id = NULL;
	Friend(unsigned long);
	Friend() {};
	string nick;
	/*������Ϣ*/
	void SendMsg(jstring msg) {
		genv->CallStaticVoidMethod(config->CPP_lib, this->Send_Msg_id, msg, (jlong)this->id);
	}
	/*������Ϣ*/
	void SendMsg(string msg) {
		genv->CallStaticVoidMethod(config->CPP_lib, this->Send_Msg_id, tools.str2jstring(msg.c_str()), (jlong)this->id);
	}
};

/*Ⱥ��Ա������*/
class Member {
private:
	jmethodID Send_Msg_id = NULL;
	jmethodID NickorName_id = NULL;
public:
	unsigned long groupid = 0;
	unsigned long id = 0;
	// env, qqid, groupid
	Member(unsigned long, unsigned long);
	Member() {};
	string nameCard = "";
	/*����at����˵�miraicode*/
	string at() {
		return "[mirai:at:" + to_string(this->id) + "] ";
	}
	/*������Ϣ*/
	void SendMsg(jstring msg) {
		genv->CallStaticVoidMethod(config->CPP_lib, this->Send_Msg_id, msg, (jlong)this->id, (jlong)this->groupid);
	}
	/*������Ϣ*/
	void SendMsg(string msg) {
		genv->CallStaticVoidMethod(config->CPP_lib, this->Send_Msg_id, tools.str2jstring(msg.c_str()), (jlong)this->id, (jlong)this->groupid);
	}
};

/*Ⱥ��������*/
class Group {
private:
	jmethodID Send_Msg_id = NULL;
public:
	/*Ⱥ��*/
	unsigned long id = NULL;
	Group(unsigned long);
	Group() {};
	/*������Ϣ*/
	void SendMsg(jstring msg) {
		genv->CallStaticVoidMethod(config->CPP_lib, config->SendMsg2G, msg, (jlong)this->id);
	}
	/*������Ϣ*/
	void SendMsg(string msg) {
		SendMsg(tools.str2jstring(msg.c_str()));
	}
};

/*Ⱥ��Ϣ�¼�����*/
class GroupMessageEvent {
public:
	//��ԴȺ
	Group group;
	//������
	Member sender;
	//��Ϣ����
	string message;
	GroupMessageEvent(Group g, Member f, string s) {
		this->group = g;
		this->sender = f;
		this->message = s;
	}
};

/*˽����Ϣ�¼�������*/
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

/*��Ⱥ��Ա����*/
class MemberJoinEvent {
public:
	/*
	�¼�����
	1 - ���������
	2 - ��������
	3 - ԭȺ��ͨ�� https://huifu.qq.com/ �ָ�ԭ��Ⱥ����ݲ���Ⱥ
	*/
	int type = NULL;
	//�½���ĳ�Ա
	Member member;
	//Ŀ��Ⱥ
	Group group;
	//������, ��type = 1ʱ���ڣ��������member������ͬ
	Member invitor;
	MemberJoinEvent(int t, Member m, Group g, Member i) {
		this->type = t;
		this->member = m;
		this->group = g;
		this->invitor = i;
	}
};

/*Ⱥ��Ա�뿪*/
class MemberLeaveEvent {
public:
	/*
	�¼�����
	1 - ���߳�
	2 - �����˳�
	*/
	int type = NULL;
	//�˳��ĳ�Ա�ĳ�Ա
	Member member;
	//Ŀ��Ⱥ
	Group group;
	//������, �����˳�ʱ��member��ͬ���ĳ�Ա�����ǵ�ǰbot��operater����ϵͳoperator����
	Member operater;
	MemberLeaveEvent(int t, Member m, Group g, Member i) {
		this->type = t;
		this->member = m;
		this->group = g;
		this->operater = i;
	}
};

/*��ʱ����ִ��*/
class SchedulingEvent {
public:
	/*�Զ���id��ʶ��*/
	int id = 0;
	SchedulingEvent(int ido){
		id = ido;
	}
};

/*
������ʱ����,time�Ƕ��ٺ����ʼ��id���Զ����ʶ��
*/
void SetScheduling(long time, int id);

/*����������*/
class Event {
private:
	typedef std::function<void(GroupMessageEvent)> GME;
	typedef std::function<void(PrivateMessageEvent)> PME;
	typedef std::function<bool(GroupInviteEvent)> GI;
	typedef std::function<bool(NewFriendRequestEvent)> NFRE;
	typedef std::function<void(MemberJoinEvent)> MJ;
	typedef std::function<void(MemberLeaveEvent)> ML;
	typedef std::function<void(SchedulingEvent)> S;

	/*
	* ��ʹ��vector�����ظ������Ĳ��֣���Ϊû��ʲô��Ҫ��vector�ȱ���ռ�ڴ�
	*/

	GME GMEf = [](GroupMessageEvent)->void {};
	PME PMEf = [](PrivateMessageEvent)->void {};
	GI GIf = [](GroupInviteEvent)->bool {return true; };
	NFRE NFREf = [](NewFriendRequestEvent)->bool {return true; };
	MJ MJf = [](MemberJoinEvent)->void {};
	ML MLf = [](MemberLeaveEvent)->void {};
	S Sf = [](SchedulingEvent)->void {};
public:
	/*
	* �㲥��������
	*/

	void broadcast(GroupMessageEvent g) {
		this->GMEf(g);
	}
	void broadcast(PrivateMessageEvent p) {
		this->PMEf(p);
	}
	string broadcast(GroupInviteEvent g) {
		return (this->GIf(g) ? "true" : "false");
	}
	string broadcast(NewFriendRequestEvent g) {
		return (this->NFREf(g) ? "true" : "false");
	}
	void broadcast(MemberJoinEvent g) {
		this->MJf(g);
	}
	void broadcast(MemberLeaveEvent g) {
		this->MLf(g);
	}
	void broadcast(SchedulingEvent g) {
		this->Sf(g);
	}

	/*
	* ������������
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
		this->MJf = move(f);
	}
	void registerEvent(ML f) {
		this->MLf = move(f);
	}
	void registerEvent(S f) {
		this->Sf = f;
	}
};

/*����ע������¼�*/
void EventRegister();

/*����ȫ�ּ�������(�㲥Դ)*/
extern Event* procession;

/*����������úͽ�������*/
void onDisable();
void onEnable();