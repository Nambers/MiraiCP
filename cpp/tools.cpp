#include "pch.h"

#include <utility>

using json = nlohmann::json;

/*�̹߳���*/

threadManager::threadManager() {
	this->_threads = std::map <std::string, t>();
}
void threadManager::setEnv(JNIEnv* e) {
	mtx.lock();
	if (!this->included(threadManager::getThreadId())) {
		t tmp{
		    e,
		    false
		};
		this->_threads.insert(std::pair<std::string, t>(threadManager::getThreadId(), tmp));
	}
	else {
		this->_threads[threadManager::getThreadId()].e = e;
	}
	mtx.unlock();
}
void threadManager::newEnv(const char* threadName) {
	JNIEnv* env = nullptr;
	JavaVMAttachArgs args{
		JNIVersion,
        const_cast<char *>(threadName),
		nullptr
	};
	gvm->AttachCurrentThread((void**)&env, &args);
	t tmp{env, true};
	this->_threads.insert(std::pair<std::string, t>(threadManager::getThreadId(), tmp));
	logger->Info("refresh env");
};
void threadManager::detach() {
	mtx.lock();
	if (this->included(threadManager::getThreadId())) {
		bool att = this->_threads[threadManager::getThreadId()].attach;
		this->_threads.erase(threadManager::getThreadId());
		if (att)
			gvm->DetachCurrentThread();
	}
	mtx.unlock();
}
bool threadManager::included(const std::string& id) {
	if (this->_threads.empty() || this->_threads.count(id) == 0)
		return false;
	return true;
}
JNIEnv* threadManager::getEnv() {
	mtx.lock();
	if (!this->included(getThreadId())) {
		this->newEnv();
	}
	JNIEnv* tmp = this->_threads[threadManager::getThreadId()].e;
	mtx.unlock();
	return tmp;
}

/*
��־��ʵ��
throw: InitException ���Ҳ���ǩ��
*/


void Logger::init(JNIEnv* env) {
	this->CPP_lib = (jclass)(env->NewGlobalRef(env->FindClass("tech/eritquearcus/miraicp/CPP_lib")));
	this->log = env->GetStaticMethodID(this->CPP_lib, "KSendLog", "(Ljava/lang/String;I)V");
	if (this->CPP_lib == nullptr) throw InitException("logger��ʼ������", 1);
    if (this->log == nullptr) throw InitException("logger��ʼ������", 2);
}
void Logger::registerHandle(Logger::action action) {
    this->loggerhandler.action = std::move(action);
}
void Logger::setHandleState(bool state) {
    this->loggerhandler.enable = state;
}
void Logger::log0(const std::string& content, JNIEnv* env, int level) {
    if(this->loggerhandler.enable)
        this->loggerhandler.action(content, level);
    env->CallStaticVoidMethod(config->CPP_lib, this->log, Tools::str2jstring(content.c_str()), (jint) level);
}
void Logger::Warning(const std::string& content, JNIEnv* env) {
	this->log0(content, env, 1);
}
void Logger::Error(const std::string& content, JNIEnv* env) {
	this->log0(content, env, 2);
}
void Logger::Info(const std::string& content, JNIEnv* env) {
	this->log0(content, env, 0);
}
Logger::~Logger() {
	manager->getEnv()->DeleteGlobalRef(this->CPP_lib);
}

/*
������ʵ��
throw: InitxException ���Ҳ�����Ӧǩ��
*/

