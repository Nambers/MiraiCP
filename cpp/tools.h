#pragma once

#include "pch.h"

/// @brief 全局JavaVM对象，用于多线程管理中新建线程的JNIEnv.
extern JavaVM* gvm;
/// @brief JNI 版本.
extern int JNIVersion;

/*!
 * @class threadManager
 * @brief 多线程管理.
 */
class threadManager {
private:
	/// @brief 每个线程实例.
	struct t {
		JNIEnv* e;
		bool attach;
	};
	std::map<std::string, t> _threads;/// < 线程池(线程id:env).
	std::recursive_mutex mtx; ///< 线程池读写锁.
	void newEnv(const char* threadName = NULL);///< 新建一个env，于getEnv中没取到env时调用.
	/// 判断该线程id是否包含在线程池里
	bool included(const std::string& id);
public:
	/// @brief 获取线程id.
	static std::string getThreadId() {
		std::ostringstream oss;
		oss << std::this_thread::get_id();
		return oss.str();
	}

	threadManager();

	/// @brief 设置env给当前线程.
	void setEnv(JNIEnv* e);

	/*!
	 * 	@brief 结束当前线程的env，也就是释放当前线程缓存的env.
	 *  @note 不过继续调用getEnv()将再次获取，所以本方法调用后线程也可以通过getEnv重新获取一个env，本方法的作用就是在结束后释放空间
     * @example 多线程管理
     * @code
     * void func(){
     *      //do some things
     *      // 一个完整的线程应该在结束时调用detach来释放env的空间
     *      manager->detch();
     * }
     * @endcode
	 */
	void detach();

    /// @brief 取env,如果不存在重新获取
    /// @internal 一般为miraicp内部调用jni接口时调用
	JNIEnv* getEnv();
};

/// 线程管理实例
extern threadManager* manager;

/*!
 * @class Logger
 * @brief 日志
 * @example 发送日志
	 *  发送消息级日志
	 *  @code logger->Info(string) @endcode
	 *  发送警告级日志
	 *  @code logger->Warning(string) @endcode
	 *  发送错误级日志
	 *  @code logger->Error(string) @endcode
 */
class Logger {
private:
	jclass CPP_lib = NULL;
	jmethodID log = nullptr;

	/// 日志底层实现封装
	void log0(const std::string&, JNIEnv*, int);

public:
	/// @brief 封装lambda类型
	/// @param string 日志内容
	/// @param 日志级别
	///     - 0 info
	///     - 1 warning
	///     - 2 error
	typedef std::function<void(std::string, int)> action;
	/// @brief loggerhandler会在每次log执行前执行一遍，可用于执行自定义的保存操作等
	struct handler {
		/// @brief 是否启用
		bool enable = true;
		/// @brief 执行的操作，格式为lambda
		Logger::action action = [](std::string content, int level) {};
	} loggerhandler;

	// 初始化 获取methodid
	void init(JNIEnv* = manager->getEnv());

	///发送普通(info级日志)
	void Info(const std::string&, JNIEnv* = manager->getEnv());

	///发送警告(warning级日志)
	void Warning(const std::string&, JNIEnv* = manager->getEnv());

	///发送错误(error级日志)
	void Error(const std::string&, JNIEnv* = manager->getEnv());

	/// @brief 设置loggerhandler的action
	/// @param action 执行的操作
	/// @see Logger::handler
	/// @example 设置loggerhandler的action
	/// @code logger->registerHandle([](std::string content, int level){
	///     \\do some things
	/// });@endcode
	inline void registerHandle(action action);

	/// @brief 设置handler的启用状态
	/// @param state 状态，启用或者关闭
	/// @example 设置handler的启用状态
	/// @code logger->setHandleState(ture); @endcode
	inline void setHandleState(bool state);

	~Logger();
};

/// 声明全局日志对象
extern Logger* logger;

/// @brief 配置类声明, 主要存放各种jmethodid, MiraiCP内部使用, 不需要更改或其他操作
/// @internal 一般为MiraiCP内部调用jni接口使用
/// @class Config
class Config {
public:
	/// kt中JNI接口类
	jclass CPP_lib = nullptr;
	/// 异常类
	jclass initexception = nullptr;
	/// 撤回信息
	jmethodID KRecall = nullptr;
	/// 发送信息
	jmethodID KSend = nullptr;
	/// 查询信息接口
	jmethodID KRefreshInfo = nullptr;
	/// 上传图片
	jmethodID KUploadImg = nullptr;
	/// 取好友列表
	jmethodID KQueryBFL = nullptr;
	/// 取群组列表
	jmethodID KQueryBGL = nullptr;
	/// 上传文件
	jmethodID KSendFile = nullptr;
	/// 查询文件信息
	jmethodID KRemoteFileInfo = nullptr;
	/// 查询图片下载地址
	jmethodID KQueryImgUrl = nullptr;
	/// 禁言
	jmethodID KMuteM = nullptr;
	/// 查询权限
	jmethodID KQueryM = nullptr;
	/// 踢出
	jmethodID KKickM = nullptr;
	/// 取群主
	jmethodID KQueryOwner = nullptr;
	/// 全员禁言
	jmethodID KMuteGroup = nullptr;
	/// 查询群成员列表
	jmethodID KQueryML = nullptr;
	/// 定时任务
	jmethodID KSchedule = nullptr;
	/// 构建转发信息
	jmethodID KBuildforward = nullptr;
	/// 好友申请事件
	jmethodID KNfroperation = nullptr;
	/// 群聊邀请事件
	jmethodID KGioperation = nullptr;
    /// 回复(引用并发送)
	jmethodID KSendWithQuote = nullptr;

	Config() {};

	void Init(JNIEnv* = manager->getEnv());

	~Config();
};

extern Config* config;

/// @brief 工具类声明, 常用的一些转换工具, 如需转码使用std::filesystem
/// @class Tools
class Tools {
public:
	/*!
	 * @name jstring2str
	 * @brief string类型转码转换到jstring类型, UTF16 -> UTF8
	 * @note 来源https://blog.csdn.net/chunleixiahe/article/details/51394116
	 * @param jstr 转换内容,jstring类型
	 * @param env 可选，JNIEnv
	 * @return 内容转换成jstring类型
	 */
	static std::string jstring2str(jstring jstr, JNIEnv* = manager->getEnv());

	/*!
	 * @name str2jstring
	 * @brief string类型到jsting类型 UTF8 -> UTF16
	 * @note 来源https://blog.csdn.net/chunleixiahe/article/details/51394116
	 * @param pat const char*(string.c_str())转换的内容
	 * @param env 可选JNIEnv
	 * @return 转换后jstring类型
	 */
	static jstring str2jstring(const char* pat, JNIEnv* = manager->getEnv());

	/*!
	 * @name JLongToString
	 * @brief jlong类型到string类型
	 * @param val 需要转换的数值
	 * @return 转换后string类型
	 */
	static std::string JLongToString(jlong val);

