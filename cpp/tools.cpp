#include "pch.h"
#include "utf8.h"
#include "tools.h"


using json = nlohmann::json;

/*线程管理*/

threadManager::threadManager() {
	this->_threads = std::map<std::string, t>();
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
			const_cast<char*>(threadName),
			nullptr
	};
	gvm->AttachCurrentThread((void**)&env, &args);
	t tmp{ env, true };
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
日志类实现
throw: InitException 即找不到签名
*/

void Logger::init(JNIEnv* env) {
	this->log = env->GetStaticMethodID(config->CPP_lib, "KSendLog", "(Ljava/lang/String;I)V");
}

void Logger::registerHandle(Logger::action action) {
	this->loggerhandler.action = std::move(action);
}

void Logger::setHandleState(bool state) {
	this->loggerhandler.enable = state;
}

void Logger::log0(const std::string& content, unsigned long long botid, int level, JNIEnv* env) {
	if (this->loggerhandler.enable)
		this->loggerhandler.action(content, level);
	json j;
	j["botid"] = botid;
	j["log"] = content;
	env->CallStaticVoidMethod(config->CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint)level);
}

void Logger::Warning(const std::string& content, unsigned long long botid, JNIEnv* env) {
	this->log0(content, botid, 1, env);
}

void Logger::Error(const std::string& content, unsigned long long botid, JNIEnv* env) {
	this->log0(content,botid, 2, env);
}

void Logger::Info(const std::string& content, unsigned long long botid, JNIEnv* env) {
	this->log0(content, botid, 0, env);
}

/*
配置类实现
throw: InitxException 即找不到对应签名
*/

void Config::Init(JNIEnv* env) {
	this->CPP_lib = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("tech/eritquearcus/miraicp/shared/CPP_lib")));
	if (this->CPP_lib == nullptr) {
		throw APIException("初始化错误，找不到CPP_lib类");
	}
    this->KOperation = env->GetStaticMethodID(CPP_lib, "KOperation", "(Ljava/lang/String;)Ljava/lang/String;");
}

Config::~Config() {
	manager->getEnv()->DeleteGlobalRef(this->CPP_lib);
}

std::string Config::koperation(int type, const json& data, JNIEnv* env) {
    json j;
    j["type"] = type;
    j["data"] = data;
    jstring re = (jstring)env->CallStaticObjectMethod(this->CPP_lib, this->KOperation, Tools::str2jstring(j.dump().c_str(), env));
    return Tools::jstring2str(re, env);
}

