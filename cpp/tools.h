#include "pch.h"

extern JavaVM* gvm;
extern int JNIVersion;
//JNIEnv�Ͷ��̹߳���
class threadManager {
private:
	struct t {
		JNIEnv* e;
		bool attach;
	};
	std::map <std::string, t> _threads;
	std::recursive_mutex mtx;
	void newEnv(char* threadName = NULL);
public:
	static std::string getThreadId(){
		std::ostringstream oss;
		oss << std::this_thread::get_id();
		return oss.str();
	}
	bool included(std::string id) {
		if (this->_threads.empty() || this->_threads.count(id) == 0)
			return false;
		return true;
	}
	threadManager();
	void setEnv(JNIEnv* e);
	void detach();
	JNIEnv* getEnv();
};
extern threadManager* manager;

/*��־������*/
class Logger {
private:
	jclass CPP_lib = NULL;
	jmethodID log = NULL;
	void log0(std::string, JNIEnv*, int);
public:
	void init(JNIEnv* = manager->getEnv());
	/*������ͨ��־*/
	void Info(std::string, JNIEnv* = manager->getEnv());
	/*���;���*/
	void Warning(std::string, JNIEnv* = manager->getEnv());
	/*���ʹ���*/
	void Error(std::string, JNIEnv* = manager->getEnv());
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
	jmethodID Send = NULL;
	jmethodID refreshInfo = NULL;
	jmethodID uploadImg = NULL;
	jmethodID queryBotFriends = NULL;
	jmethodID queryBotGroups= NULL;
	jmethodID uploadFile = NULL;
	jmethodID queryFile = NULL;
	/*ͼ����*/
	jmethodID Query = NULL;
	/*������*/
	/*Ⱥ�ĳ�Ա��*/
	jmethodID Mute = NULL;
	jmethodID QueryP = NULL;
	jmethodID KickM = NULL;
	/*Ⱥ����*/
	jmethodID getowner = NULL;
	jmethodID muteAll = NULL;
	jmethodID QueryN = NULL;
	jmethodID QueryML = NULL;
	/*��ʱ����*/
	jmethodID Schedule = NULL;
	jmethodID buildforward = NULL;
	jmethodID NFR = NULL;
	jmethodID GI = NULL;
	Config() {};
	void Init(JNIEnv* = manager->getEnv());
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
	std::string jstring2str(jstring jstr, JNIEnv* = manager->getEnv());
	/*
	* ����:str2jstring
	* ����:string����תjstring����
	* ����:JNIEnv,const char*(string)
	* ����ֵ:jstring
	* ��Դ:https://blog.csdn.net/chunleixiahe/article/details/51394116
	*/
	jstring str2jstring(const char* pat, JNIEnv* = manager->getEnv());
	/*
	* ����:JLongToString
	* ����:jlong����תstring����
	* ����:jlong qqid
	* ����ֵ:string
	*/
	std::string JLongToString(jlong qqid);
	/*
	* �滻ȫ��
	* ��Դ:https://stackoverflow.com/a/24315631/14646226
	*/
	std::string replace(std::string str, const std::string& from, const std::string& to) {
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Handles case where 'to' is a substd::string of 'from'
		}
		return str;
	}
	/*
	* JSONת����String
	* ��Դ:https://www.cnblogs.com/zhangdongsheng/p/12731021.html
	*/
	std::string JsonToString(const Json::Value& root);
	// ��string��ʽ��json
	Json::Value StringToJson(std::string);
	// vector��ʽ�����
	std::string VectorToString(std::vector<unsigned long long>);
	// ��string��ʽ����vector
	std::vector<unsigned long long> StringToVector(std::string);
};

/*��̬�������߶���*/
static Tools tools;