	/*!
	 * @brief 替换全部在一个字符串中.
	 * @param str 原字符串.
	 * @param from 需要被替换的字符.
	 * @param to 替换到的字符.
	 * @return 返回替换后的字符串.
	 * @note 来源:https://stackoverflow.com/a/24315631/14646226
	 */
	static std::string replace(std::string str, const std::string& from, const std::string& to) {
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Handles case where 'to' is a substd::string of 'from'
		}
		return str;
	}

	/// @brief long long 类型的vector格式化输出
	/// @param v vector
	/// @return string
	static std::string VectorToString(std::vector<unsigned long long> v);

	/// @brief 从string格式化到vector
	/// @param s string
	/// @return vector
	static std::vector<unsigned long long> StringToVector(std::string s);
};

/// @brief 总异常抽象类
/// @interface MiraiCPException
class MiraiCPException : public std::exception {
public:
	/// 异常信息
	virtual std::string what() { return "MiraiCP异常"; };
	/// raise 抛出方法
	virtual void raise() {
	    if(logger != nullptr)
	        logger->Error(what());
	};
};

/// 初始化异常
/// @see MiraiCPException
class InitException : public MiraiCPException {
private:
	std::string description;
public:
	int step = 0;

	InitException(std::string text, int step) {
		this->description = text;
		this->step = step;
	}

	/// 返回错误信息
	std::string what() {
		return this->description;
	}

	void raise() {
		manager->getEnv()->ThrowNew(config->initexception,
			(this->description + " step:" + std::to_string(this->step)).c_str());
	}
};

/// 文件读取异常.
/// @see MiraiCPException
class UploadException : public MiraiCPException {
public:
	UploadException(std::string text) {
		this->description = "上传(图片/文件)异常" + text;
	}

	//返回错误信息
	std::string what() {
		return this->description;
	}

	void raise() {
		logger->Error(this->description);
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}

private:
	std::string description;
};

/// 内部异常, 通常为json读写问题
/// @see MiraiCPException
class APIException : public MiraiCPException {
public:
	APIException(std::string text) {
		this->description = "MiraiCP内部无法预料的错误:" + text;
	}

	//返回错误信息
	std::string what() {
		return this->description;
	}

	void raise() {
		logger->Error(this->description);
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}

private:
	std::string description;
};

/// 机器人操作异常
/// @see MiraiCPException
class BotException : public MiraiCPException {
public:
	/*
	*	1 - 机器人无权限执行
	*/
	int type = 0;

	BotException(int type) {
		this->type = type;
		switch (type) {
		case 1:
			this->description = "没有权限执行该操作";
			break;
		}
	}

	//返回错误信息
	std::string what() {
		return this->description;
	}

	void raise() {
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}

private:
	std::string description;
};

/// 禁言异常
/// @see MiraiCPException
class MuteException : public MiraiCPException {
public:
	/*
	*	 禁言时间超出0s~30d
	*/
	MuteException() {
		this->description = "禁言时长不在0s~30d中间";
	}

	//返回错误信息
	std::string what() {
		return this->description;
	}

	void raise() {
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}

private:
	std::string description;
};

/// 获取群成员错误
/// @see MiraiCPException
class MemberException : public MiraiCPException {
public:
	/*
	*   "1" - 找不到群
	*	"2" - 找不到群成员
	*/
	int type = 0;

	MemberException(int type) {
		this->type = type;
		switch (type) {
		case 1:
			this->description = "找不到群";
			break;
		case 2:
			this->description = "找不到群成员";
			break;
		}
	}

	//返回错误信息
	std::string what() {
		return this->description;
	}

	void raise() {
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}

private:
	std::string description;
};

/// 获取群成员错误
/// @see MiraiCPException
class FriendException : public MiraiCPException {
public:
	/*
	*   找不到好友
	*/
	FriendException() {
		this->description = "找不到好友";
	}

	//返回错误信息
	std::string what() {
		return this->description;
	}

	void raise() {
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}

private:
	std::string description;
};

/// 获取群错误
/// @see MiraiCPException
class GroupException : public MiraiCPException {
public:
	GroupException() {
		this->description = "C++:找不到群";
	}

	//返回错误信息
	std::string what() {
		return this->description;
	}

	void raise() {
		//manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
	}

private:
	std::string description;
};

/// 撤回异常
/// @see MiraiCPException
class RecallException : public MiraiCPException {
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

/// 上传异常
/// @see MiraiCPException
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

inline void ErrorHandle(const std::string& re){
    if(re == "E1")
        throw FriendException();
    if(re == "E2")
        throw GroupException();
    if(re == "E3")
        throw MemberException(1);
    if(re == "E4")
        throw MemberException(2);
    if(re == "E5")
        throw APIException("");
}

/// MiraiCode父类, 指可以被转换成miraicode的类型
class MiraiCodeable {
public:
	virtual std::string toMiraiCode() = 0;
};

/// @brief miraicode字符串
class MiraiCode {
private:
	std::string content = "";
public:
	/// 输出当前内容
	std::string toString() {
		return content;
	}

	/// 和toString作用一样
	std::string toMiraiCode() {
		return toString();
	}

	/// 从MiraiCodeable类型初始化一个miraicode字符串
	MiraiCode(MiraiCodeable* a) {
		content = a->toMiraiCode();
	}

	/// 从文本初始化一个miraicode字符串
	MiraiCode(std::string a) {
		content = a;
	}

	MiraiCode operator+(MiraiCodeable* a) {
		return MiraiCode(content + a->toMiraiCode());
	}

	MiraiCode operator+(std::string a) {
		return MiraiCode(content + a);
	}

	MiraiCode operator+=(MiraiCodeable* a) {
		return MiraiCode(this->content + a->toMiraiCode());
	}

	MiraiCode operator+=(std::string a) {
		return MiraiCode(this->content + a);
	}

	MiraiCode operator=(MiraiCodeable* a) {
		return MiraiCode(a->toMiraiCode());
	}

	MiraiCode operator=(std::string a) {
		return MiraiCode(a);
	}

	MiraiCode plus(MiraiCodeable* a) {
		return MiraiCode(content + a->toMiraiCode());
	}