Event::~Event() {
	Node0* temp[] = { GMHead, PMHead, GHead, NFHead, MJHead, MLHead, RHead, BHead };
	for (Node0* ptr : temp) {
		Node0* now = ptr;
		Node0* t = nullptr;
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

void MessageSource::recall(JNIEnv* env) {
    json j;
    j["source"] = this->serializeToString();
	std::string re = config->koperation(config->Recall, j, env);
	if (re == "Y") return;
	if (re == "E1") throw BotException();
	if (re == "E2") throw RecallException();
}

MessageSource::MessageSource(std::string ids, std::string  internalids, const std::string& source) : ids(std::move(
	ids)), internalids(std::move(internalids)), source(source) {}

std::string MessageSource::serializeToString() {
	return source;
}

MessageSource MessageSource::deserializeFromString(const std::string& source) {
	json j = json::parse(source);
	try {
		return MessageSource(j["ids"].dump(), j["internalIds"].dump(), source);
	}
	catch (json::type_error& e) {
		logger->Error("消息源序列化出错，格式不符合(MessageSource::deserializeFromString)");
		logger->Error(source);
		logger->Error(e.what());
		throw e;
	}
}

//远程文件(群文件)
RemoteFile RemoteFile::deserializeFromString(const std::string& source) {
	json j;
	try {
		j = json::parse(source);
	}
	catch (json::parse_error& e) {
		logger->Error("格式化json失败，RemoteFile::deserializeFromString");
		logger->Error(source);
		logger->Error(e.what());
		throw e;
	}
	try {
		struct dinfo d {
			j["dinfo"]["url"],
				j["dinfo"]["md5"],
				j["dinfo"]["sha1"]
		};
		struct finfo f {
			j["finfo"]["size"],
				j["finfo"]["uploaderid"],
				j["finfo"]["downloadtime"],
				j["finfo"]["uploadtime"],
				j["finfo"]["lastmodifytime"]
		};
		return RemoteFile(j["id"], j["internalid"], j["name"], j["finfo"]["size"], j["path"], d, f);
	}
	catch (json::type_error& e) {
		logger->Error("json格式化失败，位置:RemoteFile");
		logger->Error(source);
		logger->Error(e.what());
		throw e;
	}
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
	return j.dump();
}

//发送这个聊天记录
MessageSource ForwardMessage::sendTo(Contact* c, JNIEnv* env){
	json temp;
	json text;
	text["id"] = c->id();
	text["groupid"] = c->groupid();
	text["type"] = c->type();
	text["content"] = sendmsg;
	temp["text"] = text.dump();
	temp["botid"] = c->botid();
	std::string re = config->koperation(config->Buildforward, temp, env);
    ErrorHandle(re);
	//TODO:https://github.com/mamoe/mirai/issues/1371
	return MessageSource::deserializeFromString(re);
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

/*图片类实现*/
Image::Image(std::string imageId) {
	this->id = std::move(imageId);
}

std::string Image::queryURL(JNIEnv* env) const {
    json j;
    j["id"] = this->id;
	std::string re = config->koperation(config->QueryImgUrl, j, env);
	if(re == "E1")
	    throw RemoteAssetException("图片id格式错误");
	return re;
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
	return "[mirai:image:" + Tools::escapeToMiraiCode(this->id) + "]";
}

MessageSource MessageSource::quoteAndSendMiraiCode(const std::string& content, unsigned long long groupid, JNIEnv* env){
    json obj;
    json sign;
    obj["messageSource"] = this->serializeToString();
    obj["msg"] = content;
    sign["MiraiCode"] = true;
    sign["groupid"] = groupid;
    obj["sign"] = sign.dump();
    std::string re = config->koperation(config->SendWithQuote, obj, env);
    ErrorHandle(re);
    return MessageSource::deserializeFromString(re);
}

MessageSource MessageSource::quoteAndSendMsg(const std::string& content, unsigned long long groupid, JNIEnv* env){
    json obj;
    json sign;
    obj["messageSource"] = this->serializeToString();
    obj["msg"] = content;
    sign["MiraiCode"] = false;
    sign["groupid"] = groupid;
    obj["sign"] = sign.dump();
    std::string re = config->koperation(config->SendWithQuote, obj, env);
    ErrorHandle(re);
    return MessageSource::deserializeFromString(re);
}

MessageSource Contact::sendMiraiCode(std::string msg, JNIEnv* env) {
    if(msg.empty())
        throw IllegalArgumentException("参数不能为空, 位置: Contact::SendMiraiCode");
    std::string re = LowLevelAPI::send0(std::move(msg), this, true, env);
    ErrorHandle(re, "reach a error area, Contact::SendMiraiCode");
    return MessageSource::deserializeFromString(re);
}

MessageSource Contact::sendMsg(std::string msg, JNIEnv* env) {
    if(msg.empty()) {
        logger->Error("警告:发送空信息, 位置: Contact::SendMsg");
        throw IllegalArgumentException("参数不能为空, 位置: Contact::SendMsg");
    }
    std::string re = LowLevelAPI::send0(std::move(msg), this, false, env);
    ErrorHandle(re, "reach a error area, Contact::SendMiraiCode");
    return MessageSource::deserializeFromString(re);
}

Image Contact::uploadImg(const std::string& path, JNIEnv* env) {
    std::string re = LowLevelAPI::uploadImg0(path, this, env);
    ErrorHandle(re);
    if (re == "E2")
        throw UploadException("上传图片大小超过30MB,路径:" + path);
    return Image(re);
}

/*好友类实现*/
Friend::Friend(unsigned long long id, unsigned long long botid, JNIEnv* env) : Contact() {
	this->_type = 1;
	this->_id = id;
	this->_botid = botid;
    refreshInfo(env);
}

/*成员类实现*/
Member::Member(unsigned long long id, unsigned long long groupid, unsigned long long botid, JNIEnv* env) : Contact() {
	this->_type = 3;
	this->_id = id;
	this->_groupid = groupid;
	this->_botid = botid;
	refreshInfo(env);
}

unsigned int Member::getPermission(JNIEnv* env) {
    json j;
    j["contactSource"] = this->serializationToString();
	std::string re = config->koperation(config->QueryM, j, env);
    ErrorHandle(re);
	return stoi(re);
}

void Member::Mute(int time, JNIEnv* env) {
    json j;
    j["time"] = time;
    j["contactSource"] = this->serializationToString();
	std::string re = config->koperation(config->MuteM, j, env);
	ErrorHandle(re);
	if (re == "E3") {
		throw BotException();
	}
	if (re == "E4") {
		throw MuteException();
	}
}

void Member::Kick(const std::string& reason, JNIEnv* env) {
    json j;
    j["message"] = reason;
    j["contactSource"] = this->serializationToString();
	std::string re = config->koperation(config->KickM, j, env);
	ErrorHandle(re);
	if (re == "E3") {
		throw BotException();
	}
}

/*群聊类实现*/
Group::Group(unsigned long long i, unsigned long long bi, JNIEnv* env) : Contact() {
	this->_type = 2;
	this->_id = i;
	this->_botid = bi;
	refreshInfo(env);
}

void Group::updateSetting(JNIEnv* env){
    json j;
    json tmp;
    j["name"] = this->setting.name;
    j["entranceAnnouncement"] = this->setting.entranceAnnouncement;
    j["isMuteAll"] = this->setting.isMuteAll;
    j["isAllowMemberInvite"] = this->setting.isAllowMemberInvite;
    j["isAutoApproveEnabled"] = this->setting.isAutoApproveEnabled;
    j["isAnonymousChatEnabled"] = this->setting.isAnonymousChatEnabled;
    tmp["source"] = j.dump();
    tmp["contactSource"] = this->serializationToString();
    std::string re = config->koperation(config->GroupSetting, tmp, env);
    if(re == "E1")
        throw BotException();
    refreshInfo(env);
}

RemoteFile Group::sendFile(const std::string& path, const std::string& filename, JNIEnv* env) {
	json tmp;
	json source;
	source["path"] = path;
	source["filename"] = filename;
	tmp["source"] = source.dump();
	tmp["contactSource"] = this->serializationToString();
	std::string callback = config->koperation(config->SendFile, tmp, env);
    ErrorHandle(callback);
	if (callback == "E2") throw UploadException("找不到" + filename + "位置:C-uploadfile");
	if (callback == "E3")
		throw UploadException("Upload Error:路径格式异常，应为'/xxx.xxx'或'/xx/xxx.xxx'目前只支持群文件和单层路径, path:" + path);
	return RemoteFile::deserializeFromString(callback);
}

RemoteFile Group::getFile(const std::string& path, const std::string& id, JNIEnv* env) {
    // source 参数
	json tmp;
	json j;
	tmp["id"] = id;
	tmp["path"] = path;
	j["source"] = tmp.dump();
	j["contactSource"] = this->serializationToString();
    std::string re = config->koperation(config->RemoteFileInfo, j, env);
	ErrorHandle(re);
	if (re == "E2") throw RemoteAssetException("Get Error: 文件路径不存在, path:" + path);
	return RemoteFile::deserializeFromString(re);
}

Member Group::getOwner(JNIEnv* env) {
    json j;
    j["contactSource"] = this->serializationToString();
	std::string re = config->koperation(config->QueryOwner, j, env);
	ErrorHandle(re);
	return Member(stoi(re), this->id(), this->botid());
}

std::string Group::getFileListString(const std::string& path, JNIEnv* env) {
	json temp;
	json j;
	temp["id"] = "-1";
	temp["path"] = path;
	j["source"] = temp.dump();
	j["contactSource"] = this->serializationToString();
	std::string re = config->koperation(config->RemoteFileInfo, j, env);
    ErrorHandle(re);
	return re;
}

std::vector<Group::file_short_info> Group::getFileList(const std::string& path, JNIEnv* env) {
	std::vector<file_short_info> re = std::vector<file_short_info>();
	std::string tmp = getFileListString(path, env);
	json root = json::parse(tmp);
	for (auto& i : root) {
		file_short_info t;
		t.path = i[0];
		t.id = i[1];
		re.push_back(t);
	}
	return re;
}

/*工具类实现*/
std::string Tools::jstring2str(jstring jStr, JNIEnv* env) {
    if(!jStr) {
        logger->Error("警告:kotlin部分返回空字符串, 位置:Tools::jstring2str");
        return "";
    }
    std::u16string s = reinterpret_cast<const char16_t*>(env->GetStringChars(jStr, nullptr));
    if(s.length() == 0) {
        logger->Error("警告:kotlin部分返回空字符串, 位置:Tools::jstring2str");
        return "";
    }
    std::string x;
    utf8::utf16to8(s.begin(), s.end(), std::back_inserter(x));
    return x;
}

jstring Tools::str2jstring(const char* stra, JNIEnv* env) {
    if(!stra){
        logger->Error("警告:C++部分传入空字符串，位置:Tools::str2jstring");
    }
    std::string str(stra);
    std::vector<unsigned short> utf16line;
    utf8::utf8to16(str.begin(), str.end(), std::back_inserter(utf16line));
    auto* c = new jchar[utf16line.size()];
    for(int i = 0; i<utf16line.size(); i++){
        c[i] = utf16line[i];
    }
    return env->NewString((const jchar*)c, (jsize)utf16line.size());
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
	for (size_t i = 0; i < a.size(); ++i) {
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

std::string Tools::escapeFromMiraiCode(const std::string &s) {
    //[	\[
    //]	\]
    //:	\:
    //,	\,
    //\	\\ /
    return Tools::replace(
            Tools::replace(
                    Tools::replace(
                            Tools::replace(
                                    Tools::replace(s,
                                                   "\\\\", "\\"),
                                    "\\,", ","),
                            "\\:", ":"),
                    "\\]", "]"), "\\[", "[");
}

std::string Tools::escapeToMiraiCode(const std::string &s) {
    //[	\[
    //]	\]
    //:	\:
    //,	\,
    //\	\\ /
    return Tools::replace(Tools::replace(Tools::replace(Tools::replace(Tools::replace(s,
                                                                                      "\\", "\\\\"),
                                                                       ",", "\\,"),
                                                        ":", "\\:"),
                                         "]", "\\]"), "[", "\\[");
    return std::string();
}

Contact Contact::deserializationFromString(const std::string& source) {
	json j;
	try {
		j = json::parse(source);
	}
	catch (json::parse_error& e) {
		logger->Error("json序列化错误 Contact::deserializationFromString");
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

MessageSource Contact::sendVoice(const std::string& path, JNIEnv* env) {
    json j;
    json source;
    source["path"] = path;
    j["source"] = source.dump();
    j["contactSource"] = this->serializationToString();
    std::string re = config->koperation(config->Voice, j, env);
    ErrorHandle(re);
    if(re == "E1")
        throw UploadException("文件格式不对(必须为.amr/.silk)或文件不存在");
    return MessageSource::deserializeFromString(re);
}