//���쳣
class MiraiCPException :public std::exception {
public:
	virtual std::string what() { return "C++���ֳ����˸�δ������쳣"; };
	virtual void raise() {};
};
//��ʼ���쳣
class InitException :public MiraiCPException
{
private:
	std::string description;
public:
	int step = 0;
	InitException(std::string text, int step)
	{
		this->description = text;
		this->step = step;
	}
	//���ش�����Ϣ
	std::string what()
	{
		return this->description;
	}
	void raise() {
		manager->getEnv()->ThrowNew(config->initexception, (this->description + " step:" + std::to_string(this->step)).c_str());
	}
};
//�ļ���ȡ�쳣
class IOException :public MiraiCPException
{
public:
	IOException(std::string text)
	{
		this->description = "�ļ���ȡ�쳣" + text;
	}
	//���ش�����Ϣ
	std::string what()
	{
		return this->description;
	}
	void raise() {
		logger->Error(this->description);
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	std::string description;
};
//�ڲ��쳣
class APIException :public MiraiCPException
{
public:
	APIException(std::string text)
	{
		this->description = "MiraiCP�ڲ��޷�Ԥ�ϵĴ���:" + text;
	}
	//���ش�����Ϣ
	std::string what()
	{
		return this->description;
	}
	void raise() {
		logger->Error(this->description);
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	std::string description;
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
	std::string what()
	{
		return this->description;
	}
	void raise() {
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	std::string description;
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
	std::string what()
	{
		return this->description;
	}
	void raise() {
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	std::string description;
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
	std::string what()
	{
		return this->description;
	}
	void raise() {
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	std::string description;
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
	std::string what()
	{
		return this->description;
	}
	void raise() {
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	std::string description;
};
//��ȡȺ����
class GroupException :public MiraiCPException
{
public:
	GroupException()
	{
		this->description = "C++:�Ҳ���Ⱥ";
	}
	//���ش�����Ϣ
	std::string what()
	{
		return this->description;
	}
	void raise() {
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}
private:
	std::string description;
};
//�����쳣
class RecallException: public MiraiCPException{
public:
	RecallException() {
		this->description = "����Ϣ�Ѿ�������";
	}
	std::string what() {
		return this->description;
	}
private:
	std::string description = "";
};
//�ϴ��쳣
class RemoteFileException : public MiraiCPException {
public:
	RemoteFileException(std::string e) {
		this->description = e;
	}
	std::string what() {
		return this->description;
	}
private:
	std::string description = "";
};

//��ϢԴ����
class MessageSource {
private:
	std::string source;
public:
	std::string ids = "";
	std::string internalids = "";
	std::string toString() {
		return source;
	}
	MessageSource() {};
	MessageSource(std::string t);
	void recall() {
		std::string re = tools.jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->recallMsgM,
			tools.str2jstring(this->toString().c_str())));
		if (re == "Y") return;
		if (re == "E1") throw BotException(1);
		if (re == "E2") throw RecallException();
	}
};

//MiraiCode
class MiraiCodeable {
public:
	virtual std::string toMiraiCode() = 0;
};
class MiraiCode {
private:
	std::string content = "";
public:
	std::string toString() {
		return content;
	}
	MiraiCode(MiraiCodeable* a) {
		content = a->toMiraiCode();
	}
	MiraiCode(std::string a) {
		content = a;
	}
	MiraiCode operator+(MiraiCodeable* a) {
		return MiraiCode(content + a->toMiraiCode());
	}
	MiraiCode operator+(std::string a) {
		return MiraiCode(content + a);
	}
	MiraiCode plus(MiraiCodeable* a) {
		return MiraiCode(content + a->toMiraiCode());
	}
	MiraiCode plus(std::string a) {
		return MiraiCode(content + a);
	}
};

/*С�����Ϳ�Ƭ*/
class LightApp: public MiraiCodeable {
public:
	std::string content = "";
	//ʹ�ô��ı����죬�Ƽ�ʹ�������ṹ�巽������
	LightApp(std::string content) {
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
	std::string toMiraiCode() {
		return "[mirai:app:" + tools.replace(tools.replace(content, "[", "\\["), "]", "\\]") + "]";
	}
};

/*ͼ��������*/
class Image: public MiraiCodeable {
private:
	jmethodID Query = NULL;
public:
	//ͼƬid����ʽ:` {xxx}.xx `
	std::string id = "";
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
	Image(std::string);
	/*
	* ��ȡͼƬ����url
	*/
	std::string queryURL(JNIEnv* = manager->getEnv());

	/*ȡȫ����ͼƬid�������Image*/
	static std::vector<std::string> GetImgIdsFromMiraiCode(std::string);

	/*ȡͼƬMirai��*/
	std::string toMiraiCode();
};

/*group, friend, member�ĸ���*/
class Contact {
protected:
	int _type;
	unsigned long long _id;
	unsigned long long _groupid;
	std::string _nickOrNameCard;
	std::string _avatarUrl;
public:
	Contact() {
		_type = 0;
		_id = 0;
		_groupid = 0;
		_nickOrNameCard = "";
	}
	Contact(int type, unsigned long long id, unsigned long long gid, std::string name) {
		this->_type = type;
		this->_id = id;
		this->_groupid = gid;
		this->_nickOrNameCard = name;
	};
	//1-Friend����, 2-groupȺ��, 3-memberȺ��Ա
	int type() {return this->_type;}
	//id����type=1��ʱ���Ǻ���qqid��2ʱ��Ⱥ��id��3��Ⱥ��Ա��qqid
	unsigned long long id() { return this->_id; }
	//��type=3ʱ���ڣ�ΪȺ��Ա˵��Ⱥ��Ⱥ��id
	unsigned long long groupid() { return this->_groupid; }
	/*Ⱥ���ƣ�Ⱥ��ԱȺ��Ƭ��������ǳ�*/
	std::string nickOrNameCard() { return this->_nickOrNameCard; };
	// ͷ��url��ַ
	std::string avatarUrl() { return this->_avatarUrl; };
	Json::Value toJsonValue() {
		Json::Value root;
		root["type"] = type();
		root["id"] = id();
		root["groupid"] = groupid();
		root["nickornamecard"] = nickOrNameCard();
		return root;
	}
	std::string toString() {
		return tools.JsonToString(this->toJsonValue());
	}
	static Contact fromString(std::string source) {
		Json::Value root = tools.StringToJson(source);
		if (root == NULL) { APIException("JSON��ʽ������,λ��:C-ContactFromString()").raise(); }
		return Contact(root["type"].asInt(), 
			root["id"].asLargestUInt(), 
			root["groupid"].asLargestUInt(), 
			root["nickornamecard"].asCString());
	}
};

// Ⱥ�ļ�

struct dinfo {
	// ���ص�ַ
	std::string url;
	// md5
	std::string md5;
	// sha1
	std::string sha1;
};
struct finfo {
	// �ļ���С
	unsigned long long size;
	// �ϴ���id
	unsigned long long uploaderid;
	// ���ش���
	unsigned int downloadtime;
	// �ϴ�ʱ��
	unsigned long long uploadtime;
	// �ϴθ���ʱ��
	unsigned long long lastmodifytime;
};
class RemoteFile : public MiraiCodeable {
private:
	std::string _id;
	unsigned int _internalid;
	std::string _name;
	long long _size;
	std::string _path;
	dinfo _dinfo;
	finfo _finfo;
public:
	// �ļ�Ψһid
	std::string id() { return this->_id; }
	// �ļ��ڲ�id
	unsigned int internalid() { return this->_internalid; }
	// �ļ���
	std::string name() { return this->_name; }
	// �ļ���С
	long long size() { return this->_size; }
	// �ļ���Ⱥ�ļ���·��
	std::string path() { return this->_path; }
	static RemoteFile buildFromString(std::string source) {
		Json::Value tmp = tools.StringToJson(source);
		Json::Value droot = tmp["dinfo"];
		dinfo d;
		d.md5 = droot["md5"].asCString();
		d.sha1 = droot["sha1"].asCString();
		d.url = droot["url"].asCString();
		Json::Value froot = tmp["finfo"];
		finfo f;
		f.downloadtime = froot["downloadtime"].asInt();
		f.lastmodifytime = froot["lastmodifytime"].asLargestUInt();
		f.size = froot["size"].asLargestUInt();
		f.uploaderid = froot["uploaderid"].asLargestUInt();
		f.uploadtime = froot["uploadtime"].asLargestUInt();
		return RemoteFile(tmp["id"].asCString(), tmp["internalid"].asLargestUInt(), tmp["name"].asCString(), tmp["size"].asLargestUInt(), d, f);
	}
	// ��������Ϣ����
	RemoteFile(std::string i, unsigned int ii, std::string n, long long s, dinfo d, finfo f) {
		this->_id = i;
		this->_internalid = ii;
		this->_name = n;
		this->_size = s;
		this->_dinfo = d;
		this->_finfo = f;
	}
	// ��·����Ⱥ����
	RemoteFile(unsigned long long groupid, std::string path) {}
	// �����ϴ�����Ч, ����ȡ��internalidʱ(internalid != 0) ���������ϴ�
	std::string toMiraiCode() {
		if (internalid() == 0) {
			// �����ϴ�
			throw RemoteFileException("toMiraiCode error: internalid���������ϴ�");
		}
		return "[mirai:file:" + id() + "," + std::to_string(internalid()) + "," + name() + "," + std::to_string(size()) + "]";
	}
};

// �ϵײ�api
class LowLevelAPI {
public:
	static std::string send0(std::string content, Contact* c, bool miraicode, JNIEnv* env) {
		Json::Value root;
		root["content"] = content;
		root["contact"] = c->toJsonValue();
		return tools.jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->Send, tools.str2jstring(tools.JsonToString(root).c_str(),env), (jboolean)miraicode), env);
	}
	static std::string getInfoSource(Contact* c, JNIEnv* env = manager->getEnv()) {
		return tools.jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->refreshInfo, tools.str2jstring(c->toString().c_str(), env)));
	}
	static std::string uploadImg0(std::string path, Contact* c, JNIEnv* env = manager->getEnv()) {
		return tools.jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->uploadImg,tools.str2jstring(path.c_str(), env), tools.str2jstring(c->toString().c_str(), env)));
	}
	static struct info {
		std::string nickornamecard;
		std::string avatarUrl;
	};
	static info info0(std::string source) {
		info re;
		Json::Value tmp = tools.StringToJson(source);
		re.avatarUrl = tmp["avatarUrl"].asCString();
		re.nickornamecard = tmp["nickornamecard"].asCString();
		return re;
	}
};

