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
	jclass initexception = NULL;
	/*ͼ����*/
	jmethodID Query = NULL;
	/*������*/
	jmethodID SendMsg2F = NULL;
	jmethodID SendMsg2FM = NULL;
	jmethodID NickorNameF = NULL;
	/*Ⱥ�ĳ�Ա��*/
	jmethodID SendMsg2M = NULL;
	jmethodID SendMsg2MM = NULL;
	jmethodID NickorNameM = NULL;
	jmethodID Mute = NULL;
	jmethodID QueryP = NULL;
	jmethodID KickM = NULL;
	/*Ⱥ����*/
	jmethodID SendMsg2G = NULL;
	jmethodID SendMsg2GM = NULL;
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
	/*
	* �滻ȫ��
	* ��Դ:https://blog.csdn.net/yanchenyu365/article/details/89181129
	*/
	string& replace(string& str, const string& old_value, const string& new_value)
	{
		string::size_type pos = 0;
		while ((pos = str.find(old_value, pos)) != string::npos)
		{
			str = str.replace(pos, old_value.length(), new_value);
			if (new_value.length() > 0)
			{
				pos += new_value.length();
			}
		}
		return str;

	}
};

/*��̬�������߶���*/
static Tools tools;

//���쳣
class MiraiCPException :public exception {
public:
	virtual string what() { return "C++���ֳ����˸�δ������쳣"; };
	virtual void raise() {};
};
//��ʼ���쳣
class InitException :public MiraiCPException
{
public:
	int step = 0;
	InitException(string text, int step)
	{
		this->description = text;
		this->step = step;
	}
	//���ش�����Ϣ
	string what()
	{
		return this->description;
	}
	void raise() {
		genv->ThrowNew(config->initexception, (this->description + " step:" + to_string(this->step)).c_str());
	}
private:
	string description;
};
//�ļ���ȡ�쳣
class IOException :public MiraiCPException
{
public:
	IOException(string text)
	{
		this->description = "�ļ���ȡ�쳣" + text;
	}
	//���ش�����Ϣ
	string what()
	{
		return this->description;
	}
	void raise() {
		logger->Error(this->description);
		//genv->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	string description;
};
//�ڲ��쳣
class APIException :public MiraiCPException
{
public:
	APIException(string text)
	{
		this->description = "MiraiCP�ڲ��޷�Ԥ�ϵĴ���:" + text;
	}
	//���ش�����Ϣ
	string what()
	{
		return this->description;
	}
	void raise() {
		logger->Error(this->description);
		//genv->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	string description;
};
//�����˲����쳣
class BotException : public MiraiCPException {
public:
	/*
	*	1 - ��������Ȩ�޽��ԶԷ�
	*/
	int type = 0;
	BotException(int type)
	{
		this->type = type;
		switch (type)
		{
		case 1:
			this->description = "û��Ȩ�޽���";
			break;
		}
	}
	//���ش�����Ϣ
	string what()
	{
		return this->description;
	}
	void raise() {
		//genv->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	string description;
};
//�����쳣
class MuteException :public MiraiCPException
{
public:
	/*
	*	 ����ʱ�䳬��0s~30d
	*/
	MuteException()
	{
		this->description = "����ʱ������0s~30d�м�";
	}
	//���ش�����Ϣ
	string what()
	{
		return this->description;
	}
	void raise() {
		//genv->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	string description;
};
//��ȡȺ��Ա����
class MemberException :public MiraiCPException
{
public:
	/*
	*   "1" - �Ҳ���Ⱥ
	*	"2" - �Ҳ���Ⱥ��Ա
	*/
	int type = 0;
	MemberException(int type)
	{
		this->type = type;
		switch (type)
		{
		case 1:
			this->description = "�Ҳ���Ⱥ";
			break;
		case 2:
			this->description = "�Ҳ���Ⱥ��Ա";
			break;
		}
	}
	//���ش�����Ϣ
	string what()
	{
		return this->description;
	}
	void raise() {
		//genv->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	string description;
};
//��ȡȺ��Ա����
class FriendException :public MiraiCPException
{
public:
	/*
	*   �Ҳ�������
	*/
	FriendException()
	{
		this->description = "�Ҳ�������";
	}
	//���ش�����Ϣ
	string what()
	{
		return this->description;
	}
	void raise() {
		//genv->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	string description;
};
//��ȡȺ����
class GroupException :public MiraiCPException
{
public:
	/*
	*   "1" - �Ҳ���Ⱥ
	*/
	int type = 0;
	GroupException(int type)
	{
		this->type = type;
		switch (type)
		{
		case 1:
			this->description = "�Ҳ���Ⱥ";
			break;
		}
	}
	//���ش�����Ϣ
	string what()
	{
		return this->description;
	}
	void raise() {
		//genv->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	string description;
};

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
			param.sender.
			(Image::uploadImg2Friend(param.env, param.sender.id, "C:\\Users\\***\\Desktop\\a.jpg").toMiraiCode());
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
	static vector<string> GetImgIdsFromMiraiCode(string);