	MiraiCode plus(std::string a) {
		return MiraiCode(content + a);
	}
};

/*!
 * @brief 小程序卡片
 * @see LightAppStyle1, LightAppStyle2, LightAppStyle3
 * @example 通过常量构建并发送小程序卡片
 * @code
 * procession->registerEvent([](GroupMessageEvent e) {
		//修改里面的属性从而自定义
		LightAppStyle1 a = LightAppStyle1();
		LightAppStyle2 b = LightAppStyle2();
		LightAppStyle3 c = LightAppStyle3();
		e.group.SendMiraiCode(LightApp(a).toMiraiCode());
		e.group.SendMiraiCode(LightApp(b).toMiraiCode());
		e.group.SendMiraiCode(LightApp(c).toMiraiCode());
	});
 * @endcode
 * @example 通过文本构建并发送小程序卡片
 * @code
 * procession->registerEvent([](GroupMessageEvent e) {
		//风格1，适合文字展示，不能交互,无大图
		//图标地址，应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
		//小标题
		string titles = "{\"title\":\"hi\", \"value\":\"test3\"}";
		//下面的按钮，但是不能按
		string buttons = "{\"name\":\"Test4\",\"action\":\"plugin\",\"actionData\":\"https://baidu.com\"}";
		string x = "{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[应用]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":"
			"{\"appName\":\"Test1\",\"appType\":4,\"appid\":1109659848,"
			"\"iconUrl\":\""+icon+"\"},"
			"\"data\":["+titles+"],"
			"\"title\":\"Test2\",\"button\":"
			"["+buttons+"],"
			"\"emphasis_keyword\":\"\"}},\"text\":\"\",\"sourceAd\":\"\"}";
		e.group.SendMiraiCode(LightApp(x).toString());
		//风格2，不能交互，有预览图
		//icon图标,应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string icon1 = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
		//预览图(大图),应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string preview1 = icon1;
		string a = "{\"config\":"
			"{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"1.0.0.103\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1110081493\",\"preview\":\""+preview1+"\",\"shareTemplateData\":{},"
			"\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"Test5\",\"title\":\"Test6\","
			"\"host\":{\"uin\":1000000,\"nick\":\"应用消息\"},"
			"\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\""+icon1+"\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
		e.group.SendMiraiCode(LightApp(a).toString());
		//风格3，可以跳转到网址，有预览图
		//跳转链接
		string url = "https://www.baidu.com";
		//icon图标,应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string icon2 = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
		//预览图(大图),应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string preview = icon2;
		string b = "{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\""+preview+"\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"Test1\",\"title\":\"Test3\",\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\""+icon+"\",\"qqdocurl\":\""+url+"\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
		e.group.SendMiraiCode(LightApp(b).toString());
		});
 * @endcode
 */
class LightApp : public MiraiCodeable {
public:
	std::string content = "";

	/// @brief 使用纯文本构造，推荐使用其他结构体方法构造
	/// @param content 构造文本
	LightApp(std::string content) {
		this->content = content;
	}

	/// 使用样式1,适合文字展示，无大图，不能交互
	/// @param c 结构体，用于自定义里面的数据
	/// @see LightAppStyle1 in constants.h
	LightApp(LightAppStyle1 c) {
		this->content =
			"{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[应用]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":"
			"{\"appName\":\"" + c.appName + "\",\"appType\":4,\"appid\":1109659848,"
			"\"iconUrl\":\"" + c.icon + "\"},"
			"\"data\":[" + c.titles + "],"
			"\"title\":\"" +
			c.title + "\",\"button\":"
			"[" + c.buttons + "],"
			"\"emphasis_keyword\":\"\"}},\"text\":\"\",\"sourceAd\":\"\"}";
	}

	/// 使用样式2，有大图，不能交互
	/// @param c 结构体，用于自定义里面的数据
	/// @see LightAppStyle1 in constants.h
	LightApp(LightAppStyle2 c) {
		this->content = "{\"config\":"
			"{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"1.0.0.103\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1110081493\",\"preview\":\"" + c.preview +
			"\",\"shareTemplateData\":{},"
			"\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" +
			c.title2 + "\",\"title\":\"" + c.title + "\","
			"\"host\":{\"uin\":0,\"nick\":\"\"},"
			"\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" +
			c.icon + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
	}

	/// 样式3，有大图，可以在电脑qq显示，并在电脑上点击的链接会跳转
	/// @param c 结构体，用于自定义里面的数据
	/// @see LightAppStyle1 in constants.h
	LightApp(LightAppStyle3 c) {
		this->content =
			"{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\"" + c.preview +
			"\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" +
			c.title + "\",\"title\":\"" + c.description + "\","
			"\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" +
			c.icon + "\",\"qqdocurl\":\"" + c.url + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
	}

	std::string toMiraiCode() {
		return "[mirai:app:" + Tools::replace(Tools::replace(content, "[", "\\["), "]", "\\]") + "]";
	}
};

/// 图像类声明
class Image : public MiraiCodeable {
public:
	const jmethodID Query = NULL;
	//图片id，样式:` {xxx}.xx `
	std::string id = "";

	/*!
	* @brief 从图片id构造，适用于服务器上已经有的图片，即接收到的
	* @example 图片miraiCode格式例子
	 * @code [mirai:image:{图片id}.jpg] @endcode
	* @note 可以用这个正则表达式找出id ` \\[mirai:image:(.*?)\\] `
	*/
	Image(std::string);

	/*
	* 获取图片下载url
	*/
	std::string queryURL(JNIEnv* = manager->getEnv()) const;

	/*!
	 * @brief 取一个miraicode字符串中全部的图片id，详情见Image
	 * @see Image
	 *  @param  miraicode的字符串
	 *  @return vector容器，每一项为一个图片id
	 *  @example 取一个miraicode字符串中全部的图片id
	 *		@code
	 *          vector<string> temp = Image::GetImgIdFromMiraiCode(param.message);
	 *	        for (int i = 0; i < temp.size(); i++) {
	 *	    	    logger->Info(temp[i]);
	 *	    	    logger->Info("图片下载地址:" + Image(param.env, temp[i]).queryURL());
	 *	         }
	 *		@endcode
	 */
	static std::vector<std::string> GetImgIdsFromMiraiCode(std::string);

	/// 取图片Mirai码
	std::string toMiraiCode();
};

/// 消息源声明
class MessageSource {
public:
    /// 消息的ids
    const std::string ids;
    /// 消息的internalids
    const std::string internalids;
    /// 消息源序列化
    const std::string source;

    MessageSource() {};

    std::string toString() {
        return this->source;
    }

    /**
     * @brief 回复(引用并发送miraicode)
     * @param msg - MiraiCodeable类型指针 - 内容
     * @see MessageSource::quoteAndSendMiraiCode
    */
    MessageSource quoteAndSendMiraiCode(MiraiCodeable* msg, unsigned long long groupid = 0, JNIEnv* env = manager->getEnv()) {
        return quoteAndSendMiraiCode(msg->toMiraiCode(), groupid, env);
    }

    /// 回复(引用并发送miraicode)
    /// @see MessageSource::quoteAndSendMiraiCode
    MessageSource quoteAndSendMiraiCode(MiraiCode msg, unsigned long long groupid = 0, JNIEnv* env = manager->getEnv()) {
        return quoteAndSendMiraiCode(msg.toString(), groupid, env);
    }

    /**
     * @brief 回复(引用并发送)
     * @param c 引用后发送的内容, 为纯文本
     * @param groupid 如果为发送给群成员需把该群成员的groupid传入以帮助获取到该成员
     * @return MessageSource
     */
    MessageSource quoteAndSendMsg(const std::string& c, unsigned long long groupid = 0, JNIEnv* = manager->getEnv());

    /**
     * @brief 回复(引用并发送)
     * @param c 引用后发送的内容, 为MiraiCode形式
     * @param groupid 如果为发送给群成员需把该群成员的groupid传入以帮助获取到该成员
     * @return MessageSource
     */
    MessageSource quoteAndSendMiraiCode(const std::string& c, unsigned long long groupid = 0, JNIEnv* = manager->getEnv());

    /*!
     * @brief 构建消息源
     * @param ids
     * @param internalids
     * @param source
     */
    MessageSource(const std::string& ids, std::string  internalids, const std::string& source);

    /*!
     * @brief 从json字符串反序列化到MessageSource对象
     * @note json应该为以下格式
     * @code
     * {"ids":"", "internalids":""}
     * @endcode
     */
    static MessageSource deserializeFromString(const std::string& source);

