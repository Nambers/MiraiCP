#include "pch.h"

/// @brief ȫ��JavaVM�������ڶ��̹߳������½��̵߳�JNIEnv
extern JavaVM* gvm;
/// @brief JNI �汾
extern int JNIVersion;
/// @brief ���̹߳���
class threadManager {
private:
    /// @brief ÿ���߳�ʵ��
	struct t {
		JNIEnv* e;
		bool attach;
	};
	std::map <std::string, t> _threads;/// < �̳߳�(�߳�id:env)
	std::recursive_mutex mtx; ///< �̳߳ض�д��
	void newEnv(const char* threadName = NULL);///< �½�һ��env����getEnv��ûȡ��envʱ����
	bool included(const std::string& id);
public:
	/// @brief ��ȡ�߳�id
	static std::string getThreadId(){
		std::ostringstream oss;
		oss << std::this_thread::get_id();
		return oss.str();
	}
	threadManager();
	/// @brief ����env����ǰ�߳�
	void setEnv(JNIEnv* e);
	/*!
	 * 	@brief ������ǰ�̵߳�env��Ҳ�����ͷŵ�ǰ�̻߳����env
	 *  @note ������������getEnv()���ٴλ�ȡ�����Ա��������ú��߳�Ҳ����ͨ��getEnv���»�ȡһ��env�����̵߳����þ����ڽ������ͷſռ�
	 */
	void detach();
	JNIEnv* getEnv();///< @brief ȡenv,������������»�ȡ
};
extern threadManager* manager;

/// @brief ��־
class Logger {
private:
	jclass CPP_lib = NULL;
	jmethodID log = nullptr;
	/// ��־�ײ�ʵ�ַ�װ
	void log0(const std::string&, JNIEnv*, int);
public:
    /// @brief ��װlambda����
    /// @param string ��־����
    /// @param ��־����
    ///     - 0 info
    ///     - 1 warning
    ///     - 2 error
    typedef std::function<void (std::string, int)> action;
    /// @brief loggerhandler����ÿ��logִ��ǰִ��һ�飬������ִ���Զ���ı��������
    struct handler{
        /// @brief �Ƿ�����
        bool enable = true;
        /// @brief ִ�еĲ�������ʽΪlambda
        Logger::action action = [](std::string content, int level){};
    }loggerhandler;
	void init(JNIEnv* = manager->getEnv());
    ///������ͨ(info����־)
	void Info(const std::string&, JNIEnv* = manager->getEnv());
	///���;���(warning����־)
	void Warning(const std::string&, JNIEnv* = manager->getEnv());
	///���ʹ���(error����־)
	void Error(const std::string&, JNIEnv* = manager->getEnv());
	/// @brief ����loggerhandler��action
	/// @param action ִ�еĲ���
	/// @see Logger::handler
    inline void registerHandle(action action);
    /// @brief ����handler������״̬
    /// @param state ״̬�����û��߹ر�
    inline void setHandleState(bool state);
	~Logger();
};

/*����ȫ����־����*/
extern Logger* logger;

/// ��������������Ҫ��Ÿ���jmethodid
class Config {
public:
	// kt��JNI�ӿ���
	jclass CPP_lib = NULL;
	// �쳣��
	jclass initexception = NULL;
	// ������Ϣ
	jmethodID KRecall = NULL;
	// ������Ϣ
	jmethodID KSend = NULL;
	// ��ѯ��Ϣ�ӿ�
	jmethodID KRefreshInfo = NULL;
	// �ϴ�ͼƬ
	jmethodID KUploadImg = NULL;
	// ȡ�����б�
	jmethodID KQueryBFL = NULL;
	// ȡȺ���б�
	jmethodID KQueryBGL= NULL;
	// �ϴ��ļ�
	jmethodID KSendFile = NULL;
	// ��ѯ�ļ���Ϣ
	jmethodID KRemoteFileInfo = NULL;
	// ��ѯͼƬ���ص�ַ
	jmethodID KQueryImgUrl = NULL;
	// ����
	jmethodID KMuteM = NULL;
	// ��ѯȨ��
	jmethodID KQueryM = NULL;
	// �߳�
	jmethodID KKickM = NULL;
	// ȡȺ��
	jmethodID KQueryOwner = NULL;
	// ȫԱ����
	jmethodID KMuteGroup = NULL;
	// ��ѯȺ��Ա�б�
	jmethodID KQueryML = NULL;
	/*��ʱ����*/
	jmethodID KSchedule = NULL;
	// ����ת����Ϣ
	jmethodID KBuildforward = NULL;
	// ���������¼�
	jmethodID KNfroperation = NULL;
	// Ⱥ�������¼�
	jmethodID KGioperation = NULL;
	Config() {};
	void Init(JNIEnv* = manager->getEnv());
	~Config();
};

