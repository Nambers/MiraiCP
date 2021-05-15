#include "pch.h"

/// @brief 全局JavaVM对象，用于多线程管理中新建线程的JNIEnv
extern JavaVM* gvm;
/// @brief JNI 版本
extern int JNIVersion;
/// @brief 多线程管理
class threadManager {
private:
    /// @brief 每个线程实例
	struct t {
		JNIEnv* e;
		bool attach;
	};
	std::map <std::string, t> _threads;/// < 线程池(线程id:env)
	std::recursive_mutex mtx; ///< 线程池读写锁
	void newEnv(const char* threadName = NULL);///< 新建一个env，于getEnv中没取到env时调用
	bool included(const std::string& id);
public:
	/// @brief 获取线程id
	static std::string getThreadId(){
		std::ostringstream oss;
		oss << std::this_thread::get_id();
		return oss.str();
	}
	threadManager();
	/// @brief 设置env给当前线程
	void setEnv(JNIEnv* e);
	/*!
	 * 	@brief 结束当前线程的env，也就是释放当前线程缓存的env
	 *  @note 不过继续调用getEnv()将再次获取，所以本方法调用后线程也可以通过getEnv重新获取一个env，本线程的作用就是在结束后释放空间
	 */
	void detach();
	JNIEnv* getEnv();///< @brief 取env,如果不存在重新获取
};
extern threadManager* manager;

/// @brief 日志
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
    typedef std::function<void (std::string, int)> action;
    /// @brief loggerhandler会在每次log执行前执行一遍，可用于执行自定义的保存操作等
    struct handler{
        /// @brief 是否启用
        bool enable = true;
        /// @brief 执行的操作，格式为lambda
        Logger::action action = [](std::string content, int level){};
    }loggerhandler;
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
    inline void registerHandle(action action);
    /// @brief 设置handler的启用状态
    /// @param state 状态，启用或者关闭
    inline void setHandleState(bool state);
	~Logger();
};

/*声明全局日志对象*/
extern Logger* logger;

/// 配置类声明，主要存放各种jmethodid
class Config {
public:
	// kt中JNI接口类
	jclass CPP_lib = NULL;
	// 异常类
	jclass initexception = NULL;
	// 撤回信息
	jmethodID KRecall = NULL;
	// 发送信息
	jmethodID KSend = NULL;
	// 查询信息接口
	jmethodID KRefreshInfo = NULL;
	// 上传图片
	jmethodID KUploadImg = NULL;
	// 取好友列表
	jmethodID KQueryBFL = NULL;
	// 取群组列表
	jmethodID KQueryBGL= NULL;
	// 上传文件
	jmethodID KSendFile = NULL;
	// 查询文件信息
	jmethodID KRemoteFileInfo = NULL;
	// 查询图片下载地址
	jmethodID KQueryImgUrl = NULL;
	// 禁言
	jmethodID KMuteM = NULL;
	// 查询权限
	jmethodID KQueryM = NULL;
	// 踢出
	jmethodID KKickM = NULL;
	// 取群主
	jmethodID KQueryOwner = NULL;
	// 全员禁言
	jmethodID KMuteGroup = NULL;
	// 查询群成员列表
	jmethodID KQueryML = NULL;
	/*定时任务*/
	jmethodID KSchedule = NULL;
	// 构建转发信息
	jmethodID KBuildforward = NULL;
	// 好友申请事件
	jmethodID KNfroperation = NULL;
	// 群聊邀请事件
	jmethodID KGioperation = NULL;
	Config() {};
	void Init(JNIEnv* = manager->getEnv());
	~Config();
};

extern Config* config;