void Config::Init(JNIEnv* env) {	
	// int x = (int)env->EnsureLocalCapacity((jint)22);
	this->initexception = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("java/lang/NoSuchMethodException")));
	this->CPP_lib = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("tech/eritquearcus/miraicp/CPP_lib")));
	if (this->CPP_lib == nullptr) {
		throw InitException("��ʼ������", 1);
	}
	this->KSend = env->GetStaticMethodID(CPP_lib, "KSend", "(Ljava/lang/String;Z)Ljava/lang/String;");
	this->KRecall = env->GetStaticMethodID(CPP_lib, "KRecall", "(Ljava/lang/String;)Ljava/lang/String;");
	this->KQueryImgUrl = env->GetStaticMethodID(CPP_lib, "KQueryImgUrl", "(Ljava/lang/String;)Ljava/lang/String;");
	this->KRefreshInfo = env->GetStaticMethodID(CPP_lib, "KRefreshInfo", "(Ljava/lang/String;)Ljava/lang/String;");
	this->KUploadImg = env->GetStaticMethodID(CPP_lib, "KUploadImg", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
	this->KSendFile = env->GetStaticMethodID(CPP_lib, "KSendFile", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
	this->KRemoteFileInfo = env->GetStaticMethodID(CPP_lib, "KRemoteFileInfo", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
	this->KSchedule = env->GetStaticMethodID(CPP_lib, "KSchedule", "(JLjava/lang/String;)V");
	this->KMuteM = env->GetStaticMethodID(CPP_lib, "KMuteM", "(ILjava/lang/String;)Ljava/lang/String;");
	this->KQueryM = env->GetStaticMethodID(CPP_lib, "KQueryM", "(Ljava/lang/String;)Ljava/lang/String;");
	this->KKickM = env->GetStaticMethodID(CPP_lib, "KKickM", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
	this->KMuteGroup = env->GetStaticMethodID(CPP_lib, "KMuteGroup", "(ZLjava/lang/String;)Ljava/lang/String;");
	this->KQueryML = env->GetStaticMethodID(CPP_lib, "KQueryML", "(Ljava/lang/String;)Ljava/lang/String;");
	this->KQueryBFL = env->GetStaticMethodID(CPP_lib, "KQueryBFL", "(J)Ljava/lang/String;");
	this->KQueryBGL = env->GetStaticMethodID(CPP_lib, "KQueryBGL", "(J)Ljava/lang/String;");
	this->KQueryOwner = env->GetStaticMethodID(CPP_lib, "KQueryOwner", "(Ljava/lang/String;)Ljava/lang/String;");
	this->KBuildforward = env->GetStaticMethodID(CPP_lib, "KBuildforward", "(Ljava/lang/String;J)Ljava/lang/String;");
	this->KNfroperation = env->GetStaticMethodID(CPP_lib, "KNfroperation", "(Ljava/lang/String;Z)Ljava/lang/String;");
	this->KGioperation = env->GetStaticMethodID(CPP_lib, "KGioperation", "(Ljava/lang/String;Z)Ljava/lang/String;");
}
Config::~Config() {
	manager->getEnv()->DeleteGlobalRef(this->CPP_lib);
}

Event::~Event()
{
	Node* temp[] = { GMHead, PMHead, GHead, NFHead,MJHead,MLHead,RHead,SHead,BHead };
	for (Node* ptr : temp) {
		Node* now = ptr;
		Node* t = nullptr;
		while (true) {
			t = now;
			if (now->nextNode == nullptr) {
				delete now;
				break;
			}
			now = now->nextNode;
			delete t;
		}
	}
}

SchedulingEvent::SchedulingEvent(const std::string& str) {
	const json arrayNum = json::parse(str)["value"];
	for (const auto & i : arrayNum)
	{
		ids.emplace_back(i);
	}
}
void MessageSource::recall() {
	std::string re = Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KRecall,
		Tools::str2jstring(this->toString().c_str())));
	if (re == "Y") return;
	if (re == "E1") throw BotException(1);
	if (re == "E2") throw RecallException();
}

MessageSource::MessageSource(const std::string &ids, const std::string &internalids, const std::string &source) : ids(
        ids), internalids(internalids), source(source) {}

std::string MessageSource::serializeToString() {
    return source;
}

MessageSource MessageSource::deserializeFromString(const std::string &source) {
    json j = json::parse(source);
    try {
        return MessageSource(j["ids"].dump(), j["internalIds"].dump(), source);
    }catch(json::type_error &e){
        logger->Error("��ϢԴ���л�������ʽ������(MessageSource::deserializeFromString)");
        logger->Error(source);
        logger->Error(e.what());
        throw e;
    }
}

//Զ���ļ�(Ⱥ�ļ�)
RemoteFile RemoteFile::deserializeFromString(const std::string& source) {
    json j;
    try {
        j = json::parse(source);
    }catch(json::parse_error &e){
        logger->Error("��ʽ��jsonʧ�ܣ�RemoteFile::deserializeFromString");
        logger->Error(source);
        logger->Error(e.what());
        throw e;
    }
    struct dinfo d{
        j["dinfo"]["url"],
        j["dinfo"]["md5"],
        j["dinfo"]["shar1"]
    };
    struct finfo f{
        j["finfo"]["size"],
        j["finfo"]["uploaderid"],
        j["finfo"]["downloadtime"],
        j["finfo"]["uploadtime"],
        j["finfo"]["lastmodifytime"]
    };
	return RemoteFile(j["id"], j["internalid"], j["name"], j["size"], d, f);
}

std::string RemoteFile::serializeToString() {
    json j;
    j["dinfo"]["url"] = this->dinfo.url;
    j["dinfo"]["md5"] = this->dinfo.md5;
    j["dinfo"]["shar1"] = this->dinfo.sha1;
    j["finfo"]["size"] = this->finfo.size;
    j["finfo"]["uploaderid"] = this->finfo.uploaderid;
    j["finfo"]["downloadtime"] = this->finfo.downloadtime;
    j["finfo"]["uploadtime"] = this->finfo.uploadtime;
    j["finfo"]["lastmodifytime"] = this->finfo.lastmodifytime;
    j["id"] = this->id;
    j["internalid"] = this->internalid;
    j["name"] = this->name;
    j["size"] = this->size;
    return "";
}

//������������¼
void ForwardMessage::sendTo(Contact* c, JNIEnv* env) const {
	json temp;
	temp["id"] = c->id();
	temp["groupid"] = c->groupid();
	temp["type"] = c->type();
	temp["content"] = sendmsg;
	std::string re = Tools::jstring2str((jstring)env->
		CallStaticObjectMethod(config->CPP_lib, config->KBuildforward,
			Tools::str2jstring(temp.dump().c_str(), env),
			(jlong)c->botid()), env);
	if (re == "Y") return;
	if (re == "E1") {
		switch (c->type()) {
		case 1:
			throw FriendException();
		case 2:
			throw GroupException();
		case 3:
			throw MemberException(1);
		}
	}
	if (re == "E2") throw MemberException(2);
	if (re == "E3") throw APIException("��������");
}
ForwardMessage::ForwardMessage(Contact* c, std::initializer_list<ForwardNode> nodes) {
	json root;
	json value;
	root["type"] = c->type();
	root["id"] = c->id();
	root["id2"] = c->groupid();
	for (const ForwardNode& node : nodes) {
		json temp;
		temp["id"] = node.id;
		temp["time"] = node.time;
		temp["message"] = node.message;
		temp["name"] = node.name;
		value.push_back(temp);
	}
	root["value"] = value;
	sendmsg = root;
}

/*ͼƬ��ʵ��*/
Image::Image(std::string imageId):Query(config->KQueryImgUrl) {
	this->id = std::move(imageId);
}
std::string Image::queryURL(JNIEnv* env) const {
	return Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, this->Query, Tools::str2jstring(this->id.c_str())));
}
std::vector<std::string> Image::GetImgIdsFromMiraiCode(std::string MiraiCode) {
	std::vector<std::string> result = std::vector<std::string>();
	std::string temp = std::move(MiraiCode);
	std::smatch m;
	std::regex re("\\[mirai:image:(.*?)\\]");
	while (std::regex_search(temp, m, re)) {
		result.push_back(m[1]);
		temp = m.suffix().str();
	}
	return result;
}
std::string Image::toMiraiCode() {
	return "[mirai:image:" + this->id + "]";
}

