#include "pch.h"

/*线程管理*/

threadManager::threadManager() {
	this->_threads = std::map <std::string, t>();
}
void threadManager::setEnv(JNIEnv* e) {
	mtx.lock();
	if (!this->included(this->getThreadId())) {
		t tmp;
		tmp.e = e;
		tmp.attach = false;
		this->_threads.insert(std::pair<std::string, t>(this->getThreadId(), tmp));
	}
	else {
		this->_threads[this->getThreadId()].e = e;
	}
	mtx.unlock();
}
void threadManager::newEnv(char* threadName) {
	JNIEnv* env = nullptr;
	JavaVMAttachArgs args{
		JNIVersion,
		threadName,
		NULL
	};
	gvm->AttachCurrentThread((void**)&env, &args);
	t tmp;
	tmp.e = env;
	tmp.attach = true;
	this->_threads.insert(std::pair<std::string, t>(this->getThreadId(), tmp));
	logger->Info("refresh env");
};
void threadManager::detach() {
	mtx.lock();
	if (this->included(this->getThreadId())) {
		bool att = this->_threads[this->getThreadId()].attach;
		this->_threads.erase(this->getThreadId());
		if (att)
			gvm->DetachCurrentThread();
	}
	mtx.unlock();
}
bool threadManager::included(std::string id) {
	if (this->_threads.empty() || this->_threads.count(id) == 0)
		return false;
	return true;
}
JNIEnv* threadManager::getEnv() {
	mtx.lock();
	if (!this->included(getThreadId())) {
		this->newEnv();
	}
	JNIEnv* tmp = this->_threads[this->getThreadId()].e;
	mtx.unlock();
	return tmp;
}

/*
日志类实现
throw: InitException 即找不到签名
*/

void Logger::init(JNIEnv* env) {
	this->CPP_lib = (jclass)(env->NewGlobalRef(env->FindClass("tech/eritquearcus/miraicp/CPP_lib")));
	this->log = env->GetStaticMethodID(this->CPP_lib, "KSendLog", "(Ljava/lang/String;I)V");
	if (this->CPP_lib == NULL) {
		throw InitException("logger初始化错误", 1);
	}
	if (this->log == NULL) {
		throw InitException("logger初始化错误", 2);
	}
}
void Logger::log0(std::string log, JNIEnv* env, int level) {
	env->CallStaticVoidMethod(config->CPP_lib, this->log, tools.str2jstring(log.c_str()), (jint)level);
}
void Logger::Warning(std::string log, JNIEnv* env) {
	this->log0(log, env, 1);
}
void Logger::Error(std::string log, JNIEnv* env) {
	this->log0(log, env, 2);
}
void Logger::Info(std::string log, JNIEnv* env) {
	this->log0(log, env, 0);
}
Logger::~Logger() {
	manager->getEnv()->DeleteGlobalRef(this->CPP_lib);
}

/*
配置类实现
throw: InitxException 即找不到对应签名
*/

