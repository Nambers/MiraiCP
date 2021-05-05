#include "pch.h"

/*
配置类实现
throw: InitxException 即找不到对应签名
*/
void Config::Init(JNIEnv* env) {	
	this->initexception = env->FindClass("java/lang/NoSuchMethodException");
	this->CPP_lib = (jclass)env->NewGlobalRef(env->FindClass("tech/eritquearcus/miraicp/CPP_lib"));
	if (this->CPP_lib == NULL) {
		throw InitException("初始化错误", 1);
	}
	this->Send = env->GetStaticMethodID(CPP_lib, "KSend", "(Ljava/lang/String;Z)Ljava/lang/String;");
	this->Query = env->GetStaticMethodID(CPP_lib, "KQueryImgUrl", "(Ljava/lang/String;)Ljava/lang/String;");
	this->NickorName = env->GetStaticMethodID(CPP_lib, "KGetNickOrNameCard", "(Ljava/lang/String;)Ljava/lang/String;");
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
}
Config::~Config() {
	manager->getEnv()->DeleteGlobalRef(this->CPP_lib);
}

threadManager::threadManager() {
	this->_threads = std::map <std::string, t>();
}
void threadManager::setEnv(JNIEnv* e){
	mtx.lock();
	if (!this->included(this->getThreadId())) {
		t tmp;
		tmp.e = e;
		tmp.attach = false;
		this->_threads.insert(std::pair<std::string, t>(this->getThreadId(), tmp));
	}else{
		this->_threads[this->getThreadId()].e = e;
	}
	mtx.unlock();
}
void threadManager::newEnv(char* threadName){
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
void threadManager::detach(){
	bool att = false;
	mtx.lock();
	if (this->included(this->getThreadId())) {
		att = this->_threads[this->getThreadId()].attach;
		this->_threads.erase(this->getThreadId());
		if (att)
			gvm->DetachCurrentThread();
	}
	mtx.unlock();
}
JNIEnv* threadManager::getEnv(){
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
void Logger::init(JNIEnv* env){
	this->CPP_lib = (jclass)(env->NewGlobalRef(env->FindClass("tech/eritquearcus/miraicp/CPP_lib")));
	this->log = env->GetStaticMethodID(this->CPP_lib, "KSendLog", "(Ljava/lang/String;I)V");
	if (this->CPP_lib == NULL) {
		throw InitException("logger初始化错误", 1);
	}
	if (this->log== NULL) {
		throw InitException("logger初始化错误", 2);
	}
}
void Logger::send0(std::string log, JNIEnv* env, int level) {
	env->CallStaticVoidMethod(config->CPP_lib, this->log, tools.str2jstring(log.c_str()), (jint)level);
}
void Logger::Warning(std::string log, JNIEnv* env) {
	this->send0(log, env, 1);
}
void Logger::Error(std::string log, JNIEnv* env) {
	this->send0(log, env, 2);
}
void Logger::Info(std::string log, JNIEnv* env) {
	this->send0(log, env, 0);
}
Logger::~Logger() {
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
//发送这个聊天记录
void ForwardMessage::sendTo(Contact* c, JNIEnv* env) {
	Json::Value temp;
	temp["id"] = c->id();
	temp["groupid"] = c->groupid();
	temp["type"] = c->type();
	temp["content"] = sendmsg;
	std::string re = tools.jstring2str((jstring)env->
		CallStaticObjectMethod(config->CPP_lib, config->buildforward,
			tools.str2jstring(tools.JsonToString(temp).c_str())));
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

/*图片类实现*/
Image::Image(std::string imageId) {
	this->Query = config->Query;
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
Friend::Friend(unsigned long long id, JNIEnv* env) {
	this->_type = 1;
	this->_id = id;
	std::string temp = LowLevelAPI::getnickornamecard0(this, env);
	if (temp == "E1") {
		throw FriendException();
	}
	this->_nickOrNameCard = temp;
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
Member::Member(unsigned long long id, unsigned long long groupid, JNIEnv* env) {
	this->_type = 3;
	this->_id = id;
	this->Mute_id = config->Mute;
	this->_groupid = groupid;
	this->Query_permission = config->QueryP;
	this->KickM = config->KickM;
	std::string temp = LowLevelAPI::getnickornamecard0(this, env);
	this->_nickOrNameCard = temp;
	this->permission = getPermission();
	if (temp == "E1") {
		throw MemberException(1);
	}
	if (temp == "E2") {
		throw MemberException(2);
	}
}
unsigned int Member::getPermission(JNIEnv* env){
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->QueryP, (jlong)id(), (jlong)groupid()));
	if (re == "E1") {
		throw MemberException(1);
	}
	if(re == "E2"){
		throw MemberException(2);
	}
	return stoi(re);
}
void Member::Mute(int time, JNIEnv* env) {
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, this->Mute_id, (jlong)this->id(), (jlong)this->groupid(), (jint)time));
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
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, this->KickM, (jlong)id(), (jlong)groupid(), tools.str2jstring(reason.c_str())));
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
Group::Group(unsigned long long id, JNIEnv* env) {
	this->_type = 2;
	this->_id = id;
	std::string re = LowLevelAPI::getnickornamecard0(this, env);
	if (re == "E1") {
		throw GroupException();
	}
	this->_nickOrNameCard = re;
}
std::vector<unsigned long long> Group::getMemberList() {
	std::string re = tools.jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib,
		config->QueryML,
		(jlong)this->id()));
	if (re == "E1") {
		throw GroupException();
	}
	std::vector<unsigned long long> result;
	std::string temp = re;
	temp.erase(temp.begin());
	temp.pop_back();
	std::regex ws_re("[,]+");
	std::vector<std::string> v(std::sregex_token_iterator(temp.begin(), temp.end(), ws_re, -1),
		std::sregex_token_iterator());
	for (auto&& s : v)
		result.push_back(atoi(s.c_str()));
	return result;
}
std::string Group::MemberListToString() {
	std::vector<unsigned long long> a = getMemberList();
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
void Group::setMuteAll(bool sign, JNIEnv* env){
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->muteAll, (jlong)this->id(), (jboolean)sign));
	if (re == "Y") return;
	if (re == "E1") throw GroupException();
	if (re == "E2") throw BotException(1);
}
Member Group::getOwner(JNIEnv* env) {
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->getowner, (jlong)this->id()));
	if (re == "E1")throw GroupException();
	return Member(stoi(re), this->id());
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