/*������ʵ��*/
Friend::Friend(unsigned long long id, unsigned long long botid, JNIEnv* env):Contact() {
	this->_type = 1;
	this->_id = id;
	this->_botid = botid;
	std::string temp = LowLevelAPI::getInfoSource(this, env);
	if (temp == "E1") {
		throw FriendException();
	}
	LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
	this->_nickOrNameCard = tmp.nickornamecard;
	this->_avatarUrl = tmp.avatarUrl;
}
Image Friend::uploadImg(const std::string& filename, JNIEnv* env) {
	std::ifstream fin(filename);
	if (!fin) {
		logger->Error("�ļ�������,λ��:C-Friend::uploadImg(),�ļ���:" + filename);
		fin.close();
		throw std::invalid_argument("NO_FILE_ERROR");
	}
	fin.close();
	std::string re = LowLevelAPI::uploadImg0(filename, this, env);

	return Image(re);
}
MessageSource Friend::SendMiraiCode(std::string msg, JNIEnv* env) {
	std::string re = LowLevelAPI::send0(std::move(msg), this, true, env);
	if (re == "E1") {
		throw FriendException();
	}
	return MessageSource::deserializeFromString(re);
}
MessageSource Friend::SendMsg(std::string msg, JNIEnv* env){
	std::string re = LowLevelAPI::send0(std::move(msg), this, false, env);
	if (re == "E1") {
		throw FriendException();
	}
	return MessageSource::deserializeFromString(re);
}