    std::string serializeToString();

    /*!
     * @brief 撤回该信息
     * @example 撤回信息
     * @code
     * procession->registerEvent([](GroupMessageEvent e) {
        try {
            e.messageSource.recall();
            e.group.SendMsg("hi").recall();
        }
        catch (MiraiCPException &e) {
            logger->Error("错误");
        }
        });
     * @endcode
    */
    void recall();
};

/*!
 * @brief group, friend, member的父类
 */
class Contact {
protected:
	int _type = 0;
	unsigned long long _id;
	unsigned long long _groupid;
	std::string _nickOrNameCard;
	std::string _avatarUrl;
	unsigned long long _botid;
public:
    /*!
     * @brief 无参初始化Contact类型
     * @internal 一般在MiraiCp内部构造
     */
	Contact() {
		this->_type = 0;
		this->_id = 0;
		this->_groupid = 0;
		this->_nickOrNameCard = "";
		this->_botid = 0;
	}

	/*!
	 * @brief 构造contact类型
	 * @param type 类型
	 *  @see Contact::type()
	 * @param id ID
	 *  @see Contact::id()
	 * @param gid 是member的时候是群id，否则为0
	 *  @see Contact::groupid
	 * @param name 群名片或昵称或群名
	 *  @see Contact::name()
	 * @param botid 对应的botid
	 */
	Contact(int type, unsigned long long id, unsigned long long gid, std::string name, unsigned long long botid) {
		this->_type = type;
		this->_id = id;
		this->_groupid = gid;
		this->_nickOrNameCard = name;
		this->_botid = botid;
	};

	/// @brief 当前对象类型
	///     - 1 Friend 好友
	///     - 2 Group 群聊
	///     - 3 Member 群成员
	int type() { return this->_type; }

	/// @brief id在全部情况存在
	///     - 当当前type为1(Friend)时，为好友id
	///     - 当当前type为2(Group)时，为群id
	///     - 当当前type为3(Member)时，为群成员id
	unsigned long long id() { return this->_id; }

	/// @brief 当type为3的时候存在，否则为0，可以看作补充id
	///     - 当当前type为1(Friend)时，为0
	///     - 当当前type为2(Group)时，为0
	///     - 当当前type为3(Member)时，为群号
	/// @attention 当当前type为2(Group)时，为0，不为群号，id才是群号
	unsigned long long groupid() { return this->_groupid; }

	/// 群名称，群成员群名片，或好友昵称
	std::string nickOrNameCard() { return this->_nickOrNameCard; };

	/// 头像url地址
	std::string avatarUrl() { return this->_avatarUrl; };

	/// 所属bot
	unsigned long long botid() { return this->_botid; };

	nlohmann::json serialization() {
		nlohmann::json j;
		j["type"] = type();
		j["id"] = id();
		j["groupid"] = groupid();
		j["nickornamecard"] = nickOrNameCard();
		j["botid"] = botid();
		return j;
	}

	/// 序列化成文本，可以通过deserializationFromString反序列化，利于保存
	/// @see Contact::fromString()
	std::string serializationToString() {
		return this->serialization().dump();
	}

	/*!
	 * @brief 从json节点反序列化
	 * @param root json节点
	 * @return Contact
	 */
	static Contact deserializationFromJson(nlohmann::json root);

	/// 反序列化成bot，可以通过serializationToString序列化，利于保存
	/// @see Contact::serializationToString()
	/// @param source 序列化后的文本
	/// @throw APIException
	static Contact deserializationFromString(const std::string& source);

    /**
     * @brief 发送miraicode
     * @param msg - MiraiCodeable类型指针 - 内容
    */
    MessageSource SendMiraiCode(MiraiCodeable* msg) {
        return SendMiraiCode(msg->toMiraiCode());
    }

    /// 发送miraicode
    MessageSource SendMiraiCode(MiraiCode msg) {
        return SendMiraiCode(msg.toString());
    }

    /// 发送miraicode
    MessageSource SendMiraiCode(std::string msg, JNIEnv* = manager->getEnv());

    /// 发送纯文本信息
    MessageSource SendMsg(std::string msg, JNIEnv* = manager->getEnv());
};

// 群文件

/// @brief 下载信息
struct dinfo {
	/// 下载地址
	std::string url;
	/// md5 可用于校验
	std::string md5;
	/// sha1 可用于校验
	std::string sha1;
};
/// @brief 文件信息
struct finfo {
	/// 文件大小
	unsigned long long size;
	/// 上传者id
	unsigned long long uploaderid;
	/// 下载次数
	unsigned int downloadtime;
	/// 上传时间, 时间戳格式
	unsigned long long uploadtime;
	/// 上次更改时间, 时间戳格式
	unsigned long long lastmodifytime;
};

/// @brief 远程(群)文件类型
class RemoteFile : public MiraiCodeable {
public:
	/// 文件唯一id, 用于识别
	const std::string id;
	/// 文件内部id, 用于构造miraiCode发送
	const unsigned int internalid;
	/// 文件名
	const std::string name;
	/// 文件大小
	const long long size;
	/// 文件在群文件的路径
	const std::string path;
	/// 文件下载信息
	/// @see dinfo
	const dinfo dinfo;
	/// 文件信息
	/// @see finfo
	const finfo finfo;

	std::string serializeToString();

	static RemoteFile deserializeFromString(const std::string& source);

	/*!
	 * @brief 构造远程(群)文件
	 * @param i ids
	 * @param ii internalids
	 * @param n name
	 * @param s size
	 * @param p path
	 * @param d dinfo
	 * @param f finfo
	 */
	RemoteFile(const std::string& i, unsigned int ii, const std::string& n, long long s, const std::string& p, struct dinfo d, struct finfo f) : id(i),
		internalid(
			ii),
		name(n),
		size(s),
		path(p),
		dinfo(d),
		finfo(f) {};

	/// 仅在上传后构建的有效, 即获取到internalid时(internalid != 0) 否则重新上传并重新获取internalid再转换
	std::string toMiraiCode() {
		if (internalid == 0) {
			// 重新上传
			throw RemoteFileException("toMiraiCode error: internalid错误，重新上传");
		}
		return "[mirai:file:" + id + "," + std::to_string(internalid) + "," + name + "," + std::to_string(size) + "]";
	}
};

/// 较底层api
class LowLevelAPI {
public:
	/// @brief 抽象封装底层发送信息接口
	/// @param content 信息字符串
	/// @param c 目标Contact指针
	/// @param miraicode 是否为miraicode格式
	/// @param env JNIEnv
	/// @return
	static std::string send0(std::string content, Contact* c, bool miraicode, JNIEnv* env) {
		nlohmann::json j;
		j["content"] = content;
		j["contact"] = c->serialization();
		return Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KSend,
			Tools::str2jstring(j.dump().c_str(), env),
			(jboolean)miraicode), env);
	}

	/// @brief 取该联系人的一些信息
	/// @param c 该联系人Contact指针
	/// @return json格式字符串，待解析
	static inline std::string getInfoSource(Contact* c, JNIEnv* env = manager->getEnv()) {
		return Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KRefreshInfo,
			Tools::str2jstring(c->serializationToString().c_str(),
				env)));
	}

	/*!
	 * @brief 上传图片
	 * @param path 本地地址
	 * @param c 上传的对象
	 * @param env JNIEnv
	 * @return string 待解析json
	 */
	static inline std::string uploadImg0(std::string path, Contact* c, JNIEnv* env = manager->getEnv()) {
		return Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KUploadImg,
			Tools::str2jstring(path.c_str(), env),
			Tools::str2jstring(c->serializationToString().c_str(),
				env)));
	}

	/// 每个对象的必有信息
	struct info {
		std::string nickornamecard;
		std::string avatarUrl;
	};

	/// 获取每个对象必有信息
	/// @see LowLevelAPI::info
	static info info0(std::string source) {
		info re;
		nlohmann::json j = nlohmann::json::parse(source);
		re.avatarUrl = j["avatarUrl"];
		re.nickornamecard = j["nickornamecard"];
		return re;
	}
};