/*工具类声明*/
class Tools {
public:
	/*!
	 * @name jstring2str
	 * @brief string类型转码转换到jstring类型
	 * @attention 因为java和cpp的字符编码不一样，所以要转码
	 * @note 来源https://blog.csdn.net/chunleixiahe/article/details/51394116
	 * @param jstr 转换内容,jstring类型
	 * @param env 可选，JNIEnv
	 * @return 内容转换成jstring类型
	 */
	static std::string jstring2str(jstring jstr, JNIEnv* = manager->getEnv());
	/*!
	 * @name str2jstring
	 * @brief string类型到jsting类型
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
	 * @brief 替换全部
	 * @param str 原字符串
	 * @param from 需要被替换的字符
	 * @param to 替换到的字符
	 * @return 返回替换后的字符串
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
	/*!
	 * @brief 根据Json::Value生成string
	 * @param root Json::Value类型
	 * @attention 本方法转换出的字符串没有美化格式
	 * @attention 如果自己生成需要注意，结果需要转码
	 * @return
	 * @note 来源:https://www.cnblogs.com/zhangdongsheng/p/12731021.html
	 */
	static std::string JsonToString(const Json::Value& root);
	/*!
	 * @brief 从string格式化json
	 * @param source 原字符串
	 * @return Json::Value类型
	 */
	static Json::Value StringToJson(const std::string& source);
	/// @brief long long 类型的vector格式化输出
	/// @param v vector
	/// @return string
	static std::string VectorToString(std::vector<unsigned long long> v);
	/// @brief 从string格式化到vector
	/// @param s string
	/// @return vector
	static std::vector<unsigned long long> StringToVector(std::string s);
};

///总异常
class MiraiCPException :public std::exception {
public:
	virtual std::string what() { return "C++部分出现了个未捕获的异常"; };
	virtual void raise() {};
};
/// 初始化异常
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
/// 文件读取异常
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
/// 内部异常
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
/// 机器人操作异常
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
/// 禁言异常
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
/// 获取群成员错误
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
/// 获取群成员错误
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
/// 获取群错误
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
/// 撤回异常
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
/// 上传异常
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
	std::string toString(){
	    return this->source;
	}
    MessageSource(std::string ids, std::string internalids, std::string source);
    /// @brief 构造个消息源，可用于撤回，未来应该可以用于引用
	/// @param source 序列化文本
	/// @param root json节点
	MessageSource(std::string source, Json::Value root):source(source), ids(Tools::replace(Tools::replace(root["ids"].toStyledString(), " ", ""), "\n", "")), internalids(Tools::replace(Tools::replace(root["internalIds"].toStyledString(), " ", ""), "\n", "")){};
	/*!
	 * @breif 从json字符串反序列化到MessageSource对象
	 * @note json应该为以下格式
	 * @code
	 * {"ids":"", "internalids":""}
	 * @endcode
	 */
	static MessageSource deserializeFromString(const std::string& source);
	std::string serializeToString();
	/// @brief 撤回该信息
	void recall();
};

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
	std::string toMiraiCode(){
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
    MiraiCode operator+=(MiraiCodeable* a){
        return MiraiCode(this->content + a->toMiraiCode());
    }
    MiraiCode operator+=(std::string a){
        return MiraiCode(this->content + a);
    }
    MiraiCode operator=(MiraiCodeable* a){
        return MiraiCode(a->toMiraiCode());
    }
    MiraiCode operator=(std::string a){
        return MiraiCode(a);
    }
	MiraiCode plus(MiraiCodeable* a) {
		return MiraiCode(content + a->toMiraiCode());
	}
	MiraiCode plus(std::string a) {
		return MiraiCode(content + a);
	}
};