/*��Ա��ʵ��*/
Member::Member(unsigned long long id, unsigned long long groupid, unsigned long long botid, JNIEnv* env):Contact() {
	this->_type = 3;
	this->_id = id;
	this->Mute_id = config->KMuteM;
	this->_groupid = groupid;
	this->Query_permission = config->KQueryM;
	this->KickM = config->KKickM;
	this->_botid = botid;
	std::string temp = LowLevelAPI::getInfoSource(this, env);
	if (temp == "E1")
		throw MemberException(1);
	if (temp == "E2")
		throw MemberException(2);
	LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
	this->_nickOrNameCard = tmp.nickornamecard;
	this->_avatarUrl = tmp.avatarUrl;
	this->permission = getPermission();
	if (temp == "E1") {
		throw MemberException(1);
	}
	if (temp == "E2") {
		throw MemberException(2);
	}
}
unsigned int Member::getPermission(JNIEnv* env){
	std::string re = Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->KQueryM, Tools::str2jstring(this->serializationToString().c_str(), env)), env);
	if (re == "E1") {
		throw MemberException(1);
	}
	if(re == "E2"){
		throw MemberException(2);
	}
	return stoi(re);
}
void Member::Mute(int time, JNIEnv* env) {
	std::string re = Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, this->Mute_id, (jint)time, Tools::str2jstring(this->serializationToString().c_str(), env)), env);
	if (re == "Y") {
		return;
	}
	if (re == "E1") {
		throw MemberException(1);
	}
	if (re == "E2") {
		throw MemberException(2);
	}
	if (re == "E3") {
		throw BotException(1);
	}
	if (re == "E4") {
		throw MuteException();
	}
}
void Member::Kick(const std::string& reason, JNIEnv* env) {
	std::string re = Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, this->KickM, Tools::str2jstring(reason.c_str(), env), Tools::str2jstring(this->serializationToString().c_str(), env)), env);
	if (re == "Y") {
		return;
	}
	if (re == "E1") {
		throw MemberException(1);
	}
	if (re == "E2") {
		throw MemberException(2);
	}
	if (re == "E3") {
		throw BotException(1);
	}
}
Image Member::uploadImg(const std::string& filename, JNIEnv* env) {
	std::ifstream fin(filename);
	if (!fin) {
		fin.close();
		throw IOException("�ļ�������,λ��:C++���� uploadImg2Group(),�ļ���:" + filename);
	}
	fin.close();
	std::string re = LowLevelAPI::uploadImg0(filename, this, env);
	return Image(re);
}
MessageSource Member::SendMiraiCode(std::string msg, JNIEnv* env) {
	std::string re = LowLevelAPI::send0(std::move(msg), this, true, env);
	if (re == "E1") {
		throw MemberException(1);
	}
	if (re == "E2") {
		throw MemberException(2);
	}
	return MessageSource::deserializeFromString(re);
}
MessageSource Member::SendMsg(std::string msg,JNIEnv* env) {
	std::string re = LowLevelAPI::send0(std::move(msg), this, false, env);
	if (re == "E1") {
		throw MemberException(1);
	}
	if (re == "E2") {
		throw MemberException(2);
	}
	return MessageSource::deserializeFromString(re);
}

