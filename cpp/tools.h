#include "pch.h"

extern JavaVM* gvm;
extern int JNIVersion;
//JNIEnv和多线程管理
class threadManager {
private:
	struct t {
		JNIEnv* e;
		bool attach;
	};
	std::map <std::string, t> _threads;
	std::recursive_mutex mtx;
	void newEnv(char* threadName = NULL);
	bool included(std::string id);
public:
	// 获取线程id
	static std::string getThreadId(){
		std::ostringstream oss;
		oss << std::this_thread::get_id();
		return oss.str();
	}
	threadManager();
	// 设置env给当前线程
	void setEnv(JNIEnv* e);
	/*
	结束当前线程的env，也就是释放当前线程缓存的env
	不过继续调用getEnv()将再次获取，所以在线程任务最后调用本方法释放
	*/
	void detach();
	// 取env
	JNIEnv* getEnv();
};
extern threadManager* manager;

/*日志类声明*/
class Logger {
private:
	jclass CPP_lib = NULL;
	jmethodID log = NULL;
	void log0(std::string, JNIEnv*, int);
public:
	void init(JNIEnv* = manager->getEnv());
	/*发送普通日志*/
	void Info(std::string, JNIEnv* = manager->getEnv());
	/*发送警告*/
	void Warning(std::string, JNIEnv* = manager->getEnv());
	/*发送错误*/
	void Error(std::string, JNIEnv* = manager->getEnv());
	~Logger();
};

/*声明全局日志对象*/
extern Logger* logger;

/*配置类声明*/
class Config {
public:
	// kt中JNI接口类
	jclass CPP_lib = NULL;
	// 异常类
	jclass initexception = NULL;
	// 撤回信息
	jmethodID recallMsgM = NULL;
	// 发送信息
	jmethodID Send = NULL;
	// 查询信息接口
	jmethodID refreshInfo = NULL;
	// 上传图片
	jmethodID uploadImg = NULL;
	// 取好友列表
	jmethodID queryBotFriends = NULL;
	// 取群组列表
	jmethodID queryBotGroups= NULL;
	// 上传文件
	jmethodID uploadFile = NULL;
	// 查询文件信息
	jmethodID queryFile = NULL;
	// 查询图片下载地址
	jmethodID Query = NULL;
	// 禁言
	jmethodID Mute = NULL;
	// 查询权限
	jmethodID QueryP = NULL;
	// 踢出
	jmethodID KickM = NULL;
	// 取群主
	jmethodID getowner = NULL;
	// 全员禁言
	jmethodID muteAll = NULL;
	// 查询群成员列表
	jmethodID QueryML = NULL;
	/*定时任务*/
	jmethodID Schedule = NULL;
	// 构建转发信息
	jmethodID buildforward = NULL;
	// 好友申请事件
	jmethodID NFR = NULL;
	// 群聊邀请事件
	jmethodID GI = NULL;
	Config() {};
	void Init(JNIEnv* = manager->getEnv());
	~Config();
};

extern Config* config;

/*工具类声明*/
class Tools {
public:
	/*
	* 名称:jstring2str
	* 作用:jstring到string转换
	* 参数:env:JNIEnv 必须,jstr:jstring 转换的文本
	* 返回值:string
	* 来源:https://blog.csdn.net/chunleixiahe/article/details/51394116
	*/
	std::string jstring2str(jstring jstr, JNIEnv* = manager->getEnv());
	/*
	* 名称:str2jstring
	* 作用:string类型转jstring类型
	* 参数:JNIEnv,const char*(string)
	* 返回值:jstring
	* 来源:https://blog.csdn.net/chunleixiahe/article/details/51394116
	*/
	jstring str2jstring(const char* pat, JNIEnv* = manager->getEnv());
	/*
	* 名称:JLongToString
	* 作用:jlong类型转string类型
	* 参数:jlong qqid
	* 返回值:string
	*/
	std::string JLongToString(jlong qqid);
	/*
	* 替换全部
	* 来源:https://stackoverflow.com/a/24315631/14646226
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
	* JSON转换到String
	* 来源:https://www.cnblogs.com/zhangdongsheng/p/12731021.html
	*/
	std::string JsonToString(const Json::Value& root);
	// 从string格式化json
	Json::Value StringToJson(std::string);
	// vector格式化输出
	std::string VectorToString(std::vector<unsigned long long>);
	// 从string格式化到vector
	std::vector<unsigned long long> StringToVector(std::string);
};

