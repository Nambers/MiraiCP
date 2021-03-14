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
	jmethodID recallMsgM = NULL;
	/*ͼ����*/
	jmethodID Query = NULL;
	/*������*/
	jmethodID SendMsg2F = NULL;
	jmethodID SendMsg2FM = NULL;
	jmethodID NickorNameF = NULL;
	jmethodID uploadImgF = NULL;
	/*Ⱥ�ĳ�Ա��*/
	jmethodID SendMsg2M = NULL;
	jmethodID SendMsg2MM = NULL;
	jmethodID NickorNameM = NULL;
	jmethodID Mute = NULL;
	jmethodID QueryP = NULL;
	jmethodID uploadImgM = NULL;
	jmethodID KickM = NULL;
	/*Ⱥ����*/
	jmethodID getowner = NULL;
	jmethodID muteAll = NULL;
	jmethodID SendMsg2G = NULL;
	jmethodID uploadImgG = NULL;
	jmethodID SendMsg2GM = NULL;
	jmethodID QueryN = NULL;
	jmethodID QueryML = NULL;
	/*��ʱ����*/
	jmethodID Schedule = NULL;
	jmethodID buildforward = NULL;
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
	* ��Դ:https://blog.csdn.net/chunleixiahe/article/details/51394116
	*/
	string jstring2str(jstring jstr);
	/*
	* ����:str2jstring
	* ����:string����תjstring����
	* ����:JNIEnv,const char*(string)
	* ����ֵ:jstring
	* ��Դ:https://blog.csdn.net/chunleixiahe/article/details/51394116
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
	* ��Դ:https://stackoverflow.com/a/24315631/14646226
	*/
	std::string replace(std::string str, const std::string& from, const std::string& to) {
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
		}
		return str;
	}
	/*
	* JSONת����String
	* ��Դ:https://www.cnblogs.com/zhangdongsheng/p/12731021.html
	*/
	std::string JsonToString(const Json::Value& root);
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
	*	1 - ��������Ȩ��ִ��
	*/
	int type = 0;
	BotException(int type)
	{
		this->type = type;
		switch (type)
		{
		case 1:
			this->description = "û��Ȩ��ִ�иò���";
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
	GroupException()
	{
		this->description = "�Ҳ���Ⱥ";
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
class RecallException: public MiraiCPException{
public:
	RecallException() {
		this->description = "����Ϣ�Ѿ�������";
	}
	string what() {
		return this->description;
	}
private:
	string description = "";
};

//��ϢԴ����
class MessageSource {
private:
	string source;
public:
	string ids = "";
	string internalids = "";
	string toString() {
		return source;
	}
	MessageSource() {};
	MessageSource(string t);
	void recall() {
		string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->recallMsgM,
			tools.str2jstring(this->toString().c_str())));
		if (re == "Y") return;
		if (re == "E1") throw BotException(1);
		if (re == "E2") throw RecallException();
	}
};

//MiraiCode
class MiraiCodeable {
public:
	virtual string toMiraiCode() = 0;
};
class MiraiCode {
private:
	string content = "";
public:
	string toString() {
		return content;
	}
	MiraiCode(MiraiCodeable* a) {
		content = a->toMiraiCode();
	}
	MiraiCode(string a) {
		content = a;
	}
	MiraiCode operator+(MiraiCodeable* a) {
		return MiraiCode(content + a->toMiraiCode());
	}
	MiraiCode operator+(string a) {
		return MiraiCode(content + a);
	}
	MiraiCode plus(MiraiCodeable* a) {
		return MiraiCode(content + a->toMiraiCode());
	}
	MiraiCode plus(string a) {
		return MiraiCode(content + a);
	}
};

/*С�����Ϳ�Ƭ*/
class LightApp: public MiraiCodeable {
public:
	string content = "";
	//ʹ�ô��ı����죬�Ƽ�ʹ�������ṹ�巽������
	LightApp(string content) {
		this->content = content;
	}
	//��ʽ1,�ʺ�����չʾ���޴�ͼ�����ܽ���
	LightApp(LightAppStyle1 c) {
		this->content = "{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[Ӧ��]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":"
			"{\"appName\":\"" + c.appName + "\",\"appType\":4,\"appid\":1109659848,"
			"\"iconUrl\":\"" + c.icon + "\"},"
			"\"data\":[" + c.titles + "],"
			"\"title\":\"" + c.title + "\",\"button\":"
			"[" + c.buttons + "],"
			"\"emphasis_keyword\":\"\"}},\"text\":\"\",\"sourceAd\":\"\"}";
	}
	//��ʽ2���д�ͼ�����ܽ���
	LightApp(LightAppStyle2 c) {
		this->content = "{\"config\":"
			"{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQС����]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"1.0.0.103\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1110081493\",\"preview\":\"" + c.preview + "\",\"shareTemplateData\":{},"
			"\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" + c.title2 + "\",\"title\":\"" + c.title + "\","
			"\"host\":{\"uin\":0,\"nick\":\"\"},"
			"\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" + c.icon + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
	}
	//��ʽ3���д�ͼ�������ڵ���qq��ʾ�����ڵ����ϵ�������ӻ���ת
	LightApp(LightAppStyle3 c) {
		this->content = "{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQС����]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\"" + c.preview + "\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" + c.title + "\",\"title\":\"" + c.description + "\","
			"\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" + c.icon + "\",\"qqdocurl\":\"" + c.url + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
	}
	string toMiraiCode() {
		return "[mirai:app:" + tools.replace(tools.replace(content, "[", "\\["), "]", "\\]") + "]";
	}
};

/*ͼ��������*/
class Image: public MiraiCodeable {
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
	* ��ȡͼƬ����url
	*/
	string queryURL();

	/*ȡȫ����ͼƬid�������Image*/
	static vector<string> GetImgIdsFromMiraiCode(string);

	/*ȡͼƬMirai��*/
	string toMiraiCode();
};

class Contact {
public:
	//1-Friend, 2-group, 3-member
	int type;
	//id
	unsigned long long id;
	//��type=3ʱ����
	unsigned long long groupid = 0;
};

class ForwardNode {
public:
	unsigned long long id = 0;
	string name = "";
	string message = "";
	int time = 0;
	/*
	�����¼���ÿ����Ϣ
		i - ������id
		n - �������ǳ�
		m - ���͵���Ϣ
		t - ����ʱ�䣬��ʱ�����
	*/
	ForwardNode(unsigned long long i, string n, string m, int t) {
		this->id = i;
		this->name = n;
		this->message = m;
		this->time = t;
	}
};

//�����¼
class ForwardMessage {
private:
	Json::Value sendmsg;
public:
	/*
	����һ�������¼
	����:
			ForwardMessage(&e.group,
			{
				ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
				ForwardNode(1930893235, "Eritque arcus", "hahaha", -1)
			}).sendTo(&e.group);
	*/
	ForwardMessage(Contact* c, initializer_list<ForwardNode> nodes) {
		Json::Value root;
		Json::Value value;
		root["type"] = c->type;
		root["id"] = c->id;
		root["id2"] = c->groupid;
		for (ForwardNode node : nodes) {
			Json::Value temp;
			temp["id"] = node.id;
			temp["time"] = node.time;
			temp["message"] = node.message;
			temp["name"] = node.name;
			value.append(temp);
		}
		root["value"] = value;
		sendmsg = root;
	}
	void sendTo(Contact* c);
};
string BuildForwardMessage(Contact*, initializer_list<ForwardNode>);

/*����������*/
class Friend:public Contact{
public:
	Friend(unsigned long long);
	Friend() {};
	//��ʼ����ǰ���󣬿����׳��쳣
	//�ǳ�
	string nick;
	/*
	* �ϴ�����ͼƬ�����Ҫ�þ���·��
	* ����miraiҪ����ͼƬ���Ͷ�������ʹ�ñ������ϴ���ͼƬֻ�ܷ�������
	* ���֧��ͼƬ��СΪ30MB
	* �����׳�invalid_argument�쳣����·����Ч
	*/
	Image uploadImg(string filename);
	/*������Ϣ*/
	 MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	 MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(string msg);
	MessageSource SendMsg(string msg);
};

/*Ⱥ��Ա������*/
class Member :public Contact {
private:
	jmethodID Mute_id = NULL;
	jmethodID Query_permission = NULL;
	jmethodID KickM = NULL;
public:
	// Ȩ�޵ȼ�. OWNERȺ�� Ϊ 2, ADMINISTRATOR����Ա Ϊ 1, MEMBERȺ��Ա Ϊ 0
	unsigned int permission = 0;
	// qqid, groupid
	Member(unsigned long long qqid, unsigned long long groupid);
	/*
	* �ϴ�����ͼƬ�����Ҫ�þ���·��
	* ����miraiҪ����ͼƬ���Ͷ�������ʹ�ñ������ϴ���ͼƬֻ�ܷ���Ⱥ
	* ���֧��ͼƬ��СΪ30MB
	* �����׳�invalid_argument�쳣����·����Ч
	*/
	Image uploadImg(string filename);
	Member() {};
	int getPermission();
	string nameCard = "";
	/*������Ϣ*/
	 MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	 MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(string msg);
	MessageSource SendMsg(string msg);
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
class Group :public Contact {
private:
	string memberlist = "";
public:
	/*Ⱥ����*/
	string name = "";
	/*ȡȺ��Ա�б�-vector<long>*/
	vector<unsigned long long> getMemberList();
	/*��string��ʽȡȺ��Ա�б�
	��ʽ��
		ÿ��Ⱥ��Աid���ö��ŷָ�
	*/
	string MemberListToString();
	//ȡȺ��
	Member getOwner();
	Group(unsigned long long);
	/*
	* �ϴ�����ͼƬ�����Ҫ�þ���·��
	* ����miraiҪ����ͼƬ���Ͷ�������ʹ�ñ������ϴ���ͼƬֻ�ܷ���Ⱥ
	* ���֧��ͼƬ��СΪ30MB
	* �����׳�invalid_argument�쳣����·����Ч
	*/
	Image uploadImg(string filename);
	Group() {};
	/*
	* ����ȫԱ����
	* param: sign = trueʱΪ��ʼ��falseΪ�ر�
	*/
	void setMuteAll(bool sign) throw(GroupException, BotException);
	/*������Ϣ*/
	MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(string msg);
	MessageSource SendMsg(string msg);
};

/*Atһ��Ⱥ��Ա*/
 inline string At(Member a) {
	/*����at����˵�miraicode*/
	return "[mirai:at:" + to_string(a.id) + "]";
}
 /*Atһ��Ⱥ��Ա*/
 inline string At(unsigned long long a) {
	/*����at����˵�miraicode*/
	return "[mirai:at:" + to_string(a) + "]";
}

/*Ⱥ��Ϣ�¼�����*/
class GroupMessageEvent {
public:
	//��ԴȺ
	Group group;
	//������
	Member sender;
	//��Ϣ����
	string message;
	//��ϢԴ
	MessageSource messageSource;
	GroupMessageEvent(Group g, Member f, string s, MessageSource s1) {
		this->group = g;
		this->sender = f;
		this->message = s;
		this->messageSource = s1;
	}
};

/*˽����Ϣ�¼�������*/
class PrivateMessageEvent {
public:
	//������
	Friend sender;
	//������Ϣ
	string message;
	//��ϢԴ
	MessageSource messageSource;
	PrivateMessageEvent(Friend f, string s, MessageSource ms) {
		this->sender = f;
		this->message = s;
		this->messageSource = ms;
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
	/*�˳��ĳ�Ա�ĳ�Ա*/
	Member member;
	/*Ŀ��Ⱥ*/
	Group group;
	/*������, �����˳�ʱ��member��ͬ���ĳ�Ա�����ǵ�ǰbot��operater����ϵͳoperator����*/
	Member operater;
	MemberLeaveEvent(int t, Member m, Group g, Member i) {
		this->type = t;
		this->member = m;
		this->group = g;
		this->operater = i;
	}
};

/*������Ϣ*/
class RecallEvent {
public:
	/*
	Ϊ1ʱ�Ǻ��ѳ��أ�Ϊ2ʱΪȺ���ڳ���
	*/
	int type = 0;
	//ʱ���
	int time = 0;
	//ԭ������
	unsigned long authorid = 0;
	//������
	unsigned long operatorid = 0;
	//��Ϣid
	string ids;
	//�ڲ�ids
	string internalids;
	//��type��2��ʱ����ڣ�����Ϊ0
	unsigned long groupid;
	RecallEvent() {}
	RecallEvent(int t, int t2, unsigned long a, unsigned long o, string id, string ii, unsigned long g = 0) {
		this->type = t;
		this->time = t2;
		this->authorid = a;
		this->operatorid = o;
		this->ids = id;
		this->internalids = ii;
		this->groupid = g;
	}
};

/*������ʱ����,time�Ƕ��ٺ����ʼ��id���Զ����ʶ��*/
inline void SetScheduling(long time, std::initializer_list<std::string> args) {
	Json::Value obj;
	Json::Value root;
	for (string it : args) {
		obj.append(it);
	}
	root["value"] = obj;
	genv->CallStaticVoidMethod(config->CPP_lib, config->Schedule, (jlong)time, tools.str2jstring(tools.JsonToString(root).c_str()));
}

/*��ʱ����ִ��*/
class SchedulingEvent {
public:
	void init() {};
	/*�Զ���id��ʶ��*/
	vector<string> ids;
	SchedulingEvent(string str) {
		const auto rawJsonLength = static_cast<int>(str.length());
		JSONCPP_STRING err;
		Json::Value root;
		Json::CharReaderBuilder builder;
		const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
		if (!reader->parse(str.c_str(), str.c_str() + rawJsonLength, &root,
			&err)) {
			//error
			logger->Error("JSON reader error");
			APIException("JSON reader error").raise();
		}

		const Json::Value arrayNum = root["value"];
		for (int i = 0; i < arrayNum.size(); i++)
		{
			ids.push_back(arrayNum[i].asCString());
		}

	}
};

/*����������*/
class Event {
private:
	typedef std::function<void(GroupMessageEvent)> GME;
	typedef std::function<void(PrivateMessageEvent)> PME;
	typedef std::function<bool(GroupInviteEvent)> GI;
	typedef std::function<bool(NewFriendRequestEvent)> NFRE;
	typedef std::function<void(MemberJoinEvent)> MJ;
	typedef std::function<void(MemberLeaveEvent)> ML;
	typedef std::function<void(RecallEvent)> R;
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
	R RE = [](RecallEvent) -> void {};
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
	void broadcast(RecallEvent r) {
		this->RE(r);
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
	void registerEvent(R r) {
		this->RE = move(r);
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