extern Config* config;

/*����������*/
class Tools {
public:
	/*!
	 * @name jstring2str
	 * @brief string����ת��ת����jstring����
	 * @attention ��Ϊjava��cpp���ַ����벻һ��������Ҫת��
	 * @note ��Դhttps://blog.csdn.net/chunleixiahe/article/details/51394116
	 * @param jstr ת������,jstring����
	 * @param env ��ѡ��JNIEnv
	 * @return ����ת����jstring����
	 */
	static std::string jstring2str(jstring jstr, JNIEnv* = manager->getEnv());
	/*!
	 * @name str2jstring
	 * @brief string���͵�jsting����
	 * @note ��Դhttps://blog.csdn.net/chunleixiahe/article/details/51394116
	 * @param pat const char*(string.c_str())ת��������
	 * @param env ��ѡJNIEnv
	 * @return ת����jstring����
	 */
	static jstring str2jstring(const char* pat, JNIEnv* = manager->getEnv());
	/*!
	 * @name JLongToString
	 * @brief jlong���͵�string����
	 * @param val ��Ҫת������ֵ
	 * @return ת����string����
	 */
	static std::string JLongToString(jlong val);
	/*!
	 * @brief �滻ȫ��
	 * @param str ԭ�ַ���
	 * @param from ��Ҫ���滻���ַ�
	 * @param to �滻�����ַ�
	 * @return �����滻����ַ���
	 * @note ��Դ:https://stackoverflow.com/a/24315631/14646226
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
	 * @brief ����Json::Value����string
	 * @param root Json::Value����
	 * @attention ������ת�������ַ���û��������ʽ
	 * @attention ����Լ�������Ҫע�⣬�����Ҫת��
	 * @return
	 * @note ��Դ:https://www.cnblogs.com/zhangdongsheng/p/12731021.html
	 */
	static std::string JsonToString(const Json::Value& root);
	/*!
	 * @brief ��string��ʽ��json
	 * @param source ԭ�ַ���
	 * @return Json::Value����
	 */
	static Json::Value StringToJson(const std::string& source);
	/// @brief long long ���͵�vector��ʽ�����
	/// @param v vector
	/// @return string
	static std::string VectorToString(std::vector<unsigned long long> v);
	/// @brief ��string��ʽ����vector
	/// @param s string
	/// @return vector
	static std::vector<unsigned long long> StringToVector(std::string s);
};

///���쳣
class MiraiCPException :public std::exception {
public:
	virtual std::string what() { return "C++���ֳ����˸�δ������쳣"; };
	virtual void raise() {};
};
/// ��ʼ���쳣
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
/// �ļ���ȡ�쳣
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
/// �ڲ��쳣
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
/// �����˲����쳣
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
/// �����쳣
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
/// ��ȡȺ��Ա����
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
/// ��ȡȺ��Ա����
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
/// ��ȡȺ����
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
/// �����쳣
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
/// �ϴ��쳣
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

/// ��ϢԴ����
class MessageSource {
public:
    /// ��Ϣ��ids
	const std::string ids;
    /// ��Ϣ��internalids
	const std::string internalids;
	/// ��ϢԴ���л�
    const std::string source;
	MessageSource() {};
	std::string toString(){
	    return this->source;
	}
    MessageSource(std::string ids, std::string internalids, std::string source);
    /// @brief �������ϢԴ�������ڳ��أ�δ��Ӧ�ÿ�����������
	/// @param source ���л��ı�
	/// @param root json�ڵ�
	MessageSource(std::string source, Json::Value root):source(source), ids(Tools::replace(Tools::replace(root["ids"].toStyledString(), " ", ""), "\n", "")), internalids(Tools::replace(Tools::replace(root["internalIds"].toStyledString(), " ", ""), "\n", "")){};
	/*!
	 * @breif ��json�ַ��������л���MessageSource����
	 * @note jsonӦ��Ϊ���¸�ʽ
	 * @code
	 * {"ids":"", "internalids":""}
	 * @endcode
	 */
	static MessageSource deserializeFromString(const std::string& source);
	std::string serializeToString();
	/// @brief ���ظ���Ϣ
	void recall();
};