/*�����¼��ÿ����Ϣ*/
class ForwardNode {
public:
	//������id
	unsigned long long id = 0;
	//�������ǳ�
	std::string name = "";
	//������Ϣ
	std::string message = "";
	//����ʱ��
	int time = 0;
	/*
	�����¼���ÿ����Ϣ
		i - ������id
		n - �������ǳ�
		m - ���͵���Ϣ
		t - ����ʱ�䣬��ʱ�����
	*/
	ForwardNode(unsigned long long i, std::string n, std::string m, int t) {
		this->id = i;
		this->name = n;
		this->message = m;
		this->time = t;
	}
	/*
	�����¼���ÿ����Ϣ
		c - һ��Contact����
		m - ���͵���Ϣ
		t - ����ʱ�䣬��ʱ�����
	*/
	ForwardNode(Contact* c, std::string message, int t) {
		this->id = c->id();
		this->name = c->nickOrNameCard();
		this->message = message;
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
	��һ�������������¼�����ĵط�
	Ȼ����ÿ����Ϣ
	����:
			ForwardMessage(&e.group,
			{
				ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
				ForwardNode(1930893235, "Eritque arcus", "hahaha", -1)
			}).sendTo(&e.group);
	*/
	ForwardMessage(Contact* c, std::initializer_list<ForwardNode> nodes) {
		Json::Value root;
		Json::Value value;
		root["type"] = c->type();
		root["id"] = c->id();
		root["id2"] = c->groupid();
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
	//���͸�Ⱥ����ѻ�Ⱥ��Ա
	void sendTo(Contact* c, JNIEnv* = manager->getEnv());
};
std::string BuildForwardMessage(Contact*, std::initializer_list<ForwardNode>);

class Bot {
private:
	bool inited = false;
	unsigned long long _id;
	std::string _nick;
	std::string _avatarUrl;
	void check() {
		if (!this->inited) { 
			refreshInfo(); 
			this->inited = true;
		}
	}
public:
	void refreshInfo(JNIEnv* env = manager->getEnv()) {
		LowLevelAPI::info tmp = LowLevelAPI::info0(tools.jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->refreshInfo, tools.str2jstring(Contact(4, this->_id, 0, "").toString().c_str(), env))));
		this->_avatarUrl = tmp.avatarUrl;
		this->_nick = tmp.nickornamecard;
	}
	Bot(unsigned long long i) {
		this->_id = i;
	}
	Bot() {}
	unsigned long long id() {
		return this->_id;
	}
	std::string nick() {
		check();
		return this->_nick;
	}
	std::string avatarUrl() {
		return this->_avatarUrl;
	}
	std::vector<unsigned long long> getFriendList(JNIEnv* env = manager->getEnv()) {
		std::string temp = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib,
			config->queryBotFriends,
			(jlong)this->id()));
		return tools.StringToVector(temp);
	}
	std::string FriendListToString() {
		return tools.VectorToString(getFriendList());
	}
	std::vector<unsigned long long> getGroupList(JNIEnv* env = manager->getEnv()) {
		std::string temp = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib,
			config->queryBotGroups,
			(jlong)this->id()));
		return tools.StringToVector(temp);
	}
	std::string GroupListToString() {
		return tools.VectorToString(getGroupList());
	}
};