///聊天记录里每个消息
class ForwardNode {
public:
	///发送者id
	const unsigned long long id = 0;
	///发送者昵称
	const std::string name = "";
	///发送信息
	const std::string message = "";
	///发送时间
	const int time = 0;

	/// @brief 聊天记录里的每条信息
	/// @param i - 发送者id
	/// @param n - 发送者昵称
	/// @param m - 发送的信息
	/// @param t - 发送时间，以时间戳记
	ForwardNode(const unsigned long long int id, const std::string& name, const std::string& message, const int time)
		: id(id), name(name), message(message), time(time) {}

	/// @brief 构造聊天记录里每条信息
	/// @param c - 发送者的contact指针
	/// @prama m - 发送的信息
	/// @param t - 发送时间，时间戳格式
	ForwardNode(Contact* c, std::string& message, int t) : id(c->id()), name(c->nickOrNameCard()), message(message),
		time(t) {}
};

///聊天记录, 由ForwardNode组成
/// @see class ForwardNode
class ForwardMessage {
public:
	nlohmann::json sendmsg;

	/*!
	*@brief 构建一条聊天记录
	*@details 第一个参数是聊天记录发生的地方
	* 然后是每条信息
	*@example 构建聊天记录
	 * @code
	*ForwardMessage(&e.group,
	*{
	*	ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
	*	ForwardNode(1930893235, "Eritque arcus", "hahaha", -1)
	*}).sendTo(&e.group);
	* @endcode
	*/
	ForwardMessage(Contact* c, std::initializer_list<ForwardNode> nodes);

	/// 发送给群或好友或群成员
	void sendTo(Contact* c, JNIEnv* = manager->getEnv());
};

/// 当前bot账号信息
class Bot {
private:
	bool inited = false;
	std::string _nick;
	std::string _avatarUrl;

	void check() {
		if (!this->inited) {
			refreshInfo();
			this->inited = true;
		}
	}

public:
    /// 该botid
    const unsigned long long id;
	/*!
	 * @brief 刷新bot信息
	 * @param env
	 */
	void refreshInfo(JNIEnv* env = manager->getEnv()) {
		LowLevelAPI::info tmp = LowLevelAPI::info0(Tools::jstring2str(
			(jstring)env->CallObjectMethod(config->CPP_lib, config->KRefreshInfo, Tools::str2jstring(
				Contact(4, 0, 0, "", this->id).serializationToString().c_str(), env))));
		this->_avatarUrl = tmp.avatarUrl;
		this->_nick = tmp.nickornamecard;
	}

	/// 用id构建机器人
	Bot(unsigned long long i): id(i){}

	/// 昵称
	std::string nick() {
		check();
		return this->_nick;
	}

	/// 头像下载链接
	std::string avatarUrl() {
		check();
		return this->_avatarUrl;
	}

	/// 取好友列表
	std::vector<unsigned long long> getFriendList(JNIEnv* env = manager->getEnv()) {
		std::string temp = Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib,
			config->KQueryBFL,
			(jlong)this->id));
		return Tools::StringToVector(temp);
	}

	/// 好友列表string形式返回，利于保存
	std::string FriendListToString() {
		return Tools::VectorToString(getFriendList());
	}

	/// 取群列表
	std::vector<unsigned long long> getGroupList(JNIEnv* env = manager->getEnv()) {
		std::string temp = Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib,
			config->KQueryBGL,
			(jlong)this->id));
		return Tools::StringToVector(temp);
	}

	/// 群列表string形式返回，利于保存
	std::string GroupListToString() {
		return Tools::VectorToString(getGroupList());
	}
};

/// 好友类声明
class Friend : public Contact {
public:
	/*!
	 * @brief 构建好友对象
	 * @param friendid q号
	 * @param botid 对应机器人id
	 */
	Friend(unsigned long long friendid, unsigned long long botid, JNIEnv* = manager->getEnv());

	Friend(Contact c) : Contact(c) {};

	/**
	 * @brief 上传本地图片，务必要用绝对路径.
	 * 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到好友.
	 * 最大支持图片大小为30MB.
	 * @exception invalid_argument代表路径无效
	 * @param filename 文件路径
	 * @return 图片实例
	*/
	Image uploadImg(const std::string& filename, JNIEnv* = manager->getEnv());
};

/// 群成员类声明
class Member : public Contact {
private:
	jmethodID Mute_id = NULL;
	jmethodID KickM = NULL;
public:
	/// @brief 权限等级
	///     - OWNER群主 为 2
	///     - ADMINISTRATOR管理员 为 1
	///     - MEMBER群成员 为 0
	/// @note 上面那些变量在constants.h中有定义
	unsigned int permission = 0;

	/// @brief 构建群成员对象
	/// @param qqid 该成员q号
	/// @param groupid 所在群号
	/// @param botid 机器人id
	/// @example 在事件中构建Member对象
	/// @code
	///  Member(this.sender.id, this.group.id, this.bot.id)
	/// @endcode
	Member(unsigned long long qqid, unsigned long long groupid, unsigned long long botid, JNIEnv* = manager->getEnv());

	/*!
	* @brief上传本地图片，务必要用绝对路径
	* 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
	* @attention 最大支持图片大小为30MB
	* @throws
	 * -可能抛出UploadException异常代表路径无效或大小大于30MB
	 * -可能抛出MemberException找不到群或群成员
	*/
	Image uploadImg(const std::string& filename, JNIEnv* = manager->getEnv());

	Member(Contact c) : Contact(c) {};

	/// 获取权限，会在构造时调用，请使用permission缓存变量
	/// @see Member::permission
	unsigned int getPermission(JNIEnv* = manager->getEnv());

	/*!
	 * 禁言当前对象，单位是秒，最少0秒最大30天
	 * @throws 可能抛出不同错误
	*/
	void Mute(int time, JNIEnv* = manager->getEnv());

	/*! 踢出这个群成员
	* @param reason - 原因
	 * @example 踢出群成员
	 * @code
	 * procession->registerEvent([](GroupMessageEvent e) {
		try {
			Member m = Member(qqid, e.group.id(), e.bot.id());
			m.Kick("this_is_reason");
		}
		catch (BotException err) {
			//权限不足
			logger->Error(err.what());
		}
		catch (MemberException err) {
			if (err.type == 1) {
				//找不到群
			}
			if (err.type == 2) {
				//找不到群成员
			}
		}
		});
	 * @endcode
	*/
	void Kick(const std::string& reason, JNIEnv* = manager->getEnv());
};