/// MiraiCode����, ָ���Ա�ת����miraicode������
class MiraiCodeable {
public:
	virtual std::string toMiraiCode() = 0;
};
/// @brief miraicode�ַ���
class MiraiCode {
private:
	std::string content = "";
public:
    /// �����ǰ����
	std::string toString() {
		return content;
	}
	/// ��toString����һ��
	std::string toMiraiCode(){
	    return toString();
	}
	/// ��MiraiCodeable���ͳ�ʼ��һ��miraicode�ַ���
	MiraiCode(MiraiCodeable* a) {
		content = a->toMiraiCode();
	}
	/// ���ı���ʼ��һ��miraicode�ַ���
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

/// С����
class LightApp: public MiraiCodeable {
public:
	std::string content = "";
	/// @brief ʹ�ô��ı����죬�Ƽ�ʹ�������ṹ�巽������
	/// @param content �����ı�
	LightApp(std::string content) {
		this->content = content;
	}
	/// ʹ����ʽ1,�ʺ�����չʾ���޴�ͼ�����ܽ���
	/// @param c �ṹ�壬�����Զ������������
	/// @see LightAppStyle1 in constants.h
	LightApp(LightAppStyle1 c) {
		this->content = "{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[Ӧ��]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":"
			"{\"appName\":\"" + c.appName + "\",\"appType\":4,\"appid\":1109659848,"
			"\"iconUrl\":\"" + c.icon + "\"},"
			"\"data\":[" + c.titles + "],"
			"\"title\":\"" + c.title + "\",\"button\":"
			"[" + c.buttons + "],"
			"\"emphasis_keyword\":\"\"}},\"text\":\"\",\"sourceAd\":\"\"}";
	}
	/// ʹ����ʽ2���д�ͼ�����ܽ���
    /// @param c �ṹ�壬�����Զ������������
    /// @see LightAppStyle1 in constants.h
	LightApp(LightAppStyle2 c) {
		this->content = "{\"config\":"
			"{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQС����]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"1.0.0.103\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1110081493\",\"preview\":\"" + c.preview + "\",\"shareTemplateData\":{},"
			"\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" + c.title2 + "\",\"title\":\"" + c.title + "\","
			"\"host\":{\"uin\":0,\"nick\":\"\"},"
			"\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" + c.icon + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
	}
	/// ��ʽ3���д�ͼ�������ڵ���qq��ʾ�����ڵ����ϵ�������ӻ���ת
    /// @param c �ṹ�壬�����Զ������������
    /// @see LightAppStyle1 in constants.h
	LightApp(LightAppStyle3 c) {
		this->content = "{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQС����]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\"" + c.preview + "\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" + c.title + "\",\"title\":\"" + c.description + "\","
			"\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" + c.icon + "\",\"qqdocurl\":\"" + c.url + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
	}
	std::string toMiraiCode() {
		return "[mirai:app:" + Tools::replace(Tools::replace(content, "[", "\\["), "]", "\\]") + "]";
	}
};

/*ͼ��������*/
class Image: public MiraiCodeable {
public:
    const jmethodID Query = NULL;
	//ͼƬid����ʽ:` {xxx}.xx `
	std::string id = "";
	/*!
	* @brief ��ͼƬid���죬�����ڷ��������Ѿ��е�ͼƬ�������յ���
	* @example ͼƬmiraiCode����: [mirai:image:{ͼƬid}.jpg]
	* @note ���������������ʽ�ҳ�id ` \\[mirai:image:(.*?)\\] `
	*/
	Image(std::string);
	/*
	* ��ȡͼƬ����url
	*/
	std::string queryURL(JNIEnv* = manager->getEnv()) const;

	/*! @brief ȡһ��miraicode�ַ�����ȫ����ͼƬid�������Image
	 *   @param  miraicode���ַ���
	 *   @return vector������ÿһ��Ϊһ��ͼƬid
 	 *   @example
 	 *   @code
	 *          vector<string> temp = Image::GetImgIdFromMiraiCode(param.message);
     *	        for (int i = 0; i < temp.size(); i++) {
     *	    	    logger->Info(temp[i]);
     *	    	    logger->Info("ͼƬ���ص�ַ:" + Image(param.env, temp[i]).queryURL());
   	 *	         }
   	 *	  @endcode
     */
	static std::vector<std::string> GetImgIdsFromMiraiCode(std::string);

	/// ȡͼƬMirai��
	std::string toMiraiCode();
};

/// group, friend, member�ĸ���
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
    /// @brief ��ǰ��������
    ///     - 1 Friend ����
    ///     - 2 Group Ⱥ��
    ///     - 3 Member Ⱥ��Ա
	int type() {return this->_type;}
    /// @brief id��ȫ���������
    ///     - ����ǰtypeΪ1(Friend)ʱ��Ϊ����id
    ///     - ����ǰtypeΪ2(Group)ʱ��ΪȺid
    ///     - ����ǰtypeΪ3(Member)ʱ��ΪȺ��Աid
	unsigned long long id() { return this->_id; }
    /// @brief ��typeΪ3��ʱ����ڣ�����Ϊ0�����Կ�������id
    ///     - ����ǰtypeΪ1(Friend)ʱ��Ϊ0
    ///     - ����ǰtypeΪ2(Group)ʱ��Ϊ0
    ///     - ����ǰtypeΪ3(Member)ʱ��ΪȺ��
    /// @attention ����ǰtypeΪ2(Group)ʱ��Ϊ0����ΪȺ�ţ�id����Ⱥ��
	unsigned long long groupid() { return this->_groupid; }
	/// Ⱥ���ƣ�Ⱥ��ԱȺ��Ƭ��������ǳ�
	std::string nickOrNameCard() { return this->_nickOrNameCard; };
	/// ͷ��url��ַ
	std::string avatarUrl() { return this->_avatarUrl; };
	/// ����bot
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
	/// ���л����ı�������ͨ��deserializationFromString�����л������ڱ���
	/// @see Contact::fromString()
	std::string serializationToString() {
		return this->serialization().dump();
	}
    /// �����л���bot������ͨ��serializationToString���л������ڱ���
    /// @see Contact::serializationToString()
    /// @param source ���л�����ı�
    /// @throw APIException
	static Contact deserializationFromString(const std::string &source);
};

