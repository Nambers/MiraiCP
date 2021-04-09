#include "pch.h"

extern JavaVM* gvm;
extern int JNIVersion;
//JNIEnv和多线程管理
class threadManager {
private:
	std::map <std::string, JNIEnv*> _threads;
	std::mutex mtx;
public:
	static std::string getThreadId() {
		std::ostringstream oss;
		oss << std::this_thread::get_id();
		return oss.str();
	}
	threadManager() {
		this->_threads = std::map <std::string, JNIEnv*>();
	}
	void setEnv(JNIEnv* e)noexcept {
		mtx.lock();
		this->_threads.insert(std::pair<std::string, JNIEnv*>(this->getThreadId(), e));
		mtx.unlock();
	}
	void start(char* threadName = NULL) noexcept {
		JNIEnv* env = nullptr;
		JavaVMAttachArgs args{
			JNIVersion,
			threadName,
			NULL
		};
		gvm->AttachCurrentThread((void**)&env, &args);
		mtx.lock();
		this->_threads.insert(std::pair<std::string, JNIEnv*>(this->getThreadId() , env));
		mtx.unlock();
	};
	void detach() noexcept {
		mtx.lock();
		this->_threads.erase(this->getThreadId());
		mtx.unlock();
		gvm->DetachCurrentThread();
	}
	JNIEnv* getEnv()noexcept {
		return this->_threads[this->getThreadId()];
	}
};
extern threadManager* manager;

/*日志类声明*/
class Logger {
private:
	jclass CPP_lib = NULL;
	jmethodID sinfo = NULL;
	jmethodID swarning = NULL;
	jmethodID serror = NULL;
public:
	void init(JNIEnv* = manager->getEnv());
	/*发送普通日志*/
	void Info(std::string log, JNIEnv* = manager->getEnv());
	/*发送警告*/
	void Warning(std::string log, JNIEnv* = manager->getEnv());
	/*发送错误*/
	void Error(std::string log, JNIEnv* = manager->getEnv());
	~Logger();
};

/*声明全局日志对象*/
extern Logger* logger;

/*配置类声明*/
class Config {
public:
	jclass CPP_lib = NULL;
	jclass initexception = NULL;
	jmethodID recallMsgM = NULL;
	/*图像类*/
	jmethodID Query = NULL;
	/*好友类*/
	jmethodID SendMsg2F = NULL;
	jmethodID SendMsg2FM = NULL;
	jmethodID NickorNameF = NULL;
	jmethodID uploadImgF = NULL;
	/*群聊成员类*/
	jmethodID SendMsg2M = NULL;
	jmethodID SendMsg2MM = NULL;
	jmethodID NickorNameM = NULL;
	jmethodID Mute = NULL;
	jmethodID QueryP = NULL;
	jmethodID uploadImgM = NULL;
	jmethodID KickM = NULL;
	/*群聊类*/
	jmethodID getowner = NULL;
	jmethodID muteAll = NULL;
	jmethodID SendMsg2G = NULL;
	jmethodID uploadImgG = NULL;
	jmethodID SendMsg2GM = NULL;
	jmethodID QueryN = NULL;
	jmethodID QueryML = NULL;
	/*定时任务*/
	jmethodID Schedule = NULL;
	jmethodID buildforward = NULL;
	jmethodID NFR = NULL;
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
public:
	//1-Friend好友, 2-group群组, 3-member群成员
	int type = 0;
	//id，当type=1的时候是好友qqid，2时是群组id，3是群成员的qqid
	unsigned long long id = 0;
	//当type=3时存在，为群成员说在群的群组id
	unsigned long long groupid = 0;
	/*群名称，群成员群名片，或好友昵称*/
	std::string nickOrNameCard = "";
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
		this->id = c->id;
		this->name = c->nickOrNameCard;
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
	ForwardMessage(Contact* c, std::initializer_list<ForwardNode> nodes) {
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
	//发送给群或好友或群成员
	void sendTo(Contact* c, JNIEnv* = manager->getEnv());
};
std::string BuildForwardMessage(Contact*, std::initializer_list<ForwardNode>);

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
	std::vector<unsigned long long> getMemberList();
	/*以string格式取群成员列表
	格式：
		每个群成员id间用逗号分隔
	*/
	std::string MemberListToString();
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
	return "[mirai:at:" + std::to_string(a.id) + "]";
}
 /*At一个群成员*/
 inline std::string At(unsigned long long a) {
	/*返回at这个人的miraicode*/
	return "[mirai:at:" + std::to_string(a) + "]";
}