/// 群聊类声明
class Group : public Contact {
public:
	/// 取群成员列表-vector<long>
	std::vector<unsigned long long> getMemberList() {
		std::string re = Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib,
			config->KQueryML,
			Tools::str2jstring(
				this->serializationToString().c_str())));
		if (re == "E1") {
			throw GroupException();
		}
		return Tools::StringToVector(re);
	}

	/*!
	 * 以string格式取群成员列表
	 * @note 格式:
	 *  每个群成员id间用逗号分隔
	*/
	std::string MemberListToString() {
		return Tools::VectorToString(getMemberList());
	};

	/// 取群主
	Member getOwner(JNIEnv* = manager->getEnv());

	///  @brief 构建以群号构建群对象
	/// @param groupid 群号
	/// @param botid 机器人id
	/// @example 在事件中构建Group对象
	/// @code Group(this.group.id, this.bot.id) @endcode
	Group(unsigned long long groupid, unsigned long long botid, JNIEnv* = manager->getEnv());

	Group(Contact c) : Contact(c) {};

	/*!
	* @brief 上传本地图片，务必要用绝对路径
	* 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
	* @note 最大支持图片大小为30MB
	* @throw 可能抛出invalid_argument异常代表路径无效
	*/
	Image uploadImg(const std::string& filename, JNIEnv* = manager->getEnv());

	/*!
	@brief 上传并发送远程(群)文件
	@param path-群文件路径(带文件名),根目录为/
	@param filepath-本地文件路径
	@example 上传并发送远程(群)文件
	 @code
	 procession->registerEvent([](GroupMessageEvent e) {
		// 发送D:\\ValveUnhandledExceptionFilter.txt本地文件到qq群的 /test.txt 路径
		RemoteFile tmp = e.group.sendFile("/test.txt", "D:\\ValveUnhandledExceptionFilter.txt");
	    });
	 @endcode
	*/
	RemoteFile sendFile(const std::string& path, const std::string& filepath, JNIEnv* = manager->getEnv());

	/*!
	取群文件信息,会自动搜索子目录
	@param path-群文件路径(不带文件名)
	@param id-文件id,可空，空则为用路径查找(此时路径要带文件名)
	@attention 因为群文件允许重名文件存在的特性，该查找并不可靠，只能返回重名文件中的其中一个文件
	@see RemoteFile
	 @example 取群文件信息
	 @code
	 //根据qq群远程路径(不带文件名)和文件id, 文件id可以在上传返回的RemoteFile类型中获得, 会在子目录中查找如果当前目录未找到
     //根据qq群远程路径(带文件名)找, 不过由于qq群文件允许同名文件这一特性, 返回的文件为群文件中同名文件中随机一个
	 procession->registerEvent([](GroupMessageEvent e) {
		e.group.SendMsg(e.group.getFile("/", id).name());
		e.group.SendMsg(e.group.getFile("/test.txt").name());
		e.group.SendMsg(e.group.getFileListString("/"));
		});
	 @endcode
	*/
	RemoteFile getFile(const std::string& path, const std::string& id = "", JNIEnv* = manager->getEnv());

	struct short_info {
		// 路径带文件名
		std::string path = "";
		// 唯一id
		std::string id = "";
	};

	/*!
	 * 获取path路径下全部文件信息
	 * @param path - 远程路径
	 * @return 返回值为一个vector容器, 每一项为short_info
	*/
	std::vector<short_info> getFileList(const std::string& path, JNIEnv* = manager->getEnv());

	/// 取文件列表以字符串形式返回
	///@example 取string格式群文件列表
	/// @code
    /// e.group.SendMsg(e.group.getFileListString("/"));
	/// @endcode
	std::string getFileListString(const std::string& path, JNIEnv* = manager->getEnv());

	/*!
	* 设置全员禁言
	* @param: sign = true时为开始，false为关闭
	*/
	void setMuteAll(bool sign, JNIEnv* = manager->getEnv());
};

/// At一个群成员
inline std::string At(Member a) {
	/*返回at这个人的miraicode*/
	return "[mirai:at:" + std::to_string(a.id()) + "]";
}

/// At一个群成员
inline std::string At(unsigned long long a) {
	/*返回at这个人的miraicode*/
	return "[mirai:at:" + std::to_string(a) + "]";
}

/// 所以事件处理timeoutevent都是机器人事件，指都有机器人实例
class BotEvent {
public:
	Bot bot;

	BotEvent(unsigned long long botid) : bot(Bot(botid)) {
	}
};

///群消息事件声明
class GroupMessageEvent : public BotEvent {
public:
	///来源群
	Group group;
	///发送人
	Member sender;
	///信息本体
	const std::string message;
	///消息源
	MessageSource messageSource;

	GroupMessageEvent(unsigned long long int botid, const Group& group, const Member& sender,
		const std::string& message, const MessageSource& messageSource) : BotEvent(botid), group(group),
		sender(sender),
		message(message),
		messageSource(messageSource) {}
};

/// 私聊消息事件类声明
class PrivateMessageEvent : public BotEvent {
public:
	/// 发起人
	Friend sender;
	/// 附带消息
	const std::string message;
	/// 信息源
	MessageSource messageSource;

	/*!
	 * @brief 构建私聊信息
	 * @param botid 对应botid
	 * @param sender 发送者
	 * @param message 消息
	 * @param messageSource 消息源
	 */
	PrivateMessageEvent(unsigned long long int botid, const Friend sender, const std::string& message,
		const MessageSource& messageSource) : BotEvent(botid), sender(sender), message(message),
		messageSource(messageSource) {}
};

/// 群聊邀请事件类声明
class GroupInviteEvent : public BotEvent {
public:
	/// 事件序列化文本
	const std::string source;
	/// 发起人昵称
	const std::string inviterNick = "";
	/// 发起人id
	const unsigned long long inviterid = 0;
	/// 被邀请进的组
	const std::string groupName = "";
	/// 群号
	const unsigned long long groupid = 0;

	static void reject(std::string source) {
		std::string re = Tools::jstring2str(
			(jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KGioperation,
				Tools::str2jstring(source.c_str()),
				(jboolean)false));
		if (re == "Y") return;
		if (re == "E")logger->Error("群聊邀请事件同意失败(可能因为重复处理),id:" + source);
	}

	void reject() {
		this->reject(this->source);
	}

	std::string getSource() {
		return this->source;
	}

	static void accept(std::string source) {
		std::string re = Tools::jstring2str(
			(jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KGioperation,
				Tools::str2jstring(source.c_str()),
				(jboolean)true));
		if (re == "Y") return;
		if (re == "E")logger->Error("群聊邀请事件同意失败(可能因为重复处理),id:" + source);
	}

	void accept() {
		this->accept(this->source);
	}