// Ⱥ�ļ�

/// @brief ������Ϣ
struct dinfo {
	/// ���ص�ַ
	std::string url;
	/// md5 ������У��
	std::string md5;
	/// sha1 ������У��
	std::string sha1;
};
/// @brief �ļ���Ϣ
struct finfo {
	/// �ļ���С
	unsigned long long size;
	/// �ϴ���id
	unsigned long long uploaderid;
	/// ���ش���
	unsigned int downloadtime;
	/// �ϴ�ʱ��, ʱ�����ʽ
	unsigned long long uploadtime;
	/// �ϴθ���ʱ��, ʱ�����ʽ
	unsigned long long lastmodifytime;
};
/// @brief Զ��(Ⱥ)�ļ�����
class RemoteFile : public MiraiCodeable {
public:
    /// �ļ�Ψһid, �����Ա�
    const std::string id;
    /// �ļ��ڲ�id, ���ڹ���miraiCode����
    const unsigned int internalid;
    /// �ļ���
    const std::string name;
    /// �ļ���С
    const long long size;
    /// �ļ���Ⱥ�ļ���·��
    const std::string path;
    /// �ļ�������Ϣ
    /// @see dinfo
    const dinfo dinfo;
    /// �ļ���Ϣ
    /// @see finfo
    const finfo finfo;
	std::string serializeToString();
	static RemoteFile deserializeFromString(const std::string& source);
	/// ��������Ϣ����
	RemoteFile(std::string i, unsigned int ii, std::string n, long long s, struct dinfo d, struct finfo f):id(i),internalid(ii),name(n),size(s),dinfo(d),
                                                                                             finfo(f){};
	/// �����ϴ��󹹽�����Ч, ����ȡ��internalidʱ(internalid != 0) ���������ϴ������»�ȡinternalid��ת��
	std::string toMiraiCode() {
		if (internalid == 0) {
			// �����ϴ�
			throw RemoteFileException("toMiraiCode error: internalid���������ϴ�");
		}
		return "[mirai:file:" + id + "," + std::to_string(internalid) + "," + name + "," + std::to_string(size) + "]";
	}
};

/// �ϵײ�api
class LowLevelAPI {
public:
	/// @brief �����װ�ײ㷢����Ϣ�ӿ�
	/// @param content ��Ϣ�ַ���
	/// @param c Ŀ��Contactָ��
	/// @param miraicode �Ƿ�Ϊmiraicode��ʽ
	/// @param env JNIEnv
	/// @return 
	static std::string send0(std::string content, Contact* c, bool miraicode, JNIEnv* env) {
	    nlohmann::json j;
	    j["content"] = content;
	    j["contact"] = c->serialization();
		return Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KSend, Tools::str2jstring(j.dump().c_str(),env), (jboolean)miraicode), env);
	}
	/// @brief ȡ����ϵ�˵�һЩ��Ϣ
	/// @param c ����ϵ��Contactָ��
	/// @return json��ʽ�ַ�����������
	static inline std::string getInfoSource(Contact* c, JNIEnv* env = manager->getEnv()) {
		return Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KRefreshInfo, Tools::str2jstring(c->serializationToString().c_str(), env)));
	}
	/*!
	 * @brief �ϴ�ͼƬ
	 * @param path ���ص�ַ
	 * @param c �ϴ��Ķ���
	 * @param env JNIEnv
	 * @return string ������json
	 */
	static inline std::string uploadImg0(std::string path, Contact* c, JNIEnv* env = manager->getEnv()) {
		return Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KUploadImg,Tools::str2jstring(path.c_str(), env), Tools::str2jstring(c->serializationToString().c_str(), env)));
	}
	/// ÿ������ı�����Ϣ
	struct info {
		std::string nickornamecard;
		std::string avatarUrl;
	};
	/// ��ȡÿ�����������Ϣ
	/// @see LowLevelAPI::info
	static info info0(std::string source) {
		info re;
		Json::Value tmp = Tools::StringToJson(source);
		re.avatarUrl = tmp["avatarUrl"].asCString();
		re.nickornamecard = tmp["nickornamecard"].asCString();
		return re;
	}
};