/*静态声明工具对象*/
static Tools tools;

//总异常
class MiraiCPException :public std::exception {
public:
	virtual std::string what() { return "C++部分出现了个未捕获的异常"; };
	virtual void raise() {};
};
//初始化异常
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
	//返回错误信息
	std::string what()
	{
		return this->description;
	}
	void raise() {
		manager->getEnv()->ThrowNew(config->initexception, (this->description + " step:" + std::to_string(this->step)).c_str());
	}
};
//文件读取异常
class IOException :public MiraiCPException
{
public:
	IOException(std::string text)
	{
		this->description = "文件读取异常" + text;
	}
	//返回错误信息
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
//内部异常
class APIException :public MiraiCPException
{
public:
	APIException(std::string text)
	{
		this->description = "MiraiCP内部无法预料的错误:" + text;
	}
	//返回错误信息
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
//机器人操作异常
class BotException : public MiraiCPException {
public:
	/*
	*	1 - 机器人无权限执行
	*/
	int type = 0;
	BotException(int type)
	{
		this->type = type;
		switch (type)
		{
		case 1:
			this->description = "没有权限执行该操作";
			break;
		}
	}
	//返回错误信息
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
//禁言异常
class MuteException :public MiraiCPException
{
public:
	/*
	*	 禁言时间超出0s~30d
	*/
	MuteException()
	{
		this->description = "禁言时长不在0s~30d中间";
	}
	//返回错误信息
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
//获取群成员错误
class MemberException :public MiraiCPException
{
public:
	/*
	*   "1" - 找不到群
	*	"2" - 找不到群成员
	*/
	int type = 0;
	MemberException(int type)
	{
		this->type = type;
		switch (type)
		{
		case 1:
			this->description = "找不到群";
			break;
		case 2:
			this->description = "找不到群成员";
			break;
		}
	}
	//返回错误信息
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
//获取群成员错误
class FriendException :public MiraiCPException
{
public:
	/*
	*   找不到好友
	*/
	FriendException()
	{
		this->description = "找不到好友";
	}
	//返回错误信息
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
//获取群错误
class GroupException :public MiraiCPException
{
public:
	GroupException()
	{
		this->description = "C++:找不到群";
	}
	//返回错误信息
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
//撤回异常
class RecallException: public MiraiCPException{
public:
	RecallException() {
		this->description = "该消息已经被撤回";
	}
	std::string what() {
		return this->description;
	}
private:
	std::string description = "";
};
//上传异常
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

//消息源声明
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
	// 撤回该消息
	void recall();
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

/*小程序发送卡片*/
class LightApp: public MiraiCodeable {
public:
	std::string content = "";
	//使用纯文本构造，推荐使用其他结构体方法构造
	LightApp(std::string content) {
		this->content = content;
	}
	//样式1,适合文字展示，无大图，不能交互
	LightApp(LightAppStyle1 c) {
		this->content = "{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[应用]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":"
			"{\"appName\":\"" + c.appName + "\",\"appType\":4,\"appid\":1109659848,"
			"\"iconUrl\":\"" + c.icon + "\"},"
			"\"data\":[" + c.titles + "],"
			"\"title\":\"" + c.title + "\",\"button\":"
			"[" + c.buttons + "],"
			"\"emphasis_keyword\":\"\"}},\"text\":\"\",\"sourceAd\":\"\"}";
	}
	//样式2，有大图，不能交互
	LightApp(LightAppStyle2 c) {
		this->content = "{\"config\":"
			"{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"1.0.0.103\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1110081493\",\"preview\":\"" + c.preview + "\",\"shareTemplateData\":{},"
			"\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" + c.title2 + "\",\"title\":\"" + c.title + "\","
			"\"host\":{\"uin\":0,\"nick\":\"\"},"
			"\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" + c.icon + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
	}
	//样式3，有大图，可以在电脑qq显示，并在电脑上点击的链接会跳转
	LightApp(LightAppStyle3 c) {
		this->content = "{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\"" + c.preview + "\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" + c.title + "\",\"title\":\"" + c.description + "\","
			"\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" + c.icon + "\",\"qqdocurl\":\"" + c.url + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
	}
	std::string toMiraiCode() {
		return "[mirai:app:" + tools.replace(tools.replace(content, "[", "\\["), "]", "\\]") + "]";
	}
};

/*图像类声明*/
class Image: public MiraiCodeable {
private:
	jmethodID Query = NULL;
public:
	//图片id，样式:` {xxx}.xx `
	std::string id = "";
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
	Image(std::string);
	/*
	* 获取图片下载url
	*/
	std::string queryURL(JNIEnv* = manager->getEnv());