/// 小程序
class LightApp: public MiraiCodeable {
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
		this->content = "{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[应用]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":"
			"{\"appName\":\"" + c.appName + "\",\"appType\":4,\"appid\":1109659848,"
			"\"iconUrl\":\"" + c.icon + "\"},"
			"\"data\":[" + c.titles + "],"
			"\"title\":\"" + c.title + "\",\"button\":"
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
			"\"meta\":{\"detail_1\":{\"appid\":\"1110081493\",\"preview\":\"" + c.preview + "\",\"shareTemplateData\":{},"
			"\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" + c.title2 + "\",\"title\":\"" + c.title + "\","
			"\"host\":{\"uin\":0,\"nick\":\"\"},"
			"\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" + c.icon + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
	}
	/// 样式3，有大图，可以在电脑qq显示，并在电脑上点击的链接会跳转
    /// @param c 结构体，用于自定义里面的数据
    /// @see LightAppStyle1 in constants.h
	LightApp(LightAppStyle3 c) {
		this->content = "{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\"" + c.preview + "\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" + c.title + "\",\"title\":\"" + c.description + "\","
			"\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" + c.icon + "\",\"qqdocurl\":\"" + c.url + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
	}
	std::string toMiraiCode() {
		return "[mirai:app:" + Tools::replace(Tools::replace(content, "[", "\\["), "]", "\\]") + "]";
	}
};

/*图像类声明*/
class Image: public MiraiCodeable {
public:
    const jmethodID Query = NULL;
	//图片id，样式:` {xxx}.xx `
	std::string id = "";
	/*!
	* @brief 从图片id构造，适用于服务器上已经有的图片，即接收到的
	* @example 图片miraiCode例子: [mirai:image:{图片id}.jpg]
	* @note 可以用这个正则表达式找出id ` \\[mirai:image:(.*?)\\] `
	*/
	Image(std::string);
	/*
	* 获取图片下载url
	*/
	std::string queryURL(JNIEnv* = manager->getEnv()) const;

	/*! @brief 取一个miraicode字符串中全部的图片id，详情见Image
	 *   @param  miraicode的字符串
	 *   @return vector容器，每一项为一个图片id
 	 *   @example
 	 *   @code
	 *          vector<string> temp = Image::GetImgIdFromMiraiCode(param.message);
     *	        for (int i = 0; i < temp.size(); i++) {
     *	    	    logger->Info(temp[i]);
     *	    	    logger->Info("图片下载地址:" + Image(param.env, temp[i]).queryURL());
   	 *	         }
   	 *	  @endcode
     */
	static std::vector<std::string> GetImgIdsFromMiraiCode(std::string);

	/// 取图片Mirai码
	std::string toMiraiCode();
};

/// group, friend, member的父类
class Contact {
protected:
	int _type = 0;
	unsigned long long _id;
	unsigned long long _groupid;
	std::string _nickOrNameCard;
	std::string _avatarUrl;
	unsigned long long _botid;
public:
	Contact() {
		this->_type = 0;
		this->_id = 0;
		this->_groupid = 0;
		this->_nickOrNameCard = "";
		this->_botid = 0;
	}
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
	int type() {return this->_type;}
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
    /// 反序列化成bot，可以通过serializationToString序列化，利于保存
    /// @see Contact::serializationToString()
    /// @param source 序列化后的文本
    /// @throw APIException
	static Contact deserializationFromString(const std::string &source);
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
    /// 文件唯一id, 用于试别
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
	/// 由完整信息构造
	RemoteFile(std::string i, unsigned int ii, std::string n, long long s, struct dinfo d, struct finfo f):id(i),internalid(ii),name(n),size(s),dinfo(d),
                                                                                             finfo(f){};
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
		return Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KSend, Tools::str2jstring(j.dump().c_str(),env), (jboolean)miraicode), env);
	}
	/// @brief 取该联系人的一些信息
	/// @param c 该联系人Contact指针
	/// @return json格式字符串，待解析
	static inline std::string getInfoSource(Contact* c, JNIEnv* env = manager->getEnv()) {
		return Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KRefreshInfo, Tools::str2jstring(c->serializationToString().c_str(), env)));
	}
	/*!
	 * @brief 上传图片
	 * @param path 本地地址
	 * @param c 上传的对象
	 * @param env JNIEnv
	 * @return string 待解析json
	 */
	static inline std::string uploadImg0(std::string path, Contact* c, JNIEnv* env = manager->getEnv()) {
		return Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KUploadImg,Tools::str2jstring(path.c_str(), env), Tools::str2jstring(c->serializationToString().c_str(), env)));
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
		Json::Value tmp = Tools::StringToJson(source);
		re.avatarUrl = tmp["avatarUrl"].asCString();
		re.nickornamecard = tmp["nickornamecard"].asCString();
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
    ForwardNode(const unsigned long long int id, const std::string &name, const std::string &message, const int time)
            : id(id), name(name), message(message), time(time) {}
	/// @brief 构造聊天记录里每条信息
	/// @param c - 发送者的contact指针
	/// @prama m - 发送的信息
	/// @param t - 发送时间，时间戳格式
	ForwardNode(Contact* c, std::string &message, int t): id(c->id()), name(c->nickOrNameCard()), message(message), time(t)
	{}
};