///�����¼��ÿ����Ϣ
class ForwardNode {
public:
	///������id
	const unsigned long long id = 0;
	///�������ǳ�
	const std::string name = "";
	///������Ϣ
	const std::string message = "";
	///����ʱ��
	const int time = 0;
	/// @brief �����¼���ÿ����Ϣ
	/// @param i - ������id
	/// @param n - �������ǳ�
	/// @param m - ���͵���Ϣ
	/// @param t - ����ʱ�䣬��ʱ�����
    ForwardNode(const unsigned long long int id, const std::string &name, const std::string &message, const int time)
            : id(id), name(name), message(message), time(time) {}
	/// @brief ���������¼��ÿ����Ϣ
	/// @param c - �����ߵ�contactָ��
	/// @prama m - ���͵���Ϣ
	/// @param t - ����ʱ�䣬ʱ�����ʽ
	ForwardNode(Contact* c, std::string &message, int t): id(c->id()), name(c->nickOrNameCard()), message(message), time(t)
	{}
};

///�����¼, ��ForwardNode���
/// @see class ForwardNode
class ForwardMessage {
public:
	Json::Value sendmsg;
	/*!
	*@brief ����һ�������¼
	*@details ��һ�������������¼�����ĵط�
	* Ȼ����ÿ����Ϣ
	*@example@code
	*ForwardMessage(&e.group,
	*{
	*	ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
	*	ForwardNode(1930893235, "Eritque arcus", "hahaha", -1)
	*}).sendTo(&e.group);
	* @endcode
	*/
	ForwardMessage(Contact* c, std::initializer_list<ForwardNode> nodes);
	/// ���͸�Ⱥ����ѻ�Ⱥ��Ա
	void sendTo(Contact* c, JNIEnv* = manager->getEnv()) const;
};

/// ��ǰbot�˺���Ϣ
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
     * @brief ˢ��bot��Ϣ
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
	/// �ǳ�
	std::string nick() {
		check();
		return this->_nick;
	}
	/// ͷ����������
	std::string avatarUrl() {
		check();
		return this->_avatarUrl;
	}
	/// ȡ�����б�
	std::vector<unsigned long long> getFriendList(JNIEnv* env = manager->getEnv()) {
		std::string temp = Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib,
			config->KQueryBFL,
			(jlong)this->id()));
		return Tools::StringToVector(temp);
	}
	/// �����б�string��ʽ���أ����ڱ���
	std::string FriendListToString() {
		return Tools::VectorToString(getFriendList());
	}
	/// ȡȺ�б�
	std::vector<unsigned long long> getGroupList(JNIEnv* env = manager->getEnv()) {
		std::string temp = Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib,
			config->KQueryBGL,
			(jlong)this->id()));
		return Tools::StringToVector(temp);
	}
    /// Ⱥ�б�string��ʽ���أ����ڱ���
	std::string GroupListToString() {
		return Tools::VectorToString(getGroupList());
	}
};

/// ����������
class Friend:public Contact{
public:
	Friend(unsigned long long friendid, unsigned long long botid, JNIEnv* =manager->getEnv());
	Friend():Contact(){};
	
	/** 
	 * @brief �ϴ�����ͼƬ�����Ҫ�þ���·��. 
	 * ����miraiҪ����ͼƬ���Ͷ�������ʹ�ñ������ϴ���ͼƬֻ�ܷ�������. 
	 * ���֧��ͼƬ��СΪ30MB.
	 * @exception invalid_argument����·����Ч
	 * @param filename �ļ�·��
	 * @return ͼƬʵ��
	*/
	Image uploadImg(const std::string& filename, JNIEnv* = manager->getEnv());

	/*������Ϣ*/
	/**
	 * @brief ����miraicode
	 * @param msg - MiraiCodeable����ָ�� - ����
	*/
	MessageSource SendMiraiCode(MiraiCodeable* msg) {
		return SendMiraiCode(msg->toMiraiCode());
	}
	MessageSource SendMiraiCode(MiraiCode msg) {
		return SendMiraiCode(msg.toString());
	}
	MessageSource SendMiraiCode(std::string msg, JNIEnv* = manager->getEnv());
	/// ���ʹ��ı���Ϣ
	MessageSource SendMsg(std::string msg, JNIEnv* = manager->getEnv());
};

