#include "pch.h"

//全局env变量
extern JNIEnv* genv;

/*日志类声明*/
class Logger {
private:
	jclass CPP_lib = NULL;
	jmethodID sinfo = NULL;
	jmethodID swarning = NULL;
	jmethodID serror = NULL;
public:
	void init();
	/*发送普通日志*/
	void Info(string log);
	/*发送警告*/
	void Warning(string log);
	/*发送错误*/
	void Error(string log);
	~Logger();
};

/*声明全局日志对象*/
extern Logger* logger;

/*配置类声明*/
class Config {
public:
	jclass CPP_lib = NULL;
	/*图像类*/
	jmethodID Query = NULL;
	/*好友类*/
	jmethodID SendMsg2F = NULL;
	jmethodID NickorNameF = NULL;
	/*群聊成员类*/
	jmethodID SendMsg2M = NULL;
	jmethodID NickorNameM = NULL;
	/*群聊类*/
	jmethodID SendMsg2G = NULL;
	/*定时任务*/
	jmethodID Schedule = NULL;
	Config() {};
	void Init();
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
	*/
	string jstring2str(jstring jstr);
	/*
	* 名称:str2jstring
	* 作用:string类型转jstring类型
	* 参数:JNIEnv,const char*(string)
	* 返回值:jstring
	*/
	jstring str2jstring(const char* pat);
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
	jmethodID Query = NULL;
public:
	//图片id，样式:` {xxx}.xx `
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
	Image(string);
	/*
	* 上传本地图片，务必要用绝对路径
	* 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到好友
	* 最大支持图片大小为30MB
	* 可能抛出invalid_argument异常代表路径无效
	* 示例:
			param.sender.SendMsg(Image::uploadImg2Friend(param.env, param.sender.id, "C:\\Users\\***\\Desktop\\a.jpg").toMiraiCode());
	*/
	static Image uploadImg2Friend(unsigned long, string);
	/*
	* 上传本地图片，务必要用绝对路径
	* 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
	* 最大支持图片大小为30MB
	* 可能抛出invalid_argument异常代表路径无效
	* 示例:
			param.group.SendMsg(Image::uploadImg2Group(param.env, param.group.id, "C:\\Users\\***\\Desktop\\a.jpg").toMiraiCode());
	*/
	static Image uploadImg2Group(unsigned long, string);
	/*
   * 上传本地图片，务必要用绝对路径
   * 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
   * 最大支持图片大小为30MB
   * 可能抛出invalid_argument异常代表路径无效
   * 示例:
		   param.sender.SendMsg(Image::uploadImg2Member(param.env, param.group.id, param.sender.id, "C:\\Users\\***\\Desktop\\a.jpg").toMiraiCode());
   */
	static Image uploadImg2Member(unsigned long, unsigned long, string);

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
	jmethodID Send_Msg_id = NULL;
	jmethodID NickorName_id = NULL;
public:
	unsigned long id = NULL;
	Friend(unsigned long);
	Friend() {};
	string nick;
	/*发送信息*/
	void SendMsg(jstring msg) {
		genv->CallStaticVoidMethod(config->CPP_lib, this->Send_Msg_id, msg, (jlong)this->id);
	}
	/*发送信息*/
	void SendMsg(string msg) {
		genv->CallStaticVoidMethod(config->CPP_lib, this->Send_Msg_id, tools.str2jstring(msg.c_str()), (jlong)this->id);
	}
};

/*群成员类声明*/
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
	/*返回at这个人的miraicode*/
	string at() {
		return "[mirai:at:" + to_string(this->id) + "] ";
	}
	/*发送信息*/
	void SendMsg(jstring msg) {
		genv->CallStaticVoidMethod(config->CPP_lib, this->Send_Msg_id, msg, (jlong)this->id, (jlong)this->groupid);
	}
	/*发送信息*/
	void SendMsg(string msg) {
		genv->CallStaticVoidMethod(config->CPP_lib, this->Send_Msg_id, tools.str2jstring(msg.c_str()), (jlong)this->id, (jlong)this->groupid);
	}
};

/*群聊类声明*/
class Group {
private:
	jmethodID Send_Msg_id = NULL;
public:
	/*群号*/
	unsigned long id = NULL;
	Group(unsigned long);
	Group() {};
	/*发送信息*/
	void SendMsg(jstring msg) {
		genv->CallStaticVoidMethod(config->CPP_lib, config->SendMsg2G, msg, (jlong)this->id);
	}
	/*发送信息*/
	void SendMsg(string msg) {
		SendMsg(tools.str2jstring(msg.c_str()));
	}
};

/*群消息事件声明*/
class GroupMessageEvent {
public:
	//来源群
	Group group;
	//发送人
	Member sender;
	//信息本体
	string message;
	GroupMessageEvent(Group g, Member f, string s) {
		this->group = g;
		this->sender = f;
		this->message = s;
	}
};

/*私聊消息事件类声明*/
class PrivateMessageEvent {
public:
	//发起人
	Friend sender;
	//附带消息
	string message;
	PrivateMessageEvent(Friend f, string s) {
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
	//退出的成员的成员
	Member member;
	//目标群
	Group group;
	//操作人, 主动退出时与member相同，改成员可能是当前bot，operater以与系统operator区分
	Member operater;
	MemberLeaveEvent(int t, Member m, Group g, Member i) {
		this->type = t;
		this->member = m;
		this->group = g;
		this->operater = i;
	}
};

/*定时任务执行*/
class SchedulingEvent {
public:
	/*自定义id标识符*/
	int id = 0;
	SchedulingEvent(int ido){
		id = ido;
	}
};

/*
启动定时任务,time是多少毫秒后开始，id是自定义标识符
*/
void SetScheduling(long time, int id);

/*监听类声明*/
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
	* 不使用vector做可重复监听的部分，因为没有什么必要且vector比变量占内存
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
	* 广播函数重载
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
	void registerEvent(S f) {
		this->Sf = f;
	}
};

/*声明注册监听事件*/
void EventRegister();

/*声明全局监听对象(广播源)*/
extern Event* procession;

/*声明插件启用和结束函数*/
void onDisable();
void onEnable();