///聊天记录, 由ForwardNode组成
/// @see class ForwardNode
class ForwardMessage {
public:
	Json::Value sendmsg;
	/*!
	*@brief 构建一条聊天记录
	*@details 第一个参数是聊天记录发生的地方
	* 然后是每条信息
	*@example@code
	*ForwardMessage(&e.group,
	*{
	*	ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
	*	ForwardNode(1930893235, "Eritque arcus", "hahaha", -1)
	*}).sendTo(&e.group);
	* @endcode
	*/
	ForwardMessage(Contact* c, std::initializer_list<ForwardNode> nodes);
	/// 发送给群或好友或群成员
	void sendTo(Contact* c, JNIEnv* = manager->getEnv()) const;
};

/// 当前bot账号信息
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
    /*!
     * @brief 刷新bot信息
     * @param env
     */
	void refreshInfo(JNIEnv* env = manager->getEnv()) {
		LowLevelAPI::info tmp = LowLevelAPI::info0(Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KRefreshInfo, Tools::str2jstring(Contact(4, 0, 0, "", this->_id).serializationToString().c_str(), env))));
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
			(jlong)this->id()));
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
			(jlong)this->id()));
		return Tools::StringToVector(temp);
	}
    /// 群列表string形式返回，利于保存
	std::string GroupListToString() {
		return Tools::VectorToString(getGroupList());
	}
};

/// 好友类声明
class Friend:public Contact{
public:
	Friend(unsigned long long friendid, unsigned long long botid, JNIEnv* =manager->getEnv());
	Friend():Contact(){};
	
	/** 
	 * @brief 上传本地图片，务必要用绝对路径. 
	 * 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到好友. 
	 * 最大支持图片大小为30MB.
	 * @exception invalid_argument代表路径无效
	 * @param filename 文件路径
	 * @return 图片实例
	*/
	Image uploadImg(const std::string& filename, JNIEnv* = manager->getEnv());

	/*发送信息*/
	/**
	 * @brief 发送miraicode
	 * @param msg - MiraiCodeable类型指针 - 内容
	*/
	MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(std::string msg, JNIEnv* = manager->getEnv());
	/// 发送纯文本信息
	MessageSource SendMsg(std::string msg, JNIEnv* = manager->getEnv());
};

/// 群成员类声明
class Member :public Contact {
private:
	jmethodID Mute_id = NULL;
	jmethodID Query_permission = NULL;
	jmethodID KickM = NULL;
public:
	/// @brief 权限等级
	///     - OWNER群主 为 2
	///     - ADMINISTRATOR管理员 为 1
	///     - MEMBER群成员 为 0
	/// @note 上面那些变量在constants.h中有定义
	unsigned int permission = 0;
	/// qqid, groupid
	Member(unsigned long long qqid, unsigned long long groupid, unsigned long long botid, JNIEnv* = manager->getEnv());
	/*
	* 上传本地图片，务必要用绝对路径
	* 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
	* 最大支持图片大小为30MB
	* 可能抛出invalid_argument异常代表路径无效
	*/
	Image uploadImg(const std::string& filename, JNIEnv* = manager->getEnv());
	Member():Contact() {};
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
	void Kick(const std::string& reason, JNIEnv* = manager->getEnv());
};