/// Ⱥ��Ա������
class Member :public Contact {
private:
	jmethodID Mute_id = NULL;
	jmethodID Query_permission = NULL;
	jmethodID KickM = NULL;
public:
	/// @brief Ȩ�޵ȼ�
	///     - OWNERȺ�� Ϊ 2
	///     - ADMINISTRATOR����Ա Ϊ 1
	///     - MEMBERȺ��Ա Ϊ 0
	/// @note ������Щ������constants.h���ж���
	unsigned int permission = 0;
	/// qqid, groupid
	Member(unsigned long long qqid, unsigned long long groupid, unsigned long long botid, JNIEnv* = manager->getEnv());
	/*
	* �ϴ�����ͼƬ�����Ҫ�þ���·��
	* ����miraiҪ����ͼƬ���Ͷ�������ʹ�ñ������ϴ���ͼƬֻ�ܷ���Ⱥ
	* ���֧��ͼƬ��СΪ30MB
	* �����׳�invalid_argument�쳣����·����Ч
	*/
	Image uploadImg(const std::string& filename, JNIEnv* = manager->getEnv());
	Member():Contact() {};
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
	void Kick(const std::string& reason, JNIEnv* = manager->getEnv());
};

/// Ⱥ��������
class Group :public Contact {
public:
	/*ȡȺ��Ա�б�-vector<long>*/
	std::vector<unsigned long long> getMemberList() {
		std::string re = Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib,
			config->KQueryML,
			Tools::str2jstring(this->serializationToString().c_str())));
		if (re == "E1") {
			throw GroupException();
		}
		return Tools::StringToVector(re);
	}
	/*��string��ʽȡȺ��Ա�б�
	��ʽ��
		ÿ��Ⱥ��Աid���ö��ŷָ�
	*/
	std::string MemberListToString() {
		return Tools::VectorToString(getMemberList());
	};
	//ȡȺ��
	Member getOwner(JNIEnv* = manager->getEnv());
	//������Ⱥ�Ź���Ⱥ����
	Group(unsigned long long groupid, unsigned long long botid, JNIEnv* = manager->getEnv());
	Group(Contact c):Contact(c);
	/*
	* �ϴ�����ͼƬ�����Ҫ�þ���·��
	* ����miraiҪ����ͼƬ���Ͷ�������ʹ�ñ������ϴ���ͼƬֻ�ܷ���Ⱥ
	* ���֧��ͼƬ��СΪ30MB
	* �����׳�invalid_argument�쳣����·����Ч
	*/
	Image uploadImg(const std::string& filename, JNIEnv* = manager->getEnv());
	/*
	�ϴ�������
	path-Ⱥ�ļ�·��(���ļ���),��Ŀ¼Ϊ/
	filepath-�����ļ�·��
	�� group.uploadFIle("/test.txt", "D:\\xxxx.xxx")
	*/
	RemoteFile sendFile(const std::string& path, const std::string& filepath, JNIEnv* = manager->getEnv());
	/* 
	ȡȺ�ļ���Ϣ,���Զ�������Ŀ¼
	path-Ⱥ�ļ�·��(�����ļ���)
	id-�ļ�id,�ɿգ�����Ϊ��·������(��ʱ·��Ҫ���ļ���)
	��ΪȺ�ļ����������ļ����ڵ����ԣ��ò��Ҳ����ɿ���ֻ�ܷ��������ļ��е�����һ���ļ�
	*/
	RemoteFile getFile(const std::string& path, const std::string& id = "", JNIEnv* = manager->getEnv());
	struct short_info {
		// ·�����ļ���
		std::string path = "";
		// Ψһid
		std::string id = "";
	};
	/*
	��ȡpath·����ȫ���ļ���Ϣ
	����ֵΪһ��vector����, ÿһ��Ϊshort_info
	*/
	std::vector<short_info> getFileList(const std::string& path, JNIEnv* = manager->getEnv());
	// ȡ�ļ��б���ֵ���ַ���
	std::string getFileListString(const std::string& path, JNIEnv* = manager->getEnv());
	Group():Contact() {};
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

/// Atһ��Ⱥ��Ա
inline std::string At(Member a) {
	/*����at����˵�miraicode*/
	return "[mirai:at:" + std::to_string(a.id()) + "]";
}
/// Atһ��Ⱥ��Ա
inline std::string At(unsigned long long a) {
	/*����at����˵�miraicode*/
	return "[mirai:at:" + std::to_string(a) + "]";
}

/// �����¼�����timeoutevent���ǻ������¼���ָ���л�����ʵ��
class BotEvent {
 public:
	 const Bot bot;
	 BotEvent(unsigned long long botid):bot(Bot(botid)) {
	 }
 };

///Ⱥ��Ϣ�¼�����
class GroupMessageEvent:public BotEvent {
public:
	///��ԴȺ
	const Group group;
	///������
	const Member sender;
	///��Ϣ����
	const std::string message;
	///��ϢԴ
	const MessageSource messageSource;
    GroupMessageEvent(unsigned long long int botid, const Group &group, const Member &sender,
                      const std::string &message, const MessageSource &messageSource) : BotEvent(botid), group(group),
                                                                                        sender(sender),
                                                                                        message(message),
                                                                                        messageSource(messageSource) {}
};

