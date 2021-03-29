#include "pch.h"

//全局env变量
extern JNIEnv* genv;
extern JavaVM* gvm;
extern int JNIVersion;

/*日志类声明*/
class Logger {
private:
	jclass CPP_lib = NULL;
	jmethodID sinfo = NULL;
	jmethodID swarning = NULL;
	jmethodID serror = NULL;
public:
	void init(JNIEnv* = genv);
	/*发送普通日志*/
	void Info(std::string log, JNIEnv* = genv);
	/*发送警告*/
	void Warning(std::string log, JNIEnv* = genv);
	/*发送错误*/
	void Error(std::string log, JNIEnv* = genv);
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
	Config() {};
	void Init(JNIEnv* = genv);
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
	std::string jstring2str(jstring jstr, JNIEnv* = genv);
	/*
	* 名称:str2jstring
	* 作用:string类型转jstring类型
	* 参数:JNIEnv,const char*(string)
	* 返回值:jstring
	* 来源:https://blog.csdn.net/chunleixiahe/article/details/51394116
	*/
	jstring str2jstring(const char* pat, JNIEnv* = genv);
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
		genv->ThrowNew(config->initexception, (this->description + " step:" + std::to_string(this->step)).c_str());
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
		//genv->ThrowNew(config->initexception, (this->description).c_str());
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
		//genv->ThrowNew(config->initexception, (this->description).c_str());
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
		//genv->ThrowNew(config->initexception, (this->description).c_str());
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
		//genv->ThrowNew(config->initexception, (this->description).c_str());
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
		//genv->ThrowNew(config->initexception, (this->description).c_str());
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
		//genv->ThrowNew(config->initexception, (this->description).c_str());
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
		this->description = "找不到群";
	}
	//返回错误信息
	std::string what()
	{
		return this->description;
	}
	void raise() {
		//genv->ThrowNew(config->initexception, (this->description).c_str());
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
		std::string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->recallMsgM,
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
	std::string queryURL(JNIEnv* = genv);

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
	void sendTo(Contact* c, JNIEnv* = genv);
};
std::string BuildForwardMessage(Contact*, std::initializer_list<ForwardNode>);

/*好友类声明*/
class Friend:public Contact{
public:
	Friend(unsigned long long, JNIEnv* =genv);
	Friend() {};
	//初始化当前对象，可能抛出异常
	//昵称
	/*
	* 上传本地图片，务必要用绝对路径
	* 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到好友
	* 最大支持图片大小为30MB
	* 可能抛出invalid_argument异常代表路径无效
	*/
	Image uploadImg(std::string filename, JNIEnv* = genv);
	/*发送信息*/
	//发送miraicode
	 MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	 MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(std::string msg, JNIEnv* = genv);
	//发送文本信息
	MessageSource SendMsg(std::string msg, JNIEnv* = genv);
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
	Member(unsigned long long qqid, unsigned long long groupid, JNIEnv* = genv);
	/*
	* 上传本地图片，务必要用绝对路径
	* 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
	* 最大支持图片大小为30MB
	* 可能抛出invalid_argument异常代表路径无效
	*/
	Image uploadImg(std::string filename, JNIEnv* = genv);
	Member() {};
	//获取权限，会在构造时调用，请使用permission缓存变量
	unsigned int getPermission(JNIEnv* = genv);
	/*发送信息*/
	//发送miraicode
	 MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	 MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(std::string msg, JNIEnv* = genv);
	//发送文本信息，不进行miraicode解析
	MessageSource SendMsg(std::string msg, JNIEnv* = genv);
	/*禁言当前对象，单位是秒，最少0秒最大30天
	* 返回值对应报错
	*	"E1" - 找不到群
	*	"E2" - 找不到群成员
	*	"E3" - 机器人无权限禁言对方
	*	"E4" - 禁言时间超出0s~30d
	*	"Y" - 一切正常
	*/
	void Mute(int time, JNIEnv* = genv);
	/*踢出这个群成员*/
	void Kick(std::string reason, JNIEnv* = genv);
};

/*群聊类声明*/
class Group :public Contact {
private:
	std::string memberlist = "";
public:
	/*取群成员列表-vector<long>*/
	std::vector<unsigned long long> getMemberList();
	/*以string格式取群成员列表
	格式：
		每个群成员id间用逗号分隔
	*/
	std::string MemberListToString();
	//取群主
	Member getOwner(JNIEnv* = genv);
	//构建以群号构建群对象
	Group(unsigned long long, JNIEnv* = genv);
	/*
	* 上传本地图片，务必要用绝对路径
	* 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
	* 最大支持图片大小为30MB
	* 可能抛出invalid_argument异常代表路径无效
	*/
	Image uploadImg(std::string filename, JNIEnv* = genv);
	Group() {};
	/*
	* 设置全员禁言
	* param: sign = true时为开始，false为关闭
	*/
	void setMuteAll(bool sign, JNIEnv* = genv);
	/*发送信息*/
	MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(std::string msg, JNIEnv* = genv);
	MessageSource SendMsg(std::string msg, JNIEnv* = genv);
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
	std::string message;
	NewFriendRequestEvent(Friend f, std::string s) {
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

/*启动定时任务,time是多少毫秒后开始，id是自定义标识符*/
inline void SetScheduling(long time, std::initializer_list<std::string> args) {
	Json::Value obj;
	Json::Value root;
	for (std::string it : args) {
		obj.append(it);
	}
	root["value"] = obj;
	genv->CallStaticVoidMethod(config->CPP_lib, config->Schedule, (jlong)time, tools.str2jstring(tools.JsonToString(root).c_str()));
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
	typedef std::function<void(GroupMessageEvent)> GME;
	typedef std::function<void(PrivateMessageEvent)> PME;
	typedef std::function<bool(GroupInviteEvent)> GI;
	typedef std::function<bool(NewFriendRequestEvent)> NFRE;
	typedef std::function<void(MemberJoinEvent)> MJ;
	typedef std::function<void(MemberLeaveEvent)> ML;
	typedef std::function<void(RecallEvent)> R;
	typedef std::function<void(SchedulingEvent)> S;

	/*
	* 不使用vector做可重复监听的部分，因为没有什么必要且vector比变量占内存
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
	* 广播函数重载
	*/

	void broadcast(GroupMessageEvent g) {
		this->GMEf(g);
	}
	void broadcast(PrivateMessageEvent p) {
		this->PMEf(p);
	}
	std::string broadcast(GroupInviteEvent g) {
		return (this->GIf(g) ? "true" : "false");
	}
	std::string broadcast(NewFriendRequestEvent g) {
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

/*
	https://stackoverflow.com/questions/12900695/how-to-obtain-jni-interface-pointer-jnienv-for-asynchronous-calls
	取消了threadgroupname 因为不可能在获取env前str to jstring 操作
*/
inline JNIEnv* getEnv(char* threadName = NULL) {
	JNIEnv* env = nullptr;
	JavaVMAttachArgs args{
		JNIVersion,
		threadName,
		NULL
	};
	gvm->AttachCurrentThread((void**)&env, &args);
	return env;
}
//取消进程
inline void releaseThread() {
		gvm->DetachCurrentThread();
	}

/*声明全局监听对象(广播源)*/
extern Event* procession;

/*声明插件启用和结束函数*/
void onDisable();
void onEnable();