	/*!
	 * @brief 群邀请事件
	 * @param botid 当前botid
	 * @param source 序列化后字符串
	 * @param inviterNick 邀请人昵称
	 * @param inviterid 邀请人id
	 * @param groupName 群聊名称
	 * @param groupid 群号
	 */
	GroupInviteEvent(unsigned long long int botid, const std::string& source, const std::string& inviterNick,
		unsigned long long int inviterid, const std::string& groupName, unsigned long long int groupid)
		: BotEvent(botid), source(source), inviterNick(inviterNick), inviterid(inviterid), groupName(groupName),
		groupid(groupid) {}
};

/// 好友申请事件声明
class NewFriendRequestEvent : public BotEvent {
public:
	/// @brief 序列化的事件信息
	const std::string source;
	/// @brief 对方id
	const unsigned long long fromid;
	const unsigned long long fromgroupid;
	/// @brief 对方昵称
	const std::string nick;
	/// @brief 申请理由
	const std::string message;

	/// @brief 拒绝好友申请
	/// @param source 事件序列化信息
	static void reject(std::string source) {
		std::string re = Tools::jstring2str(
			(jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KNfroperation,
				Tools::str2jstring(source.c_str()),
				(jboolean)false));
		if (re == "Y") return;
		if (re == "E")logger->Error("好友申请事件拒绝失败(可能因为重复处理),id:" + source);
	}

	/// @brief 拒绝好友申请
	void reject() {
		this->reject(this->source);
	}

	/// @brief 接受好友申请
	/// @param source 事件序列化信息
	static void accept(std::string source) {
		std::string re = Tools::jstring2str(
			(jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KNfroperation,
				Tools::str2jstring(source.c_str()),
				(jboolean)true));
		if (re == "Y") return;
		if (re == "E")logger->Error("好友申请事件同意失败(可能因为重复处理),id:" + source);
	}

	/// @brief 接受申请
	void accept() {
		this->accept(this->source);
	}

	/*!
	 * @brief 好友申请事件
	 * @param botid 对应botid
	 * @param source 序列化后信息
	 * @param fromid 对方id
	 * @param fromgroupid 从哪个群申请的，否则为0
	 * @param nick 对方昵称
	 * @param message 申请理由
	 */
	NewFriendRequestEvent(unsigned long long int botid, const std::string& source, const unsigned long long int fromid,
		const unsigned long long int fromgroupid, const std::string& nick, const std::string& message)
		: BotEvent(botid), source(source), fromid(fromid), fromgroupid(fromgroupid), nick(nick), message(message) {}
};

/// 新群成员加入
class MemberJoinEvent : public BotEvent {
public:
	/*!
	* @brief 事件类型
	*   1 - 被邀请进来
	*   2 - 主动加入
	*   3 - 原群主通过 https://huifu.qq.com/ 恢复原来群主身份并入群
	*/
	const int type = 0;
	///新进入的成员
	Member member;
	///目标群
	Group group;
	///邀请人, 当type = 1时存在，否则则和member变量相同
	const unsigned long long inviterid;

	/*!
	 * @brief 新群成员入群事件
	 * @param botid botid
	 * @param type 类别 @see MemberJoinEvent::type
	 * @param member 入群群成员
	 * @param group 群组
	 * @param inviterid 邀请群成员id，如果不存在和member id参数一致
	 */
	MemberJoinEvent(unsigned long long int botid, const int type, const Member& member, const Group& group,
		const unsigned long long& inviterid) : BotEvent(botid), type(type), member(member), group(group),
		inviterid(inviterid) {}
};

/// 群成员离开
class MemberLeaveEvent : public BotEvent {
public:
	/*!
	* @brief 事件类型
	*           1 - 被踢出
	*           2 - 主动退出
	*/
	const int type = 0;
	/// 退出的成员q号
	const unsigned long long memberid;
	/// 目标群
	Group group;
	/// 操作人, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
	const unsigned long long operaterid;

	/*!
	 * @brief 群成员离开
	 * @param botid
	 * @param type
	 * @param memberid 退出的群成员
	 * @param group 群
	 * @param operaterid 操作人id, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
	 */
	MemberLeaveEvent(unsigned long long int botid, const int type, const unsigned long long memberid,
		const Group& group,
		const unsigned long long& operaterid) : BotEvent(botid), type(type), memberid(memberid),
		group(group),
		operaterid(operaterid) {}
};

/// 撤回信息
class RecallEvent : public BotEvent {
public:
	/// 为1时是好友私聊中撤回，为2时为群聊内撤回
	const int type = 0;
	/// 时间戳
	const int time = 0;
	/// 原发送者
	const unsigned long long authorid = 0;
	/// 撤回者
	const unsigned long long operatorid = 0;
	/// 信息id
	const std::string ids = "";
	//内部ids
	const std::string internalids = "";
	//当type是2的时候存在，否则为0
	const unsigned long long groupid = 0;

	/*!
	 * @brief 撤回事件
	 * @param botid 对应bot
	 * @param type 类型
	 * @param time 时间
	 * @param authorid 发送者id
	 * @param operatorid 撤回者id
	 * @param ids 消息源ids
	 * @param internalids 消息源internalids
	 * @param groupid
	 */
	RecallEvent(unsigned long long int botid, const int type, const int time, const unsigned long long int authorid,
		const unsigned long long int operatorid, const std::string& ids, const std::string& internalids,
		const unsigned long long int groupid) : BotEvent(botid), type(type), time(time), authorid(authorid),
		operatorid(operatorid), ids(ids), internalids(internalids),
		groupid(groupid) {}
};

/// 机器人进入某群
class BotJoinGroupEvent : public BotEvent {
public:
	/// 1-主动加入,2-被邀请加入,3-提供恢复群主身份加入
	const int type;
	/// 进入的群
	Group group;
	/// 当type=2时存在，为邀请人，否则为空，调用可能会报错
	const unsigned long long inviterid;

	/*!
	 * @brief bot加入群
	 * @param botid 对应bot
	 * @param type 类别
	 * @param group 加入的群
	 * @param inviter 邀请人
	 */
	BotJoinGroupEvent(unsigned long long int botid, const int type, const Group& group,
		const unsigned long long inviter)
		: BotEvent(botid), type(type), group(group), inviterid(inviterid) {}
};

/// 群临时会话
class GroupTempMessageEvent : public BotEvent {
public:
	/// 来源群
	Group group;
	/// 发送人
	Member sender;
	/// 信息本体
	const std::string message;
	/// 消息源
	MessageSource messageSource;

	/*!
	 * @brief 群临时会话消息事件
	 * @param botid 对应bot
	 * @param group 发起的群
	 * @param sender 发送消息对象
	 * @param message 消息
	 * @param messageSource 消息源
	 */
	GroupTempMessageEvent(unsigned long long int botid, const Group& group, const Member& sender,
		const std::string& message, const MessageSource& messageSource) : BotEvent(botid),
		group(group),
		sender(sender),
		message(message),
		messageSource(
			messageSource) {}
};