/*����������*/
class Friend:public Contact{
public:
	Friend(unsigned long long, JNIEnv* =manager->getEnv());
	Friend() {};
	//��ʼ����ǰ���󣬿����׳��쳣
	//�ǳ�
	/*
	* �ϴ�����ͼƬ�����Ҫ�þ���·��
	* ����miraiҪ����ͼƬ���Ͷ�������ʹ�ñ������ϴ���ͼƬֻ�ܷ�������
	* ���֧��ͼƬ��СΪ30MB
	* �����׳�invalid_argument�쳣����·����Ч
	*/
	Image uploadImg(std::string filename, JNIEnv* = manager->getEnv());
	/*������Ϣ*/
	//����miraicode
	 MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	 MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(std::string msg, JNIEnv* = manager->getEnv());
	//�����ı���Ϣ
	MessageSource SendMsg(std::string msg, JNIEnv* = manager->getEnv());
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
	Member(unsigned long long qqid, unsigned long long groupid, JNIEnv* = manager->getEnv());
	/*
	* �ϴ�����ͼƬ�����Ҫ�þ���·��
	* ����miraiҪ����ͼƬ���Ͷ�������ʹ�ñ������ϴ���ͼƬֻ�ܷ���Ⱥ
	* ���֧��ͼƬ��СΪ30MB
	* �����׳�invalid_argument�쳣����·����Ч
	*/
	Image uploadImg(std::string filename, JNIEnv* = manager->getEnv());
	Member() {};
	//��ȡȨ�ޣ����ڹ���ʱ���ã���ʹ��permission�������
	unsigned int getPermission(JNIEnv* = manager->getEnv());
	/*������Ϣ*/
	//����miraicode
	 MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	 MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(std::string msg, JNIEnv* = manager->getEnv());
	//�����ı���Ϣ��������miraicode����
	MessageSource SendMsg(std::string msg, JNIEnv* = manager->getEnv());
	/*���Ե�ǰ���󣬵�λ���룬����0�����30��
	* ����ֵ��Ӧ����
	*	"E1" - �Ҳ���Ⱥ
	*	"E2" - �Ҳ���Ⱥ��Ա
	*	"E3" - ��������Ȩ�޽��ԶԷ�
	*	"E4" - ����ʱ�䳬��0s~30d
	*	"Y" - һ������
	*/
	void Mute(int time, JNIEnv* = manager->getEnv());
	/*�߳����Ⱥ��Ա*/
	void Kick(std::string reason, JNIEnv* = manager->getEnv());
};