/// ˽����Ϣ�¼�������
class PrivateMessageEvent : public BotEvent {
public:
	/// ������
	const Friend sender;
	/// ������Ϣ
	const std::string message;
	/// ��ϢԴ
	const MessageSource messageSource;
    PrivateMessageEvent(unsigned long long int botid, const Friend &sender, const std::string &message,
                        const MessageSource &messageSource) : BotEvent(botid), sender(sender), message(message),
                                                              messageSource(messageSource) {}
};

/// Ⱥ�������¼�������
class GroupInviteEvent: public BotEvent {
public:
    /// �¼����л��ı�
    const std::string source;
	/// �������ǳ�
	const std::string inviterNick = "";
	/// ������id
	const unsigned long long inviterid = 0;
	/// �����������
	const std::string groupName = "";
	/// Ⱥ��
	const unsigned long long groupid = 0;
	static void reject(std::string source) {
		std::string re = Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KGioperation, Tools::str2jstring(source.c_str()), (jboolean)false));
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
		std::string re = Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KGioperation, Tools::str2jstring(source.c_str()), (jboolean)true));
		if (re == "Y") return;
		if (re == "E")logger->Error("Ⱥ�������¼�ͬ��ʧ��,id:" + source);
	}
	void accept() {
		this->accept(this->source);
	}
	/*!
	 * @brief Ⱥ�����¼�
	 * @param botid ��ǰbotid
	 * @param source ���л����ַ���
	 * @param inviterNick �������ǳ�
	 * @param inviterid ������id
	 * @param groupName Ⱥ������
	 * @param groupid Ⱥ��
	 */
    GroupInviteEvent(unsigned long long int botid, const std::string &source, const std::string &inviterNick,
                     unsigned long long int inviterid, const std::string &groupName, unsigned long long int groupid)
            : BotEvent(botid), source(source), inviterNick(inviterNick), inviterid(inviterid), groupName(groupName),
              groupid(groupid) {}
};

/// ���������¼�����
class NewFriendRequestEvent : public BotEvent {
public:
    /// @brief ���л����¼���Ϣ
    const std::string source;
    /// @brief �Է�id
    const unsigned long long fromid;
    const unsigned long long fromgroupid;
    /// @brief �Է��ǳ�
    const std::string nick;
    /// @brief ��������
    const std::string message;
    /// @brief �ܾ���������
    /// @param source �¼����л���Ϣ
	static void reject(std::string source) {
		std::string re = Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KNfroperation, Tools::str2jstring(source.c_str()), (jboolean)false));
		if (re == "Y") return;
		if (re == "E")logger->Error("���������¼�ͬ��ʧ��,id:" + source);
	}
	/// @brief �ܾ���������
	void reject() {
		this->reject(this->source);
	}
	/// @brief ���ܺ�������
	/// @param source �¼����л���Ϣ
	static void accept(std::string source) {
		std::string re = Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KNfroperation, Tools::str2jstring(source.c_str()), (jboolean)true));
		if (re == "Y") return;
		if (re == "E")logger->Error("���������¼�ͬ��ʧ��,id:" + source);
	}
	/// @brief ��������
	void accept() {
		this->accept(this->source);
	}
    NewFriendRequestEvent(unsigned long long int botid, const std::string &source, const unsigned long long int fromid,
                          const unsigned long long int fromgroupid, const std::string &nick, const std::string &message)
            : BotEvent(botid), source(source), fromid(fromid), fromgroupid(fromgroupid), nick(nick), message(message) {}
};

/// ��Ⱥ��Ա����
class MemberJoinEvent : public BotEvent {
public:
	/*!
	* @brief �¼�����
	*   1 - ���������
	*   2 - ��������
	*   3 - ԭȺ��ͨ�� https://huifu.qq.com/ �ָ�ԭ��Ⱥ����ݲ���Ⱥ
	*/
	const int type = 0;
	///�½���ĳ�Ա
	const Member member;
	///Ŀ��Ⱥ
	const Group group;
	///������, ��type = 1ʱ���ڣ��������member������ͬ
	const Member inviter;
    /*!
     * @brief ��Ⱥ��Ա��Ⱥ�¼�
     * @param botid botid
     * @param type ��� @see MemberJoinEvent::type
     * @param member ��ȺȺ��Ա
     * @param group Ⱥ��
     * @param inviter ����Ⱥ��Ա����������ں�member����һ��
     */
    MemberJoinEvent(unsigned long long int botid, const int type, const Member &member, const Group &group,
                    const Member &inviter) : BotEvent(botid), type(type), member(member), group(group),
                                             inviter(inviter) {}
};