/*!
 * @brief 启动定时任务,time是多少毫秒后开始，id是自定义标识符
 * @param time
 * @param args
 * @param e
 * @example 使用定时任务
 * @code
 * //注，需为全局变量
     unsigned long id = 0;
    ...
	procession->registerEvent([=](SchedulingEvent e) {
		//此处的e.id为自定义id，需在调用定时任务时自定义传入
		if (e.id == 1) {
			//do something
        Friend(id).SendMsg("这是定时任务");
        }
        });
        //这里的[&]和[=]分别为以引用和值的方式捕获全部可以捕获的变量，前者可以改变量值，相当于指针，后者无法更改，相当于本地一个局部变量并进行值传递，详细见搜索引擎的c++ lambda
        procession->registerEvent([&](PrivateMessageEvent param)->void {
        //在这写你自己处理私聊消息的代码
        logger->Info("hi");
        //延迟100ms发送，后面的1为自定义id
        SetScheduling(100, 1);
        id = param.sender.id();
        param.sender.SendMsg(param.message);
        });
        ...
 * @endcode
 */
inline void SetScheduling(long time, std::initializer_list<std::string> args, BotEvent* e) {
	nlohmann::json obj;
	nlohmann::json root;
	for (const std::string& it : args) {
		obj.push_back(it);
	}
	root["value"] = obj;
	manager->getEnv()->CallStaticVoidMethod(config->CPP_lib, config->KSchedule, (jlong)time,
		Tools::str2jstring(root.dump().c_str()));
}

/// 定时任务执行
class SchedulingEvent {
public:
	void init() {};
	/*自定义id标识符*/
	std::vector<std::string> ids;

	SchedulingEvent(const std::string& str);
};


/**监听类声明*/
class Event {
private:
	class Node0 {
	public:
		Node0* nextNode = nullptr;
        bool enable = true;
	};

    template <class T>
    class Node: public Node0{
    public:
        std::function<void(T)> f = [](T) -> void{};
        Node* next = nullptr;
    };

	Node<GroupMessageEvent>* GMHead = new Node<GroupMessageEvent>();
	Node<PrivateMessageEvent>* PMHead = new Node<PrivateMessageEvent>();
	Node<GroupInviteEvent>* GHead = new Node<GroupInviteEvent>;
	Node<NewFriendRequestEvent>* NFHead = new Node<NewFriendRequestEvent>();
	Node<MemberJoinEvent>* MJHead = new Node<MemberJoinEvent>();
	Node<MemberLeaveEvent>* MLHead = new Node<MemberLeaveEvent>();
	Node<RecallEvent>* RHead = new Node<RecallEvent>();
	Node<SchedulingEvent>* SHead = new Node<SchedulingEvent>();
	Node<BotJoinGroupEvent>* BHead = new Node<BotJoinGroupEvent>();
	Node<GroupTempMessageEvent>* GTMHead = new Node<GroupTempMessageEvent>();

	/// 取链表首节点
	template <class T>
    Node<T> *head(){
        if constexpr(std::is_same_v<T, GroupMessageEvent>){
            return GMHead;
        }else if constexpr(std::is_same_v<T, PrivateMessageEvent>){
            return PMHead;
        }else if constexpr(std::is_same_v<T, GroupInviteEvent>){
            return GHead;
        }else if constexpr(std::is_same_v<T, NewFriendRequestEvent>){
            return NFHead;
        }else if constexpr(std::is_same_v<T, MemberJoinEvent>){
            return MJHead;
        }else if constexpr(std::is_same_v<T, MemberLeaveEvent>){
            return MLHead;
        }else if constexpr(std::is_same_v<T, RecallEvent>){
            return RHead;
        }else if constexpr(std::is_same_v<T, SchedulingEvent>){
            return SHead;
        }else if constexpr(std::is_same_v<T, BotJoinGroupEvent>){
            return BHead;
        }else if constexpr(std::is_same_v<T, GroupTempMessageEvent>){
            return GTMHead;
        }
        logger->Error("内部错误, 位置:C-Head");
        return nullptr;
    }

    Node<GroupMessageEvent>* GMTail = GMHead;
    Node<PrivateMessageEvent>* PMTail = PMHead;
    Node<GroupInviteEvent>* GTail = GHead;
    Node<NewFriendRequestEvent>* NFTail = NFHead;
    Node<MemberJoinEvent>* MJTail = MJHead;
    Node<MemberLeaveEvent>* MLTail = MLHead;
    Node<RecallEvent>* RTail = RHead;
    Node<SchedulingEvent>* STail = SHead;
    Node<BotJoinGroupEvent>* BTail = BHead;
    Node<GroupTempMessageEvent>* GTMTail = GTMHead;

    /// 取链表尾节点
    template <class T>
    bool* tail(std::function<void(T)> f){
        Node<T>* temp = new Node<T>();
        temp->f = f;
        if constexpr(std::is_same_v<T, GroupMessageEvent>){
            GMTail->next = temp;
            GMTail->nextNode = temp;
            GMTail = temp;
        }else if constexpr(std::is_same_v<T, PrivateMessageEvent>){
            PMTail->next = temp;
            PMTail->nextNode = temp;
            PMTail = temp;
        }else if constexpr(std::is_same_v<T, GroupInviteEvent>){
            GTail->next = temp;
            GTail->nextNode = temp;
            GTail = temp;
        }else if constexpr(std::is_same_v<T, NewFriendRequestEvent>){
            NFTail->next = temp;
            NFTail->nextNode = temp;
            NFTail = temp;
        }else if constexpr(std::is_same_v<T, MemberJoinEvent>){
            MJTail->next = temp;
            MJTail->nextNode = temp;
            MJTail = temp;
        }else if constexpr(std::is_same_v<T, MemberLeaveEvent>){
            MLTail->next = temp;
            MLTail->nextNode = temp;
            MLTail = temp;
        }else if constexpr(std::is_same_v<T, RecallEvent>){
            RTail->next = temp;
            RTail->nextNode = temp;
            RTail = temp;
        }else if constexpr(std::is_same_v<T, SchedulingEvent>){
            STail->next = temp;
            STail->nextNode = temp;
            STail = temp;
        }else if constexpr(std::is_same_v<T, BotJoinGroupEvent>){
            BTail->next = temp;
            BTail->nextNode = temp;
            BTail = temp;
        }else if constexpr(std::is_same_v<T, GroupTempMessageEvent>){
            GTMTail->next = temp;
            GTMTail->nextNode = temp;
            GTMTail = temp;
        }else {
            logger->Error("内部错误, 位置:C-Tail");
            return nullptr;
        }
        return &temp->enable;
    }

public:
    /// 事件监听操控, 可用于stop停止监听和resume继续监听
	class NodeHandle {
	private:
		bool* enable;
	public:
		NodeHandle(bool* a) {
			this->enable = a;
		}

		void stop() const {
			*enable = false;
		}

		void resume() const {
			*enable = true;
		}
	};

	/*!
	* 广播函数重载
	*/
	template <class T>
	void broadcast(T e){
        Node<T>* now = Event::head<T>();
        while(now){
            if(now->enable){now->f(e);}
            now = now->next;
        }
	}

	/*!
	* @brief 监听函数
	* @note 在极其少见(MiraiCP内部出问题的时候)会返回nullptr
	*/
	template<class T>
    NodeHandle registerEvent(std::function<void(T)> f){
        bool* e = Event::tail(f);
        if(e != nullptr)
            return NodeHandle(e);
        return nullptr;
    }

	~Event();
};

/*声明全局监听对象(广播源)*/
extern Event* procession;

/*声明插件启用和结束函数*/
void onDisable();

void onEnable();