/// 群聊类声明
class Group :public Contact {
public:
	/*取群成员列表-vector<long>*/
	std::vector<unsigned long long> getMemberList() {
		std::string re = Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib,
			config->KQueryML,
			Tools::str2jstring(this->serializationToString().c_str())));
		if (re == "E1") {
			throw GroupException();
		}
		return Tools::StringToVector(re);
	}
	/*以string格式取群成员列表
	格式：
		每个群成员id间用逗号分隔
	*/
	std::string MemberListToString() {
		return Tools::VectorToString(getMemberList());
	};
	//取群主
	Member getOwner(JNIEnv* = manager->getEnv());
	//构建以群号构建群对象
	Group(unsigned long long groupid, unsigned long long botid, JNIEnv* = manager->getEnv());
	Group(Contact c):Contact(c);
	/*
	* 上传本地图片，务必要用绝对路径
	* 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
	* 最大支持图片大小为30MB
	* 可能抛出invalid_argument异常代表路径无效
	*/
	Image uploadImg(const std::string& filename, JNIEnv* = manager->getEnv());
	/*
	上传并发送
	path-群文件路径(带文件名),根目录为/
	filepath-本地文件路径
	如 group.uploadFIle("/test.txt", "D:\\xxxx.xxx")
	*/
	RemoteFile sendFile(const std::string& path, const std::string& filepath, JNIEnv* = manager->getEnv());
	/* 
	取群文件信息,会自动搜索子目录
	path-群文件路径(不带文件名)
	id-文件id,可空，空则为用路径查找(此时路径要带文件名)
	因为群文件允许重名文件存在的特性，该查找并不可靠，只能返回重名文件中的其中一个文件
	*/
	RemoteFile getFile(const std::string& path, const std::string& id = "", JNIEnv* = manager->getEnv());
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
	std::vector<short_info> getFileList(const std::string& path, JNIEnv* = manager->getEnv());
	// 取文件列表返回值是字符串
	std::string getFileListString(const std::string& path, JNIEnv* = manager->getEnv());
	Group():Contact() {};
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
	 const Bot bot;
	 BotEvent(unsigned long long botid):bot(Bot(botid)) {
	 }
 };

///群消息事件声明
class GroupMessageEvent:public BotEvent {
public:
	///来源群
	const Group group;
	///发送人
	const Member sender;
	///信息本体
	const std::string message;
	///消息源
	const MessageSource messageSource;
    GroupMessageEvent(unsigned long long int botid, const Group &group, const Member &sender,
                      const std::string &message, const MessageSource &messageSource) : BotEvent(botid), group(group),
                                                                                        sender(sender),
                                                                                        message(message),
                                                                                        messageSource(messageSource) {}
};

/// 私聊消息事件类声明
class PrivateMessageEvent : public BotEvent {
public:
	/// 发起人
	const Friend sender;
	/// 附带消息
	const std::string message;
	/// 信息源
	const MessageSource messageSource;
    PrivateMessageEvent(unsigned long long int botid, const Friend &sender, const std::string &message,
                        const MessageSource &messageSource) : BotEvent(botid), sender(sender), message(message),
                                                              messageSource(messageSource) {}
};