	/*ȡͼƬMirai��*/
	string toMiraiCode();
};

class LightApp {
public:
	string content = "";
	LightApp(string content) {
		this->content = content;
	};
	string toString() {
		return "[mirai:app:" + tools.replace(tools.replace(content, "[", "\\["),"]","\\]") +"]";
	}
};

/*����������*/
class Friend {
public:
	unsigned long id = NULL;
	Friend(unsigned long);
	Friend() {};
	//��ʼ����ǰ���󣬿����׳��쳣
	void init();
	string nick;
	/*������Ϣ*/
	void SendMiraiCode(string msg)throw(FriendException) {
		string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2F, tools.str2jstring(msg.c_str()), (jlong)this->id));
		if (re == "E1") {
			throw FriendException();
		}
	}
	void SendMsg(string msg)throw(FriendException) {
		string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2FM, tools.str2jstring(msg.c_str()), (jlong)this->id));
		if (re == "E1") {
			throw FriendException();
		}
	}
};

/*Ⱥ��Ա������*/
class Member {
private:
	jmethodID Mute_id = NULL;
	jmethodID Query_permission = NULL;
	jmethodID KickM = NULL;
public:
	unsigned long groupid = 0;
	unsigned long id = 0;
	// Ȩ�޵ȼ�. OWNERȺ�� Ϊ 2, ADMINISTRATOR����Ա Ϊ 1, MEMBERȺ��Ա Ϊ 0
	unsigned int permission = 0;
	// qqid, groupid
	Member(unsigned long qqid, unsigned long groupid);
	//��ʼ����ǰ���󣬿����׳��쳣
	void init();
	Member() {};
	int getPermission();
	string nameCard = "";
	/*����at����˵�miraicode*/
	string at() {
		return "[mirai:at:" + to_string(this->id) + "] ";
	}
	/*������Ϣ*/
	void SendMiraiCode(string msg)throw(MemberException) {
		string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2M, tools.str2jstring(msg.c_str()), (jlong)this->id, (jlong)this->groupid));
		if (re == "E1") {
			throw MemberException(1);
		}
		if (re == "E2") {
			throw MemberException(2);
		}
	}
	void SendMsg(string msg) throw(MemberException) {
		string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2MM, tools.str2jstring(msg.c_str()), (jlong)this->id, (jlong)this->groupid));
		if (re == "E1") {
			throw MemberException(1);
		}
		if (re == "E2") {
			throw MemberException(2);
		}
	}
	/*���Ե�ǰ���󣬵�λ���룬����0�����30��
	* ����ֵ��Ӧ����
	*	"E1" - �Ҳ���Ⱥ
	*	"E2" - �Ҳ���Ⱥ��Ա
	*	"E3" - ��������Ȩ�޽��ԶԷ�
	*	"E4" - ����ʱ�䳬��0s~30d
	*	"Y" - һ������
	*/
	void Mute(int time);
	/*�߳����Ⱥ��Ա*/
	void Kick(string reason);
};

/*Ⱥ��������*/
class Group {
public:
	/*Ⱥ��*/
	unsigned long id = NULL;
	Group(unsigned long);
	//��ʼ����ǰ���󣬿����׳��쳣-��������
	void init() {};
	Group() {};
	/*������Ϣ*/
	void SendMiraiCode(string msg)throw(GroupException) {
		string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2G, tools.str2jstring(msg.c_str()), (jlong)this->id));
		if (re == "E1") {
			throw GroupException(1);
		}
	}
	void SendMsg(string msg) throw(GroupException) {
		string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2GM, tools.str2jstring(msg.c_str()), (jlong)this->id));
		if (re == "E1") {
			throw GroupException(1);
		}
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
	void init() {
		group.init();
		sender.init();
	}
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
	void init() throw(FriendException){
		this->sender.init();
	}
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
	void init() {
		sender.init();
		group.init();
	}
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
	void init() throw (FriendException){
		this->sender.init();
	}
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
	void init() throw(MemberException){
		this->member.init();
		this->group.init();
		this->invitor.init();
	}
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
	/*�˳��ĳ�Ա�ĳ�Ա*/
	Member member;
	/*Ŀ��Ⱥ*/
	Group group;
	/*������, �����˳�ʱ��member��ͬ���ĳ�Ա�����ǵ�ǰbot��operater����ϵͳoperator����*/
	Member operater;
	//��ʼ���¼�
	void init() throw(MemberException){
		this->member.init();
		this->group.init();
		this->operater.init();
	}
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
		try {
			this->MLf(g);
		}
		catch (MiraiCPException e) {
			e.raise();
		}
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

/*����ȫ�ּ�������(�㲥Դ)*/
extern Event* procession;

/*����������úͽ�������*/
void onDisable();
void onEnable();