/*Ⱥ����ʵ��*/
Group::Group(unsigned long long i, unsigned long long bi, JNIEnv* env):Contact() {
	this->_type = 2;
	this->_id = i;
	this->_botid = bi;
	std::string re = LowLevelAPI::getInfoSource(this, env);
	if (re == "E1") {
		throw GroupException();
	}
	LowLevelAPI::info tmp = LowLevelAPI::info0(re);
	this->_nickOrNameCard = tmp.nickornamecard;
	this->_avatarUrl = tmp.avatarUrl;
}
Image Group::uploadImg(const std::string& filename, JNIEnv* env) {
	std::ifstream fin(filename);
	if (!fin) {
		logger->Error("�ļ�������,λ��:C++���� Group::uploadImg2(),�ļ���:" + filename);
		fin.close();
		//throw invalid_argument("NO_FILE_ERROR");
	}
	fin.close();
	std::string re = LowLevelAPI::uploadImg0(filename, this, env);
	return Image(re);
}
RemoteFile Group::sendFile(const std::string& path, const std::string& filename, JNIEnv* env)
{
	json tmp;
	tmp["path"] = path;
	tmp["filename"] = filename;
	std::string callback = Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->KSendFile, Tools::str2jstring(tmp.dump().c_str(), env), Tools::str2jstring(this->serializationToString().c_str(), env)), env);
	if (callback == "E1") throw GroupException();
	if (callback == "E2") throw IOException("�Ҳ���" + filename + "λ��:C-uploadfile");
	if (callback == "E3") throw RemoteFileException("Upload Error:·����ʽ�쳣��ӦΪ'/xxx.xxx'��'/xx/xxx.xxx'Ŀǰֻ֧��Ⱥ�ļ��͵���·��, path:" + path);
	return RemoteFile::deserializeFromString(callback);
}
RemoteFile Group::getFile(const std::string& path, const std::string& id, JNIEnv* env) {
	json tmp;
	tmp["id"] = id;
	tmp["path"] = path;
	std::string re = Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->KRemoteFileInfo, Tools::str2jstring(tmp.dump().c_str(), env), Tools::str2jstring(this->serializationToString().c_str(), env)), env);
	if (re == "E1") throw GroupException();
	if (re == "E2") throw RemoteFileException("Get Error: �ļ�·��������, path:" + path);
	return RemoteFile::deserializeFromString(re);
}
void Group::setMuteAll(bool sign, JNIEnv* env){
	std::string re = Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->KMuteGroup, (jboolean)sign, Tools::str2jstring(this->serializationToString().c_str(), env)), env);
	if (re == "Y") return;
	if (re == "E1") throw GroupException();
	if (re == "E2") throw BotException(1);
}
Member Group::getOwner(JNIEnv* env) {
	std::string re = Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->KQueryOwner, Tools::str2jstring(this->serializationToString().c_str(), env)), env);
	if (re == "E1")throw GroupException();
	return Member(stoi(re), this->id(), this->botid());
}
MessageSource Group::SendMiraiCode(std::string msg, JNIEnv* env) {
	std::string re = LowLevelAPI::send0(std::move(msg), this, true, env);
	if (re == "E1") {
		throw GroupException();
	}
	return MessageSource::deserializeFromString(re);
}
MessageSource Group::SendMsg(std::string msg, JNIEnv* env) {
	std::string re = LowLevelAPI::send0(std::move(msg), this, false, env);
	if (re == "E1") {
		throw GroupException();
	}
	return MessageSource::deserializeFromString(re);
}
std::string Group::getFileListString(const std::string& path, JNIEnv* env) {
	json temp;
	temp["id"] = "-1";
	temp["path"] = path;
	std::string tmp = Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->KRemoteFileInfo, Tools::str2jstring(temp.dump().c_str(), env), Tools::str2jstring(this->serializationToString().c_str(), env)), env);
	if (tmp == "E1") throw GroupException();
	return tmp;
}
std::vector<Group::short_info> Group::getFileList(const std::string& path, JNIEnv* env) {
	std::vector<short_info> re = std::vector<short_info>();
	std::string tmp = getFileListString(path, env);
	json root = json::parse(tmp);
	for (auto & i : root) {
		short_info t;
		t.path = i[0];
		t.id = i[1];
		re.push_back(t);
	}
	return re;
}