/*Ⱥ��������*/
class Group :public Contact {
public:
	/*ȡȺ��Ա�б�-vector<long>*/
	std::vector<unsigned long long> getMemberList() {
		std::string re = tools.jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib,
			config->QueryML,
			(jlong)this->id()));
		if (re == "E1") {
			throw GroupException();
		}
		return tools.StringToVector(re);
	}
	/*��string��ʽȡȺ��Ա�б�
	��ʽ��
		ÿ��Ⱥ��Աid���ö��ŷָ�
	*/
	std::string MemberListToString() {
		return tools.VectorToString(getMemberList());
	};
	//ȡȺ��
	Member getOwner(JNIEnv* = manager->getEnv());
	//������Ⱥ�Ź���Ⱥ����
	Group(unsigned long long, JNIEnv* = manager->getEnv());
	/*
	* �ϴ�����ͼƬ�����Ҫ�þ���·��
	* ����miraiҪ����ͼƬ���Ͷ�������ʹ�ñ������ϴ���ͼƬֻ�ܷ���Ⱥ
	* ���֧��ͼƬ��СΪ30MB
	* �����׳�invalid_argument�쳣����·����Ч
	*/
	Image uploadImg(std::string filename, JNIEnv* = manager->getEnv());
	// �ϴ�����Ⱥ�ļ�·��(���ļ���)��Ŀ¼Ϊ/, �ļ���, �� group.uploadFIle("/test.txt", "D:\\xxxx")
	RemoteFile uploadFile(std::string path, std::string filename, JNIEnv* = manager->getEnv());
	// Ⱥ�ļ�·��(�����ļ���), �ļ�id
	RemoteFile getFile(std::string path, std::string id, JNIEnv* = manager->getEnv());
	Group() {};
	/*
	* ����ȫԱ����
	* param: sign = trueʱΪ��ʼ��falseΪ�ر�
	*/
	void setMuteAll(bool sign, JNIEnv* = manager->getEnv());
	/*������Ϣ*/
	MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(std::string msg, JNIEnv* = manager->getEnv());
	MessageSource SendMsg(std::string msg, JNIEnv* = manager->getEnv());
};

/*Atһ��Ⱥ��Ա*/
 inline std::string At(Member a) {
	/*����at����˵�miraicode*/
	return "[mirai:at:" + std::to_string(a.id()) + "]";
}
 /*Atһ��Ⱥ��Ա*/
 inline std::string At(unsigned long long a) {
	/*����at����˵�miraicode*/
	return "[mirai:at:" + std::to_string(a) + "]";
}

 class BotEvent {
 public:
	 Bot bot;
	 BotEvent(unsigned long long botid) {
		 bot = Bot(botid);
	 }
 };