void Config::Init(JNIEnv* env) {	
	// int x = (int)env->EnsureLocalCapacity((jint)22);
	this->initexception = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("java/lang/NoSuchMethodException")));
	this->CPP_lib = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("tech/eritquearcus/miraicp/CPP_lib")));
	if (this->CPP_lib == NULL) {
		throw InitException("初始化错误", 1);
	}
	/*
	this->Send = env->GetStaticMethodID(CPP_lib, "KSend", "(Ljava/lang/String;Z)Ljava/lang/String;");
	this->Query = env->GetStaticMethodID(CPP_lib, "KQueryImgUrl", "(Ljava/lang/String;Ljava/lang/String;)V");
	this->refreshInfo = env->GetStaticMethodID(CPP_lib, "KRefreshInfo", "(Ljava/lang/String;)Ljava/lang/String;");
	this->Schedule = env->GetStaticMethodID(CPP_lib, "KSchedule", "(JLjava/lang/String;)V");
	this->Mute = env->GetStaticMethodID(CPP_lib, "KMuteM", "(JJI)Ljava/lang/String;");
	this->QueryP = env->GetStaticMethodID(CPP_lib, "KQueryM", "(JJ)Ljava/lang/String;");
	this->KickM = env->GetStaticMethodID(CPP_lib, "KKickM", "(JJLjava/lang/String;)Ljava/lang/String;");
	this->recallMsgM = env->GetStaticMethodID(CPP_lib, "KRecall", "(Ljava/lang/String;)Ljava/lang/String;");
	this->QueryML = env->GetStaticMethodID(CPP_lib, "KQueryML", "(J)Ljava/lang/String;");
	this->uploadImg= env->GetStaticMethodID(config->CPP_lib, "KUploadImg", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
	this->muteAll = env->GetStaticMethodID(config->CPP_lib, "KMuteGroup", "(JZ)Ljava/lang/String;");
	this->getowner = env->GetStaticMethodID(config->CPP_lib, "KQueryOwner", "(J)Ljava/lang/String;");
	this->buildforward = env->GetStaticMethodID(config->CPP_lib, "KBuildforward", "(Ljava/lang/String;)Ljava/lang/String;");
	this->NFR = env->GetStaticMethodID(config->CPP_lib, "KNfroperation", "(Ljava/lang/String;Z)Ljava/lang/String;");
	this->GI = env->GetStaticMethodID(config->CPP_lib, "KGioperation", "(Ljava/lang/String;Z)Ljava/lang/String;");
	this->queryBotFriends = env->GetStaticMethodID(config->CPP_lib, "KQueryBFL", "(J)Ljava/lang/String;");
	this->queryBotGroups = env->GetStaticMethodID(config->CPP_lib, "KQueryBGL", "(J)Ljava/lang/String;");
	this->uploadFile = env->GetStaticMethodID(config->CPP_lib, "KSendFile", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
	this->queryFile = env->GetStaticMethodID(config->CPP_lib, "KRemoteFileInfo", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");*/
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

SchedulingEvent::SchedulingEvent(std::string str) {
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

//消息源
MessageSource::MessageSource(std::string t) {
	this->source = t;
	const auto rawJsonLength = static_cast<int>(t.length());
	Json::String err;
	Json::Value root;
	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(t.c_str(), t.c_str() + rawJsonLength, &root,
		&err)) {
		//error
		throw APIException("JSON reader error,位置:C-MessageSource");

	}
	this->ids = root["ids"].toStyledString();
	this->ids = tools.replace(this->ids, "\n", "");
	this->ids = tools.replace(this->ids, " ", "");
	this->internalids = root["internalIds"].toStyledString();
	this->internalids = tools.replace(this->internalids, "\n", "");
	this->internalids = tools.replace(this->internalids, " ", "");
}
void MessageSource::recall() {
	std::string re = tools.jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KRecall,
		tools.str2jstring(this->toString().c_str())));
	if (re == "Y") return;
	if (re == "E1") throw BotException(1);
	if (re == "E2") throw RecallException();
}