	/*取全部的图片id，详情见Image*/
	static std::vector<std::string> GetImgIdsFromMiraiCode(std::string);

	/*取图片Mirai码*/
	std::string toMiraiCode();
};

/*group, friend, member的父类*/
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
	//1-Friend好友, 2-group群组, 3-member群成员
	int type() {return this->_type;}
	//id，当type=1的时候是好友qqid，2时是群组id，3是群成员的qqid
	unsigned long long id() { return this->_id; }
	//当type=3时存在，为群成员说在群的群组id
	unsigned long long groupid() { return this->_groupid; }
	/*群名称，群成员群名片，或好友昵称*/
	std::string nickOrNameCard() { return this->_nickOrNameCard; };
	// 头像url地址
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
		if (root == NULL) { APIException("JSON格式化错误,位置:C-ContactFromString()").raise(); }
		return Contact(root["type"].asInt(), 
			root["id"].asLargestUInt(), 
			root["groupid"].asLargestUInt(), 
			root["nickornamecard"].asCString());
	}
};

// 群文件

struct dinfo {
	// 下载地址
	std::string url;
	// md5
	std::string md5;
	// sha1
	std::string sha1;
};
struct finfo {
	// 文件大小
	unsigned long long size;
	// 上传者id
	unsigned long long uploaderid;
	// 下载次数
	unsigned int downloadtime;
	// 上传时间
	unsigned long long uploadtime;
	// 上次更改时间
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
	// 文件唯一id
	std::string id() { return this->_id; }
	// 文件内部id
	unsigned int internalid() { return this->_internalid; }
	// 文件名
	std::string name() { return this->_name; }
	// 文件大小
	long long size() { return this->_size; }
	// 文件在群文件的路径
	std::string path() { return this->_path; }
	static RemoteFile buildFromString(std::string source);
	// 由完整信息构造
	RemoteFile(std::string i, unsigned int ii, std::string n, long long s, dinfo d, finfo f);
	// 仅在上传后有效, 即获取到internalid时(internalid != 0) 否则重新上传
	std::string toMiraiCode() {
		if (internalid() == 0) {
			// 重新上传
			throw RemoteFileException("toMiraiCode error: internalid错误，重新上传");
		}
		return "[mirai:file:" + id() + "," + std::to_string(internalid()) + "," + name() + "," + std::to_string(size()) + "]";
	}
};

// 较底层api
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

/*聊天记录里每个消息*/
class ForwardNode {
public:
	//发送者id
	unsigned long long id = 0;
	//发送者昵称
	std::string name = "";
	//发送信息
	std::string message = "";
	//发送时间
	int time = 0;
	/*
	聊天记录里的每条信息
		i - 发送者id
		n - 发送者昵称
		m - 发送的信息
		t - 发送时间，以时间戳记
	*/
	ForwardNode(unsigned long long i, std::string n, std::string m, int t) {
		this->id = i;
		this->name = n;
		this->message = m;
		this->time = t;
	}
	/*
	聊天记录里的每条信息
		c - 一个Contact对象
		m - 发送的信息
		t - 发送时间，以时间戳记
	*/
	ForwardNode(Contact* c, std::string message, int t) {
		this->id = c->id();
		this->name = c->nickOrNameCard();
		this->message = message;
		this->time = t;
	}
};

//聊天记录
class ForwardMessage {
private:
	Json::Value sendmsg;
public:
	/*
	构建一条聊天记录
	第一个参数是聊天记录发生的地方
	然后是每条信息
	例子:
			ForwardMessage(&e.group,
			{
				ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
				ForwardNode(1930893235, "Eritque arcus", "hahaha", -1)
			}).sendTo(&e.group);
	*/
	ForwardMessage(Contact* c, std::initializer_list<ForwardNode> nodes);
	//发送给群或好友或群成员
	void sendTo(Contact* c, JNIEnv* = manager->getEnv());
};
std::string BuildForwardMessage(Contact*, std::initializer_list<ForwardNode>);

// 当前bot账号信息
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
		check();
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

/*好友类声明*/
class Friend:public Contact{
public:
	Friend(unsigned long long, JNIEnv* =manager->getEnv());
	Friend() {};
	//初始化当前对象，可能抛出异常
	//昵称
	/*
	* 上传本地图片，务必要用绝对路径
	* 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到好友
	* 最大支持图片大小为30MB
	* 可能抛出invalid_argument异常代表路径无效
	*/
	Image uploadImg(std::string filename, JNIEnv* = manager->getEnv());
	/*发送信息*/
	//发送miraicode
	MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(std::string msg, JNIEnv* = manager->getEnv());
	//发送文本信息
	MessageSource SendMsg(std::string msg, JNIEnv* = manager->getEnv());
};

/*群成员类声明*/
class Member :public Contact {
private:
	jmethodID Mute_id = NULL;
	jmethodID Query_permission = NULL;
	jmethodID KickM = NULL;
public:
	// 权限等级. OWNER群主 为 2, ADMINISTRATOR管理员 为 1, MEMBER群成员 为 0
	unsigned int permission = 0;
	// qqid, groupid
	Member(unsigned long long qqid, unsigned long long groupid, JNIEnv* = manager->getEnv());
	/*
	* 上传本地图片，务必要用绝对路径
	* 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
	* 最大支持图片大小为30MB
	* 可能抛出invalid_argument异常代表路径无效
	*/
	Image uploadImg(std::string filename, JNIEnv* = manager->getEnv());
	Member() {};
	//获取权限，会在构造时调用，请使用permission缓存变量
	unsigned int getPermission(JNIEnv* = manager->getEnv());
	/*发送信息*/
	//发送miraicode
	 MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	 MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(std::string msg, JNIEnv* = manager->getEnv());
	//发送文本信息，不进行miraicode解析
	MessageSource SendMsg(std::string msg, JNIEnv* = manager->getEnv());
	/*禁言当前对象，单位是秒，最少0秒最大30天
	* 返回值对应报错
	*	"E1" - 找不到群
	*	"E2" - 找不到群成员
	*	"E3" - 机器人无权限禁言对方
	*	"E4" - 禁言时间超出0s~30d
	*	"Y" - 一切正常
	*/
	void Mute(int time, JNIEnv* = manager->getEnv());
	/*踢出这个群成员*/
	void Kick(std::string reason, JNIEnv* = manager->getEnv());
};

/*群聊类声明*/
class Group :public Contact {
public:
	/*取群成员列表-vector<long>*/
	std::vector<unsigned long long> getMemberList() {
		std::string re = tools.jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib,
			config->QueryML,
			(jlong)this->id()));
		if (re == "E1") {
			throw GroupException();
		}
		return tools.StringToVector(re);
	}
	/*以string格式取群成员列表
	格式：
		每个群成员id间用逗号分隔
	*/
	std::string MemberListToString() {
		return tools.VectorToString(getMemberList());
	};
	//取群主
	Member getOwner(JNIEnv* = manager->getEnv());
	//构建以群号构建群对象
	Group(unsigned long long, JNIEnv* = manager->getEnv());
	/*
	* 上传本地图片，务必要用绝对路径
	* 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
	* 最大支持图片大小为30MB
	* 可能抛出invalid_argument异常代表路径无效
	*/
	Image uploadImg(std::string filename, JNIEnv* = manager->getEnv());
	/*
	上传并发送
	path-群文件路径(带文件名),根目录为/
	filepath-本地文件路径
	如 group.uploadFIle("/test.txt", "D:\\xxxx.xxx")
	*/
	RemoteFile sendFile(std::string path, std::string filepath, JNIEnv* = manager->getEnv());
	/* 
	取群文件信息,会自动搜索子目录
	path-群文件路径(不带文件名)
	id-文件id,可空，空则为用路径查找(此时路径要带文件名)
	因为群文件允许重名文件存在的特性，该查找并不可靠，只能返回重名文件中的其中一个文件
	*/
	RemoteFile getFile(std::string path, std::string id = "", JNIEnv* = manager->getEnv());
	struct short_info {
		// 路径带文件名
		std::string path = "";
		// 唯一id
		std::string id = "";
	};
	/*
	获取path路径下全部文件信息
	返回值为一个vector容器, 每一项为short_info
	*/
	std::vector<short_info> getFileList(std::string path, JNIEnv* = manager->getEnv());
	// 取文件列表返回值是字符串
	std::string getFileListString(std::string path, JNIEnv* = manager->getEnv());
	Group() {};
	/*
	* 设置全员禁言
	* param: sign = true时为开始，false为关闭
	*/
	void setMuteAll(bool sign, JNIEnv* = manager->getEnv());
	/*发送信息*/
	MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(std::string msg, JNIEnv* = manager->getEnv());
	MessageSource SendMsg(std::string msg, JNIEnv* = manager->getEnv());
};

/*At一个群成员*/
 inline std::string At(Member a) {
	/*返回at这个人的miraicode*/
	return "[mirai:at:" + std::to_string(a.id()) + "]";
}
 /*At一个群成员*/
 inline std::string At(unsigned long long a) {
	/*返回at这个人的miraicode*/
	return "[mirai:at:" + std::to_string(a) + "]";
}