/*������ʵ��*/
std::string Tools::jstring2str(jstring jstr, JNIEnv* env)
{	
	if (jstr == nullptr) {
		logger->Warning("�쳣:kotlin���ؿ��ַ���");
		return ""; 
	}
	jsize len = env->GetStringLength(jstr);
	const jchar* jcstr = env->GetStringChars(jstr, nullptr);
	int size = 0;
	char* str = (char*)malloc(static_cast<size_t>(len) * 2 + 1);
	if ((size = WideCharToMultiByte(CP_ACP, 0, LPCWSTR(jcstr), len, str, len * 2 + 1, NULL, NULL)) == 0)
	{
		logger->Warning("�쳣:kotlin���ؿ��ַ���");
		return "";
	}
	env->ReleaseStringChars(jstr, jcstr);
	str[size] = 0;
	return str;
}
jstring Tools::str2jstring(const char* str, JNIEnv* env)
{	
	if (str == nullptr || *str == '\0') {
		logger->Warning("�쳣:������ַ�����kotlin");
		return nullptr;
	}
	jstring rtn = nullptr;
	int slen = strlen(str);
	unsigned short* buffer = nullptr;
	if (slen == 0)
		rtn = (env)->NewStringUTF(str);
	else
	{
		int length = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str, slen, nullptr, 0);
		buffer = (unsigned short*)malloc(static_cast<size_t>(length) * 2 + 1);
		if (MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str, slen, (LPWSTR)buffer, length) > 0)
			rtn = (env)->NewString((jchar*)buffer, length);
	}
	if (buffer)
		free(buffer);
	return rtn;
}
std::string Tools::JLongToString(jlong qqid) {
	auto id = [qqid]() -> std::string {
		std::stringstream stream;
		stream << qqid;
		std::string a;
		stream >> a;
		stream.clear();
		return a;
	};
	return id();
}
std::string Tools::VectorToString(std::vector<unsigned long long> a) {
	std::stringstream ss;
	for (size_t i = 0; i < a.size(); ++i)
	{
		if (i != 0)
			ss << ",";
		ss << a[i];
	}
	std::string s = ss.str();
	return s;
}
std::vector<unsigned long long> Tools::StringToVector(std::string temp) {
	std::vector<unsigned long long> result;
	temp.erase(temp.begin());
	temp.pop_back();
	std::regex ws_re("[,]+");
	std::vector<std::string> v(std::sregex_token_iterator(temp.begin(), temp.end(), ws_re, -1),
		std::sregex_token_iterator());
	for (auto&& s : v)
		result.push_back(atoi(s.c_str()));
	return result;
}

Contact Contact::deserializationFromString(const std::string &source) {
    json j;
    try{
        j = json::parse(source);
    }catch(json::parse_error &e){
        logger->Error("json���л����� Contact::deserializationFromString");
        logger->Error(source);
        logger->Error(e.what());
    }
    return Contact::deserializationFromJson(j);
}

Contact Contact::deserializationFromJson(nlohmann::json j) {
    return Contact(j["type"],
                   j["id"],
                   j["groupid"],
                   j["nickornamecard"],
                   j["botid"]);
}