/*Ⱥ��Ϣ�¼�����*/
class GroupMessageEvent:public BotEvent {
public:
	//��ԴȺ
	Group group;
	//������
	Member sender;
	//��Ϣ����
	std::string message;
	//��ϢԴ
	MessageSource messageSource;
	GroupMessageEvent(Group g, Member f, std::string s, MessageSource s1, unsigned long long botid) :BotEvent(botid) {
		this->group = g;
		this->sender = f;
		this->message = s;
		this->messageSource = s1;
	}
};

/*˽����Ϣ�¼�������*/
class PrivateMessageEvent : public BotEvent {
public:
	//������
	Friend sender;
	//������Ϣ
	std::string message;
	//��ϢԴ
	MessageSource messageSource;
	PrivateMessageEvent(Friend f, std::string s, MessageSource ms, unsigned long long botid) :BotEvent(botid) {
		this->sender = f;
		this->message = s;
		this->messageSource = ms;
	}
};

/*Ⱥ�������¼�������*/
class GroupInviteEvent: public BotEvent {
private:
	std::string source;
public:
	//������
	std::string inviterNick = "";
	unsigned long long inviterid = 0;
	//�����������
	std::string groupName = "";
	unsigned long long groupid = 0;
	static void reject(std::string source) {
		std::string re = tools.jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->GI, tools.str2jstring(source.c_str()), (jboolean)false));
		if (re == "Y") return;
		if (re == "E")if (re == "E")logger->Error("Ⱥ�������¼�ͬ��ʧ��,id:" + source);
	}
	void reject() {
		this->reject(this->source);
	}
	std::string getSource() {
		return this->source;
	}
	static void accept(std::string source) {
		std::string re = tools.jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->GI, tools.str2jstring(source.c_str()), (jboolean)true));
		if (re == "Y") return;
		if (re == "E")logger->Error("Ⱥ�������¼�ͬ��ʧ��,id:" + source);
	}
	void accept() {
		this->accept(this->source);
	}
	GroupInviteEvent(unsigned long long ii, std::string inick, unsigned long long gi, std::string gn, std::string s, unsigned long long botid):BotEvent(botid) {
		this->inviterNick = inick;
		this->groupid = gi;
		this->inviterid = ii;
		this->groupName = gn;
		this->source = s;
	}
};

/*���������¼�����*/
class NewFriendRequestEvent : public BotEvent {
private:
	std::string source;
public:
	//������
	unsigned long long senderid;
	//������Ϣ
	std::string message;
	static void reject(std::string source) {
		std::string re = tools.jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->NFR, tools.str2jstring(source.c_str()), (jboolean)false));
		if (re == "Y") return;
		if (re == "E")logger->Error("���������¼�ͬ��ʧ��,id:" + source);
	}
	void reject() {
		this->reject(this->source);
	}
	std::string getSource() {
		return this->source;
	}
	static void accept(std::string source) {
		std::string re = tools.jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->NFR, tools.str2jstring(source.c_str()), (jboolean)true));
		if (re == "Y") return;
		if (re == "E")logger->Error("���������¼�ͬ��ʧ��,id:" + source);
	}
	void accept() {
		this->accept(this->source);
	}
	NewFriendRequestEvent(unsigned long long i, std::string m, std::string s, unsigned long long botid) :BotEvent(botid) {
		this->senderid = i;
		this->message = m;
		this->source = s;
	}
};

/*��Ⱥ��Ա����*/
class MemberJoinEvent : public BotEvent {
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
	Member inviter;
	MemberJoinEvent(int t, Member m, Group g, Member i, unsigned long long botid) :BotEvent(botid) {
		this->type = t;
		this->member = m;
		this->group = g;
		this->inviter = i;
	}
};

/*Ⱥ��Ա�뿪*/
class MemberLeaveEvent : public BotEvent {
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
	MemberLeaveEvent(int t, Member m, Group g, Member i, unsigned long long botid) :BotEvent(botid) {
		this->type = t;
		this->member = m;
		this->group = g;
		this->operater = i;
	}
};

/*������Ϣ*/
class RecallEvent : public BotEvent {
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
	std::string ids = "";
	//�ڲ�ids
	std::string internalids = "";
	//��type��2��ʱ����ڣ�����Ϊ0
	unsigned long groupid = 0;
	RecallEvent(int t, int t2, unsigned long a, unsigned long o, std::string id, std::string ii, unsigned long long botid, unsigned long g = 0) :BotEvent(botid) {
		this->type = t;
		this->time = t2;
		this->authorid = a;
		this->operatorid = o;
		this->ids = id;
		this->internalids = ii;
		this->groupid = g;
	}
};