 class BotEvent {
 public:
	 Bot bot;
	 BotEvent(unsigned long long botid) {
		 bot = Bot(botid);
	 }
 };

/*群消息事件声明*/
class GroupMessageEvent:public BotEvent {
public:
	//来源群
	Group group;
	//发送人
	Member sender;
	//信息本体
	std::string message;
	//消息源
	MessageSource messageSource;
	GroupMessageEvent(Group g, Member f, std::string s, MessageSource s1, unsigned long long botid) :BotEvent(botid) {
		this->group = g;
		this->sender = f;
		this->message = s;
		this->messageSource = s1;
	}
};

/*私聊消息事件类声明*/
class PrivateMessageEvent : public BotEvent {
public:
	//发起人
	Friend sender;
	//附带消息
	std::string message;
	//信息源
	MessageSource messageSource;
	PrivateMessageEvent(Friend f, std::string s, MessageSource ms, unsigned long long botid) :BotEvent(botid) {
		this->sender = f;
		this->message = s;
		this->messageSource = ms;
	}
};

/*群聊邀请事件类声明*/
class GroupInviteEvent: public BotEvent {
private:
	std::string source;
public:
	//发起人
	std::string inviterNick = "";
	unsigned long long inviterid = 0;
	//被邀请进的组
	std::string groupName = "";
	unsigned long long groupid = 0;
	static void reject(std::string source) {
		std::string re = tools.jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->GI, tools.str2jstring(source.c_str()), (jboolean)false));
		if (re == "Y") return;
		if (re == "E")if (re == "E")logger->Error("群聊邀请事件同意失败,id:" + source);
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
		if (re == "E")logger->Error("群聊邀请事件同意失败,id:" + source);
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

/*好友申请事件声明*/
class NewFriendRequestEvent : public BotEvent {
private:
	std::string source;
public:
	//发起者
	unsigned long long senderid;
	//附加信息
	std::string message;
	static void reject(std::string source) {
		std::string re = tools.jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->NFR, tools.str2jstring(source.c_str()), (jboolean)false));
		if (re == "Y") return;
		if (re == "E")logger->Error("好友申请事件同意失败,id:" + source);
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
		if (re == "E")logger->Error("好友申请事件同意失败,id:" + source);
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

/*新群成员加入*/
class MemberJoinEvent : public BotEvent {
public:
	/*
	事件类型
	1 - 被邀请进来
	2 - 主动加入
	3 - 原群主通过 https://huifu.qq.com/ 恢复原来群主身份并入群
	*/
	int type = NULL;
	//新进入的成员
	Member member;
	//目标群
	Group group;
	//邀请人, 当type = 1时存在，否则则和member变量相同
	Member inviter;
	MemberJoinEvent(int t, Member m, Group g, Member i, unsigned long long botid) :BotEvent(botid) {
		this->type = t;
		this->member = m;
		this->group = g;
		this->inviter = i;
	}
};

/*群成员离开*/
class MemberLeaveEvent : public BotEvent {
public:
	/*
	事件类型
	1 - 被踢出
	2 - 主动退出
	*/
	int type = NULL;
	/*退出的成员的成员*/
	Member member;
	/*目标群*/
	Group group;
	/*操作人, 主动退出时与member相同，改成员可能是当前bot，operater以与系统operator区分*/
	Member operater;
	MemberLeaveEvent(int t, Member m, Group g, Member i, unsigned long long botid) :BotEvent(botid) {
		this->type = t;
		this->member = m;
		this->group = g;
		this->operater = i;
	}
};

/*撤回信息*/
class RecallEvent : public BotEvent {
public:
	/*
	为1时是好友撤回，为2时为群聊内撤回
	*/
	int type = 0;
	//时间戳
	int time = 0;
	//原发送者
	unsigned long authorid = 0;
	//撤回者
	unsigned long operatorid = 0;
	//信息id
	std::string ids = "";
	//内部ids
	std::string internalids = "";
	//当type是2的时候存在，否则为0
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

/*机器人进入某群*/
class BotJoinGroupEvent : public BotEvent {
public:
	//1-主动加入,2-被邀请加入,3-提供恢复群主身份加入
	int type;
	//进入的群
	Group group;
	//当type=2时存在，为邀请人，否则为NULL
	Member inviter = Member();
	BotJoinGroupEvent(int t, Group g, Member i, unsigned long long botid) :BotEvent(botid) {
		this->type = t;
		this->group = g;
		this->inviter = i;
	}
};

// 群临时会话
class GroupTempMessageEvent : public BotEvent {
public:
	//来源群
	Group group;
	//发送人
	Member sender;
	//信息本体
	std::string message;
	//消息源
	MessageSource messageSource;
	GroupTempMessageEvent(Group g, Member f, std::string s, MessageSource s1, unsigned long long botid) :BotEvent(botid) {
		this->group = g;
		this->sender = f;
		this->message = s;
		this->messageSource = s1;
	}
};

/*启动定时任务,time是多少毫秒后开始，id是自定义标识符*/
inline void SetScheduling(long time, std::initializer_list<std::string> args) {
	Json::Value obj;
	Json::Value root;
	for (std::string it : args) {
		obj.append(it);
	}
	root["value"] = obj;
	manager->getEnv()->CallStaticVoidMethod(config->CPP_lib, config->Schedule, (jlong)time, tools.str2jstring(tools.JsonToString(root).c_str()));
}

/*定时任务执行*/
class SchedulingEvent:BotEvent {
public:
	void init() {};
	/*自定义id标识符*/
	std::vector<std::string> ids;
	SchedulingEvent(std::string str, unsigned long long botid);
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
/*监听类声明*/
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

	/*
	* 广播函数重载
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
	* 监听函数重载
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

/*声明全局监听对象(广播源)*/
extern Event* procession;

/*声明插件启用和结束函数*/
void onDisable();
void onEnable();