/*群消息事件声明*/
class GroupMessageEvent {
public:
	//来源群
	Group group;
	//发送人
	Member sender;
	//信息本体
	std::string message;
	//消息源
	MessageSource messageSource;
	GroupMessageEvent(Group g, Member f, std::string s, MessageSource s1) {
		this->group = g;
		this->sender = f;
		this->message = s;
		this->messageSource = s1;
	}
};

/*私聊消息事件类声明*/
class PrivateMessageEvent {
public:
	//发起人
	Friend sender;
	//附带消息
	std::string message;
	//信息源
	MessageSource messageSource;
	PrivateMessageEvent(Friend f, std::string s, MessageSource ms) {
		this->sender = f;
		this->message = s;
		this->messageSource = ms;
	}
};

/*群聊邀请事件类声明*/
class GroupInviteEvent {
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
	GroupInviteEvent(unsigned long long gi, std::string gn, unsigned long long ii, std::string inick, std::string s) {
		this->inviterNick = inick;
		this->groupid = gi;
		this->inviterid = ii;
		this->groupName = gn;
		this->source = s;
	}
};

/*好友申请事件声明*/
class NewFriendRequestEvent {
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
	NewFriendRequestEvent(unsigned long long i, std::string m, std::string s) {
		this->senderid = i;
		this->message = m;
		this->source = s;
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
	int type = NULL;
	//新进入的成员
	Member member;
	//目标群
	Group group;
	//邀请人, 当type = 1时存在，否则则和member变量相同
	Member invitor;
	MemberJoinEvent(int t, Member m, Group g, Member i) {
		this->type = t;
		this->member = m;
		this->group = g;
		this->invitor = i;
	}
};

/*群成员离开*/
class MemberLeaveEvent {
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
	MemberLeaveEvent(int t, Member m, Group g, Member i) {
		this->type = t;
		this->member = m;
		this->group = g;
		this->operater = i;
	}
};

/*撤回信息*/
class RecallEvent {
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
	RecallEvent() {}
	RecallEvent(int t, int t2, unsigned long a, unsigned long o, std::string id, std::string ii, unsigned long g = 0) {
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
class BotJoinGroupEvent {
public:
	//1-主动加入,2-被邀请加入,3-提供恢复群主身份加入
	int type;
	//进入的群
	Group group;
	//当type=2时存在，为邀请人，否则为NULL
	Member invitor = Member();
	BotJoinGroupEvent(int t, Group g, Member i) {
		this->type = t;
		this->group = g;
		this->invitor = i;
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
class SchedulingEvent {
public:
	void init() {};
	/*自定义id标识符*/
	std::vector<std::string> ids;
	SchedulingEvent(std::string str) {
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

/*监听类声明*/
class Event {
private:
	using GME = std::function<void(GroupMessageEvent)>;
	using PME = std::function<void(PrivateMessageEvent)>;
	using GI = std::function<void(GroupInviteEvent)>;
	using NFRE = std::function<void(NewFriendRequestEvent)>;
	using MJ = std::function<void(MemberJoinEvent)>;
	using ML = std::function<void(MemberLeaveEvent)>;
	using R = std::function<void(RecallEvent)>;
	using S = std::function<void(SchedulingEvent)>;
	using BJ = std::function<void(BotJoinGroupEvent)>;

	/*
	* 不使用vector做可重复监听的部分，因为没有什么必要且vector比变量占内存
	*/
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
	
	GMENode* GMHead = new GMENode();
	PMENode* PMHead = new PMENode();
	GINode* GHead = new GINode();
	NFRENode* NFHead = new NFRENode();
	MJNode* MJHead = new MJNode();
	MLNode* MLHead = new MLNode();
	RNode* RHead = new RNode();
	SNode* SHead = new SNode();
	BJNode* BHead = new BJNode();

	GMENode* GMTail = GMHead;
	PMENode* PMTail = PMHead;
	GINode* GTail = GHead;
	NFRENode* NFTail = NFHead;
	MJNode* MJTail = MJHead;
	MLNode* MLTail = MLHead;
	RNode* RTail = RHead;
	SNode* STail = SHead;
	BJNode* BTail = BHead;
	
public:
	/*
	* 广播函数重载
	*/

	void broadcast(GroupMessageEvent g) {
		GMENode *now = GMHead;
		while (now != nullptr) {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}
	void broadcast(PrivateMessageEvent p) {
		PMENode* now = PMHead;
		while (now != nullptr) {
			if (now->enable) { now->f(p); }
			now = now->next;
		}
	}
	void broadcast(GroupInviteEvent g) {
		// TODO 改成链表
		GINode* now = GHead;
		while (now != nullptr)  {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}
	void broadcast(NewFriendRequestEvent g) {
		// TODO 改成链表
		NFRENode* now = NFHead;
		while (now != nullptr) {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}
	void broadcast(MemberJoinEvent g) {
		MJNode* now = MJHead;
		while (now != nullptr) {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}
	void broadcast(MemberLeaveEvent g) {
		MLNode* now = MLHead;
		while (now != nullptr) {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}
	void broadcast(RecallEvent r) {
		RNode* now = RHead;
		while (now != nullptr) {
			if (now->enable) { now->f(r); }
			now = now->next;
		}
	}
	void broadcast(SchedulingEvent g) {
		SNode* now = SHead;
		while (now != nullptr) {
			if (now->enable) { now->f(g); }
			now = now->next;
		}
	}
	void broadcast(BotJoinGroupEvent b) {
		BJNode* now = BHead;
		while (now != nullptr) {
			if (now->enable) { now->f(b); }
			now = now->next;
		}
	}

	/*
	* 监听函数重载
	*/

	GMENode* registerEvent(GME f) {
		GMENode* node = new GMENode();
		node->f = f;
		GMTail->next = node;
		GMTail->nextNode = node;
		GMTail = node;
		return node;
	}
	PMENode* registerEvent(PME f) {
		PMENode* node = new PMENode();
		node->f = f;
		PMTail->next = node;
		PMTail->nextNode = node;
		PMTail = node;
		return node;
	}
	GINode* registerEvent(GI f) {
		GINode* node = new GINode();
		node->f = f;
		GTail->next = node;
		GTail->nextNode = node;
		GTail = node;
		return node;
	}
	NFRENode* registerEvent(NFRE f) {
		NFRENode* node = new NFRENode();
		node->f = f;
		NFTail->next = node;
		NFTail->nextNode = node;
		NFTail = node;
		return node;
	}
	MJNode* registerEvent(MJ f) {
		MJNode* node = new MJNode();
		node->f = f;
		MJTail->next = node;
		MJTail->nextNode = node;
		MJTail = node;
		return node;
	}
	MLNode* registerEvent(ML f) {
		MLNode* node = new MLNode();
		node->f = f;
		MLTail->next = node;
		MLTail->nextNode = node;
		MLTail = node;
		return node;
	}
	RNode* registerEvent(R r) {
		RNode* node = new RNode();
		node->f = r;
		RTail->next = node;
		RTail->nextNode = node;
		RTail = node;
		return node;
	}
	SNode* registerEvent(S f) {
		SNode* node = new SNode();
		node->f = f;
		STail->next = node;
		STail->nextNode = node;
		STail = node;
		return node;
	}
	BJNode* registerEvent(BJ f) {
		BJNode* node = new BJNode();
		node->f = f;
		BTail->next = node;
		BTail->nextNode = node;
		BTail = node;
		return node;
	}

	~Event();
};

//取消进程
inline void releaseThread() {
		gvm->DetachCurrentThread();
	}

/*声明全局监听对象(广播源)*/
extern Event* procession;

/*声明插件启用和结束函数*/
void onDisable();
void onEnable();