//远程文件(群文件)
RemoteFile RemoteFile::buildFromString(std::string source) {
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
RemoteFile::RemoteFile(std::string i, unsigned int ii, std::string n, long long s, dinfo d, finfo f) {
	this->_id = i;
	this->_internalid = ii;
	this->_name = n;
	this->_size = s;
	this->_dinfo = d;
	this->_finfo = f;
}

//发送这个聊天记录
void ForwardMessage::sendTo(Contact* c, JNIEnv* env) {
	Json::Value temp;
	temp["id"] = c->id();
	temp["groupid"] = c->groupid();
	temp["type"] = c->type();
	temp["content"] = sendmsg;
	std::string re = tools.jstring2str((jstring)env->
		CallStaticObjectMethod(config->CPP_lib, config->KBuildforward,
			tools.str2jstring(tools.JsonToString(temp).c_str(), env),
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
	if (re == "E3") throw APIException("参数错误");
}
ForwardMessage::ForwardMessage(Contact* c, std::initializer_list<ForwardNode> nodes) {
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

/*图片类实现*/
Image::Image(std::string imageId) {
	this->Query = config->KQueryImgUrl;
	this->id = imageId;
}
std::string Image::queryURL(JNIEnv* env) {
	return tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, this->Query, tools.str2jstring(this->id.c_str())));
}
std::vector<std::string> Image::GetImgIdsFromMiraiCode(std::string MiraiCode) {
	std::vector<std::string> result = std::vector<std::string>();
	std::string temp = MiraiCode;
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

/*好友类实现*/
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
Image Friend::uploadImg(std::string filename, JNIEnv* env) {
	std::ifstream fin(filename);
	if (!fin) {
		logger->Error("文件不存在,位置:C-Friend::uploadImg(),文件名:" + filename);
		fin.close();
		throw std::invalid_argument("NO_FILE_ERROR");
	}
	fin.close();
	std::string re = LowLevelAPI::uploadImg0(filename, this, env);
	return Image(re);
}
MessageSource Friend::SendMiraiCode(std::string msg, JNIEnv* env) {
	std::string re = LowLevelAPI::send0(msg, this, true, env);
	if (re == "E1") {
		throw FriendException();
	}
	return MessageSource(re);
}
MessageSource Friend::SendMsg(std::string msg, JNIEnv* env){
	std::string re = LowLevelAPI::send0(msg, this, false, env);
	if (re == "E1") {
		throw FriendException();
	}
	return MessageSource(re);
}

/*成员类实现*/
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
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->KQueryM, tools.str2jstring(this->toString().c_str(), env)), env);
	if (re == "E1") {
		throw MemberException(1);
	}
	if(re == "E2"){
		throw MemberException(2);
	}
	return stoi(re);
}
void Member::Mute(int time, JNIEnv* env) {
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, this->Mute_id, (jint)time, tools.str2jstring(this->toString().c_str(), env)), env);
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
void Member::Kick(std::string reason, JNIEnv* env) {
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, this->KickM, tools.str2jstring(reason.c_str(), env), tools.str2jstring(this->toString().c_str(), env)), env);
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
Image Member::uploadImg(std::string filename, JNIEnv* env) {
	std::ifstream fin(filename);
	if (!fin) {
		fin.close();
		throw IOException("文件不存在,位置:C++部分 uploadImg2Group(),文件名:" + filename);
	}
	fin.close();
	std::string re = LowLevelAPI::uploadImg0(filename, this, env);
	return Image(re);
}
MessageSource Member::SendMiraiCode(std::string msg, JNIEnv* env) {
	std::string re = LowLevelAPI::send0(msg, this, true, env);
	if (re == "E1") {
		throw MemberException(1);
	}
	if (re == "E2") {
		throw MemberException(2);
	}
	return MessageSource(re);
}
MessageSource Member::SendMsg(std::string msg,JNIEnv* env) {
	std::string re = LowLevelAPI::send0(msg, this, false, env);
	if (re == "E1") {
		throw MemberException(1);
	}
	if (re == "E2") {
		throw MemberException(2);
	}
	return MessageSource(re);
}