/// 群聊邀请事件类声明
class GroupInviteEvent: public BotEvent {
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
		std::string re = Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KGioperation, Tools::str2jstring(source.c_str()), (jboolean)false));
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
		std::string re = Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KGioperation, Tools::str2jstring(source.c_str()), (jboolean)true));
		if (re == "Y") return;
		if (re == "E")logger->Error("群聊邀请事件同意失败,id:" + source);
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
    GroupInviteEvent(unsigned long long int botid, const std::string &source, const std::string &inviterNick,
                     unsigned long long int inviterid, const std::string &groupName, unsigned long long int groupid)
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
		std::string re = Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KNfroperation, Tools::str2jstring(source.c_str()), (jboolean)false));
		if (re == "Y") return;
		if (re == "E")logger->Error("好友申请事件同意失败,id:" + source);
	}
	/// @brief 拒绝好友申请
	void reject() {
		this->reject(this->source);
	}
	/// @brief 接受好友申请
	/// @param source 事件序列化信息
	static void accept(std::string source) {
		std::string re = Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KNfroperation, Tools::str2jstring(source.c_str()), (jboolean)true));
		if (re == "Y") return;
		if (re == "E")logger->Error("好友申请事件同意失败,id:" + source);
	}
	/// @brief 接受申请
	void accept() {
		this->accept(this->source);
	}
    NewFriendRequestEvent(unsigned long long int botid, const std::string &source, const unsigned long long int fromid,
                          const unsigned long long int fromgroupid, const std::string &nick, const std::string &message)
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
	const Member member;
	///目标群
	const Group group;
	///邀请人, 当type = 1时存在，否则则和member变量相同
	const Member inviter;
    /*!
     * @brief 新群成员入群事件
     * @param botid botid
     * @param type 类别 @see MemberJoinEvent::type
     * @param member 入群群成员
     * @param group 群组
     * @param inviter 邀请群成员，如果不存在和member参数一致
     */
    MemberJoinEvent(unsigned long long int botid, const int type, const Member &member, const Group &group,
                    const Member &inviter) : BotEvent(botid), type(type), member(member), group(group),
                                             inviter(inviter) {}
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
	const Group group;
	/// 操作人, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
	const Member operater;
    /*!
     * @brief
     * @param botid
     * @param type
     * @param member 退出的群成员
     * @param group 群
     * @param operater 操作人, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
     */
    MemberLeaveEvent(unsigned long long int botid, const int type, const unsigned long long memberid, const Group &group,
                     const Member &operater) : BotEvent(botid), type(type), memberid(memberid), group(group),
                                               operater(operater) {}
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
                const unsigned long long int operatorid, const std::string &ids, const std::string &internalids,
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
	const Group group;
	/// 当type=2时存在，为邀请人，否则为空，调用可能会报错
	const Member inviter;
    /*!
     * @brief bot加入群
     * @param botid 对应bot
     * @param type 类别
     * @param group 加入的群
     * @param inviter 邀请人
     */
    BotJoinGroupEvent(unsigned long long int botid, const int type, const Group &group, const Member &inviter)
            : BotEvent(botid), type(type), group(group), inviter(inviter) {}
};

/// 群临时会话
class GroupTempMessageEvent : public BotEvent {
public:
	/// 来源群
	const Group group;
	/// 发送人
	const Member sender;
	/// 信息本体
	const std::string message;
	/// 消息源
	const MessageSource messageSource;
    /*!
     * @brief 群临时会话消息事件
     * @param botid 对应bot
     * @param group 发起的群
     * @param sender 发送消息对象
     * @param message 消息
     * @param messageSource 消息源
     */
    GroupTempMessageEvent(unsigned long long int botid, const Group &group, const Member &sender,
                          const std::string &message, const MessageSource &messageSource) : BotEvent(botid),
                                                                                            group(group),
                                                                                            sender(sender),
                                                                                            message(message),
                                                                                            messageSource(
                                                                                                    messageSource) {}
};

/// 启动定时任务,time是多少毫秒后开始，id是自定义标识符
inline void SetScheduling(long time, std::initializer_list<std::string> args, BotEvent* e) {
	Json::Value obj;
	Json::Value root;
	for (const std::string& it : args) {
		obj.append(it);
	}
	root["value"] = obj;
	manager->getEnv()->CallStaticVoidMethod(config->CPP_lib, config->KSchedule, (jlong)time, Tools::str2jstring(Tools::JsonToString(root).c_str()));
}

/*定时任务执行*/
class SchedulingEvent{
public:
	void init() {};
	/*自定义id标识符*/
	std::vector<std::string> ids;
	SchedulingEvent(const std::string& str);
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