/*�����˽���ĳȺ*/
class BotJoinGroupEvent : public BotEvent {
public:
	//1-��������,2-���������,3-�ṩ�ָ�Ⱥ����ݼ���
	int type;
	//�����Ⱥ
	Group group;
	//��type=2ʱ���ڣ�Ϊ�����ˣ�����ΪNULL
	Member inviter = Member();
	BotJoinGroupEvent(int t, Group g, Member i, unsigned long long botid) :BotEvent(botid) {
		this->type = t;
		this->group = g;
		this->inviter = i;
	}
};

// Ⱥ��ʱ�Ự
class GroupTempMessageEvent : public BotEvent {
public:
	//��ԴȺ
	Group group;
	//������
	Member sender;
	//��Ϣ����
	std::string message;
	//��ϢԴ
	MessageSource messageSource;
	GroupTempMessageEvent(Group g, Member f, std::string s, MessageSource s1, unsigned long long botid) :BotEvent(botid) {
		this->group = g;
		this->sender = f;
		this->message = s;
		this->messageSource = s1;
	}
};

/*������ʱ����,time�Ƕ��ٺ����ʼ��id���Զ����ʶ��*/
inline void SetScheduling(long time, std::initializer_list<std::string> args) {
	Json::Value obj;
	Json::Value root;
	for (std::string it : args) {
		obj.append(it);
	}
	root["value"] = obj;
	manager->getEnv()->CallStaticVoidMethod(config->CPP_lib, config->Schedule, (jlong)time, tools.str2jstring(tools.JsonToString(root).c_str()));
}