/*群聊类实现*/
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
Image Group::uploadImg(std::string filename, JNIEnv* env) {
	std::ifstream fin(filename);
	if (!fin) {
		logger->Error("文件不存在,位置:C++部分 Group::uploadImg2(),文件名:" + filename);
		fin.close();
		//throw invalid_argument("NO_FILE_ERROR");
	}
	fin.close();
	std::string re = LowLevelAPI::uploadImg0(filename, this, env);
	return Image(re);
}
RemoteFile Group::sendFile(std::string path, std::string filename, JNIEnv* env)
{
	Json::Value tmp;
	tmp["path"] = path;
	tmp["filename"] = filename;
	std::string callback = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->KSendFile, tools.str2jstring(tools.JsonToString(tmp).c_str(), env), tools.str2jstring(this->toString().c_str(), env)), env);
	if (callback == "E1") throw GroupException();
	if (callback == "E2") throw IOException("找不到" + filename + "位置:C-uploadfile");
	if (callback == "E3") throw RemoteFileException("Upload Error:路径格式异常，应为'/xxx.xxx'或'/xx/xxx.xxx'目前只支持群文件和单层路径, path:" + path);
	return RemoteFile::buildFromString(callback);
}
RemoteFile Group::getFile(std::string path, std::string id, JNIEnv* env) {
	Json::Value tmp;
	tmp["id"] = id;
	tmp["path"] = path;
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->KRemoteFileInfo, tools.str2jstring(tools.JsonToString(tmp).c_str(), env), tools.str2jstring(this->toString().c_str(), env)), env);
	if (re == "E1") throw GroupException();
	if (re == "E2") throw RemoteFileException("Get Error: 文件路径不存在, path:" + path);
	return RemoteFile::buildFromString(re);
}
void Group::setMuteAll(bool sign, JNIEnv* env){
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->KMuteGroup, (jboolean)sign, tools.str2jstring(this->toString().c_str(), env)), env);
	if (re == "Y") return;
	if (re == "E1") throw GroupException();
	if (re == "E2") throw BotException(1);
}
Member Group::getOwner(JNIEnv* env) {
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->KQueryOwner, tools.str2jstring(this->toString().c_str(), env)), env);
	if (re == "E1")throw GroupException();
	return Member(stoi(re), this->id(), this->botid());
}
MessageSource Group::SendMiraiCode(std::string msg, JNIEnv* env) {
	std::string re = LowLevelAPI::send0(msg, this, true, env);
	if (re == "E1") {
		throw GroupException();
	}
	return MessageSource(re);
}
MessageSource Group::SendMsg(std::string msg, JNIEnv* env) {
	std::string re = LowLevelAPI::send0(msg, this, false, env);
	if (re == "E1") {
		throw GroupException();
	}
	return MessageSource(re);
}
std::string Group::getFileListString(std::string path, JNIEnv* env) {
	Json::Value temp;
	temp["id"] = "-1";
	temp["path"] = path;
	std::string tmp = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->KRemoteFileInfo, tools.str2jstring(tools.JsonToString(temp).c_str(), env), tools.str2jstring(this->toString().c_str(), env)), env);
	if (tmp == "E1") throw GroupException();
	return tmp;
}
std::vector<Group::short_info> Group::getFileList(std::string path, JNIEnv* env) {
	std::vector<short_info> re = std::vector<short_info>();
	std::string tmp = getFileListString(path, env);
	Json::Value root = tools.StringToJson(tmp);
	for (int i = 0; i < root.size(); i++) {
		short_info t;
		t.path = root[i][0].asCString();
		t.id = root[i][1].asCString();
		re.push_back(t);
	}
	return re;
}

/*工具类实现*/
std::string Tools::jstring2str(jstring jstr, JNIEnv* env)
{	
	if (jstr == NULL) { 
		logger->Warning("异常:kotlin返回空字符串");
		return ""; 
	}
	jsize len = env->GetStringLength(jstr);
	const jchar* jcstr = env->GetStringChars(jstr, NULL);
	int size = 0;
	char* str = (char*)malloc(static_cast<size_t>(len) * 2 + 1);
	if ((size = WideCharToMultiByte(CP_ACP, 0, LPCWSTR(jcstr), len, str, len * 2 + 1, NULL, NULL)) == 0)
	{
		logger->Warning("异常:kotlin返回空字符串");
		return "";
	}
	env->ReleaseStringChars(jstr, jcstr);
	str[size] = 0;
	return str;
}
jstring Tools::str2jstring(const char* str, JNIEnv* env)
{	
	if (str == NULL || *str == '\0') { 
		logger->Warning("异常:传入空字符串到kotlin");
		return NULL; 
	}
	jstring rtn = 0;
	int slen = strlen(str);
	unsigned short* buffer = 0;
	if (slen == 0)
		rtn = (env)->NewStringUTF(str);
	else
	{
		int length = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str, slen, NULL, 0);
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
std::string Tools::JsonToString(const Json::Value& root)
{
	std::ostringstream stream;
	Json::StreamWriterBuilder stream_builder;
	stream_builder.settings_["emitUTF8"] = true;//Config emitUTF8
	std::unique_ptr<Json::StreamWriter> writer(stream_builder.newStreamWriter());
	writer->write(root, &stream);
	return stream.str();
}
Json::Value Tools::StringToJson(std::string Rcontent) {
	const auto rawJsonLength = static_cast<int>(Rcontent.length());
	Json::String err;
	Json::Value root;
	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(Rcontent.c_str(), Rcontent.c_str() + rawJsonLength, &root,
		&err)) {
		//error
		return NULL;
	}
	return root;
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