/// Ⱥ��Ա�뿪
class MemberLeaveEvent : public BotEvent {
public:
	/*!
	* @brief �¼�����
	*           1 - ���߳�
	*           2 - �����˳�
	*/
	const int type = 0;
	/// �˳��ĳ�Աq��
	const unsigned long long memberid;
	/// Ŀ��Ⱥ
	const Group group;
	/// ������, �����˳�ʱ��member��ͬ���ó�Ա�����ǵ�ǰbot������Ϊoperater����ϵͳoperator����
	const Member operater;
    /*!
     * @brief
     * @param botid
     * @param type
     * @param member �˳���Ⱥ��Ա
     * @param group Ⱥ
     * @param operater ������, �����˳�ʱ��member��ͬ���ó�Ա�����ǵ�ǰbot������Ϊoperater����ϵͳoperator����
     */
    MemberLeaveEvent(unsigned long long int botid, const int type, const unsigned long long memberid, const Group &group,
                     const Member &operater) : BotEvent(botid), type(type), memberid(memberid), group(group),
                                               operater(operater) {}
};

/// ������Ϣ
class RecallEvent : public BotEvent {
public:
	/// Ϊ1ʱ�Ǻ���˽���г��أ�Ϊ2ʱΪȺ���ڳ���
	const int type = 0;
	/// ʱ���
	const int time = 0;
	/// ԭ������
	const unsigned long long authorid = 0;
	/// ������
	const unsigned long long operatorid = 0;
	/// ��Ϣid
	const std::string ids = "";
	//�ڲ�ids
	const std::string internalids = "";
	//��type��2��ʱ����ڣ�����Ϊ0
	const unsigned long long groupid = 0;
    /*!
     * @brief �����¼�
     * @param botid ��Ӧbot
     * @param type ����
     * @param time ʱ��
     * @param authorid ������id
     * @param operatorid ������id
     * @param ids ��ϢԴids
     * @param internalids ��ϢԴinternalids
     * @param groupid
     */
    RecallEvent(unsigned long long int botid, const int type, const int time, const unsigned long long int authorid,
                const unsigned long long int operatorid, const std::string &ids, const std::string &internalids,
                const unsigned long long int groupid) : BotEvent(botid), type(type), time(time), authorid(authorid),
                                                        operatorid(operatorid), ids(ids), internalids(internalids),
                                                        groupid(groupid) {}
};

/// �����˽���ĳȺ
class BotJoinGroupEvent : public BotEvent {
public:
	/// 1-��������,2-���������,3-�ṩ�ָ�Ⱥ����ݼ���
	const int type;
	/// �����Ⱥ
	const Group group;
	/// ��type=2ʱ���ڣ�Ϊ�����ˣ�����Ϊ�գ����ÿ��ܻᱨ��
	const Member inviter;
    /*!
     * @brief bot����Ⱥ
     * @param botid ��Ӧbot
     * @param type ���
     * @param group �����Ⱥ
     * @param inviter ������
     */
    BotJoinGroupEvent(unsigned long long int botid, const int type, const Group &group, const Member &inviter)
            : BotEvent(botid), type(type), group(group), inviter(inviter) {}
};

/// Ⱥ��ʱ�Ự
class GroupTempMessageEvent : public BotEvent {
public:
	/// ��ԴȺ
	const Group group;
	/// ������
	const Member sender;
	/// ��Ϣ����
	const std::string message;
	/// ��ϢԴ
	const MessageSource messageSource;
    /*!
     * @brief Ⱥ��ʱ�Ự��Ϣ�¼�
     * @param botid ��Ӧbot
     * @param group �����Ⱥ
     * @param sender ������Ϣ����
     * @param message ��Ϣ
     * @param messageSource ��ϢԴ
     */
    GroupTempMessageEvent(unsigned long long int botid, const Group &group, const Member &sender,
                          const std::string &message, const MessageSource &messageSource) : BotEvent(botid),
                                                                                            group(group),
                                                                                            sender(sender),
                                                                                            message(message),
                                                                                            messageSource(
                                                                                                    messageSource) {}
};

/// ������ʱ����,time�Ƕ��ٺ����ʼ��id���Զ����ʶ��
inline void SetScheduling(long time, std::initializer_list<std::string> args, BotEvent* e) {
	Json::Value obj;
	Json::Value root;
	for (const std::string& it : args) {
		obj.append(it);
	}
	root["value"] = obj;
	manager->getEnv()->CallStaticVoidMethod(config->CPP_lib, config->KSchedule, (jlong)time, Tools::str2jstring(Tools::JsonToString(root).c_str()));
}

/*��ʱ����ִ��*/
class SchedulingEvent{
public:
	void init() {};
	/*�Զ���id��ʶ��*/
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