/*��ʱ����ִ��*/
class SchedulingEvent:BotEvent {
public:
	void init() {};
	/*�Զ���id��ʶ��*/
	std::vector<std::string> ids;
	SchedulingEvent(std::string str, unsigned long long botid):BotEvent(botid) {
		const auto rawJsonLength = static_cast<int>(str.length());
		Json::String err;
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
using GME = std::function<void(GroupMessageEvent)>;
using PME = std::function<void(PrivateMessageEvent)>;
using GI = std::function<void(GroupInviteEvent)>;
using NFRE = std::function<void(NewFriendRequestEvent)>;
using MJ = std::function<void(MemberJoinEvent)>;
using ML = std::function<void(MemberLeaveEvent)>;
using R = std::function<void(RecallEvent)>;
using S = std::function<void(SchedulingEvent)>;
using BJ = std::function<void(BotJoinGroupEvent)>;
using GTME = std::function<void(GroupTempMessageEvent)>;
/*����������*/
class Event {
private:
	class Node {
	public:
		Node* nextNode = nullptr;
		Node() {};
	};
	class GMENode:public Node
	{
	public: 
		bool enable = true;
		GME f = [](GroupMessageEvent)->void {};
		GMENode* next = nullptr;
	};
	class PMENode:public Node
	{
	public:
		bool enable = true;
		PME f = [](PrivateMessageEvent)->void {};
		PMENode* next = nullptr;
	};
	class GINode :public Node
	{
	public:
		bool enable = true;
		GI f = [](GroupInviteEvent) {};
		GINode* next = nullptr;
	};
	class NFRENode :public Node
	{
	public:
		
		bool enable = true;
		NFRE f = [](NewFriendRequestEvent) {};
		NFRENode* next = nullptr;
	};
	class MJNode :public Node
	{
	public:
		
		bool enable = true;
		MJ f = [](MemberJoinEvent)->void {};
		MJNode* next = nullptr;
	};
	class MLNode :public Node
	{
	public:
		
		bool enable = true;
		ML f = [](MemberLeaveEvent)->void {};
		MLNode* next = nullptr;
	};
	class RNode :public Node
	{
	public:
		
		bool enable = true;
		R f = [](RecallEvent)->void {};
		RNode* next = nullptr;
	};
	class SNode :public Node
	{
	public:
		
		bool enable = true;
		S f = [](SchedulingEvent)->void {};
		SNode* next = nullptr;
	};
	class BJNode :public Node
	{
	public:
		
		bool enable = true;
		BJ f = [](BotJoinGroupEvent)->void {};
		BJNode* next = nullptr;
	};
	class GTMENode :public Node
	{
	public:

		bool enable = true;
		GTME f = [](GroupTempMessageEvent)->void {};
		GTMENode* next = nullptr;
	};
	
	GMENode* GMHead = new GMENode();
	PMENode* PMHead = new PMENode();
	GINode* GHead = new GINode();
	NFRENode* NFHead = new NFRENode();
	MJNode* MJHead = new MJNode();
	MLNode* MLHead = new MLNode();
	RNode* RHead = new RNode();
	SNode* SHead = new SNode();
	BJNode* BHead = new BJNode();
	GTMENode* GTMHead = new GTMENode();

	GMENode* GMTail = GMHead;
	PMENode* PMTail = PMHead;
	GINode* GTail = GHead;
	NFRENode* NFTail = NFHead;
	MJNode* MJTail = MJHead;
	MLNode* MLTail = MLHead;
	RNode* RTail = RHead;
	SNode* STail = SHead;
	BJNode* BTail = BHead;
	GTMENode* GTMTail = GTMHead;
	
public:
	class NodeHandle {
	private:
		bool* enable;
	public:
		NodeHandle(bool* a) {
			this->enable = a;
		}
		void stop() const{
			*enable = false;
		}
		void consume() const {
			*enable = true;
		}
	};
	class SuperNodeHandle {
	public:
		PMENode* enable;
		SuperNodeHandle(PMENode* a) {
			this->enable = a;
		}
	};

	/*
	* �㲥��������
	*/

	void broadcast(GroupMessageEvent g) {
		GMENode *now = GMHead;
		while (now) {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}
	void broadcast(PrivateMessageEvent p) {
		PMENode* now = PMHead;
		while (now) {
			if (now->enable) {
				now->f(p);
			}
			now = now->next;
		}
	}
	void broadcast(GroupInviteEvent g) {
		GINode* now = GHead;
		while (now)  {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}
	void broadcast(NewFriendRequestEvent g) {
		NFRENode* now = NFHead;
		while (now) {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}
	void broadcast(MemberJoinEvent g) {
		MJNode* now = MJHead;
		while (now) {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}
	void broadcast(MemberLeaveEvent g) {
		MLNode* now = MLHead;
		while (now) {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}
	void broadcast(RecallEvent r) {
		RNode* now = RHead;
		while (now) {
			if (now->enable) { now->f(r); }
			now = now->next;
		}
	}
	void broadcast(SchedulingEvent g) {
		SNode* now = SHead;
		while (now) {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}
	void broadcast(BotJoinGroupEvent b) {
		BJNode* now = BHead;
		while (now) {
			if (now->enable) { now->f(b); }
			now = now->next;
		}
	}
	void broadcast(GroupTempMessageEvent g) {
		GTMENode* now = GTMHead;
		while (now) {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}

	/*
	* ������������
	*/

	NodeHandle registerEvent(GME f) {
		GMENode* node = new GMENode();
		node->f = f;
		GMTail->next = node;
		GMTail->nextNode = node;
		GMTail = node;
		return NodeHandle(&node->enable);
	}
	NodeHandle registerEvent(PME f) {
		PMENode* node = new PMENode();
		node->f = f;
		PMTail->next = node;
		PMTail->nextNode = node;
		PMTail = node;
		return NodeHandle(&node->enable);
	}
	NodeHandle registerEvent(GI f) {
		GINode* node = new GINode();
		node->f = f;
		GTail->next = node;
		GTail->nextNode = node;
		GTail = node;
		return NodeHandle(&node->enable);
	}
	NodeHandle registerEvent(NFRE f) {
		NFRENode* node = new NFRENode();
		node->f = f;
		NFTail->next = node;
		NFTail->nextNode = node;
		NFTail = node;
		return NodeHandle(&node->enable);
	}
	NodeHandle registerEvent(MJ f) {
		MJNode* node = new MJNode();
		node->f = f;
		MJTail->next = node;
		MJTail->nextNode = node;
		MJTail = node;
		return NodeHandle(&node->enable);
	}
	NodeHandle registerEvent(ML f) {
		MLNode* node = new MLNode();
		node->f = f;
		MLTail->next = node;
		MLTail->nextNode = node;
		MLTail = node;
		return NodeHandle(&node->enable);
	}
	NodeHandle registerEvent(R r) {
		RNode* node = new RNode();
		node->f = r;
		RTail->next = node;
		RTail->nextNode = node;
		RTail = node;
		return NodeHandle(&node->enable);
	}
	NodeHandle registerEvent(S f) {
		SNode* node = new SNode();
		node->f = f;
		STail->next = node;
		STail->nextNode = node;
		STail = node;
		return NodeHandle(&node->enable);
	}
	NodeHandle registerEvent(BJ f) {
		BJNode* node = new BJNode();
		node->f = f;
		BTail->next = node;
		BTail->nextNode = node;
		BTail = node;
		return NodeHandle(&node->enable);
	}
	NodeHandle registerEvent(GTME f) {
		GTMENode* node = new GTMENode();
		node->f = f;
		GTMTail->next = node;
		GTMTail->nextNode = node;
		GTMTail = node;
		return NodeHandle(&node->enable);
	}

	~Event();
};

/*����ȫ�ּ�������(�㲥Դ)*/
extern Event* procession;

/*����������úͽ�������*/
void onDisable();
void onEnable();