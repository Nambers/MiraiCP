#include "pch.h"

/*
������ʵ��
throw: InitxException ���Ҳ�����Ӧǩ��
*/
void Config::Init(JNIEnv* env) {	
	this->initexception = env->FindClass("java/lang/NoSuchMethodException");
	this->CPP_lib = (jclass)env->NewGlobalRef(env->FindClass("tech/eritquearcus/miraicp/CPP_lib"));
	if (this->CPP_lib == NULL) {
		throw InitException("��ʼ������", 1);
	}
	this->Query = env->GetStaticMethodID(CPP_lib, "QueryImgUrl", "(Ljava/lang/String;)Ljava/lang/String;");
	this->SendMsg2F = env->GetStaticMethodID(CPP_lib, "SendPrivateMSG", "(Ljava/lang/String;J)Ljava/lang/String;");
	this->SendMsg2FM = env->GetStaticMethodID(CPP_lib, "SendPrivateMSGM", "(Ljava/lang/String;J)Ljava/lang/String;");
	this->NickorNameF = env->GetStaticMethodID(CPP_lib, "GetNick", "(J)Ljava/lang/String;");
	this->SendMsg2M = env->GetStaticMethodID(CPP_lib, "SendPrivateM2M", "(Ljava/lang/String;JJ)Ljava/lang/String;");
	this->SendMsg2MM = env->GetStaticMethodID(CPP_lib, "SendPrivateM2MM", "(Ljava/lang/String;JJ)Ljava/lang/String;");
	this->NickorNameM = env->GetStaticMethodID(CPP_lib, "GetNameCard", "(JJ)Ljava/lang/String;");
	this->SendMsg2G = env->GetStaticMethodID(CPP_lib, "SendGroup", "(Ljava/lang/String;J)Ljava/lang/String;");
	this->SendMsg2GM = env->GetStaticMethodID(CPP_lib, "SendGroupM", "(Ljava/lang/String;J)Ljava/lang/String;");
	this->Schedule = env->GetStaticMethodID(CPP_lib, "schedule", "(JLjava/lang/String;)V");
	this->Mute = env->GetStaticMethodID(CPP_lib, "muteM", "(JJI)Ljava/lang/String;");
	this->QueryP = env->GetStaticMethodID(CPP_lib, "queryM", "(JJ)Ljava/lang/String;");
	this->KickM = env->GetStaticMethodID(CPP_lib, "kickM", "(JJLjava/lang/String;)Ljava/lang/String;");
	this->recallMsgM = env->GetStaticMethodID(CPP_lib, "recall", "(Ljava/lang/String;)Ljava/lang/String;");
	this->QueryML = env->GetStaticMethodID(CPP_lib, "queryML", "(J)Ljava/lang/String;");
	this->QueryN = env->GetStaticMethodID(CPP_lib, "queryNG", "(J)Ljava/lang/String;");
	this->uploadImgF= env->GetStaticMethodID(config->CPP_lib, "uploadImgF", "(JLjava/lang/String;)Ljava/lang/String;");
	this->uploadImgG = env->GetStaticMethodID(config->CPP_lib, "uploadImgG", "(JLjava/lang/String;)Ljava/lang/String;");
	this->uploadImgM = env->GetStaticMethodID(config->CPP_lib, "uploadImgM", "(JJLjava/lang/String;)Ljava/lang/String;");
	this->muteAll = env->GetStaticMethodID(config->CPP_lib, "muteGroup", "(JZ)Ljava/lang/String;");
	this->getowner = env->GetStaticMethodID(config->CPP_lib, "queryOwner", "(J)Ljava/lang/String;");
	this->buildforward = env->GetStaticMethodID(config->CPP_lib, "buildforward", "(Ljava/lang/String;)Ljava/lang/String;");
	this->NFR = env->GetStaticMethodID(config->CPP_lib, "nfroperation", "(Ljava/lang/String;Z)Ljava/lang/String;");
	this->GI = env->GetStaticMethodID(config->CPP_lib, "gioperation", "(Ljava/lang/String;Z)Ljava/lang/String;");
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
	else {
		logger->Error("zbc");
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
��־��ʵ��
throw: InitException ���Ҳ���ǩ��
*/
void Logger::init(JNIEnv* env){
	this->CPP_lib = (jclass)(env->NewGlobalRef(env->FindClass("tech/eritquearcus/miraicp/CPP_lib")));
	this->sinfo = env->GetStaticMethodID(this->CPP_lib, "SendLog", "(Ljava/lang/String;)V");
	this->swarning = env->GetStaticMethodID(this->CPP_lib, "SendW", "(Ljava/lang/String;)V");
	this->serror = env->GetStaticMethodID(this->CPP_lib, "SendE", "(Ljava/lang/String;)V");
	if (this->CPP_lib == NULL) {
		throw InitException("logger��ʼ������", 1);
	}
	if (this->sinfo == NULL) {
		throw InitException("logger��ʼ������", 2);
	}
	if (this->swarning == NULL) {
		throw InitException("logger��ʼ������", 3);
	}
	if (this->serror == NULL) {
		throw InitException("logger��ʼ������", 4);
	}
}
void Logger::Warning(std::string log, JNIEnv* env) {
	env->CallStaticVoidMethod(config->CPP_lib, this->swarning, tools.str2jstring(log.c_str()));
}
void Logger::Error(std::string log, JNIEnv* env) {
	env->CallStaticVoidMethod(config->CPP_lib, this->serror, tools.str2jstring(log.c_str()));
}
void Logger::Info(std::string log, JNIEnv* env) {
	env->CallStaticVoidMethod(config->CPP_lib, this->sinfo, tools.str2jstring(log.c_str()));
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

//��ϢԴ
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
		throw APIException("JSON reader error,λ��:C-MessageSource");

	}
	this->ids = root["ids"].toStyledString();
	this->ids = tools.replace(this->ids, "\n", "");
	this->ids = tools.replace(this->ids, " ", "");
	this->internalids = root["internalIds"].toStyledString();
	this->internalids = tools.replace(this->internalids, "\n", "");
	this->internalids = tools.replace(this->internalids, " ", "");
}
//������������¼
void ForwardMessage::sendTo(Contact* c, JNIEnv* env) {
	Json::Value temp;
	temp["id"] = c->id;
	temp["groupid"] = c->groupid;
	temp["type"] = c->type;
	temp["content"] = sendmsg;
	std::string re = tools.jstring2str((jstring)env->
		CallStaticObjectMethod(config->CPP_lib, config->buildforward,
			tools.str2jstring(tools.JsonToString(temp).c_str())));
	if (re == "Y") return;
	if (re == "E1") {
		switch (c->type) {
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

/*ͼƬ��ʵ��*/
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

/*������ʵ��*/
Friend::Friend(unsigned long long id, JNIEnv* env) {
	this->type = 1;
	this->id = id;
	std::string temp = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->NickorNameF, (jlong)id, (jlong)id));
	if (temp == "E1") {
		throw FriendException();
	}
	this->nickOrNameCard = temp;
}
Image Friend::uploadImg(std::string filename, JNIEnv* env) {
	std::ifstream fin(filename);
	if (!fin) {
		logger->Error("�ļ�������,λ��:C-Friend::uploadImg(),�ļ���:" + filename);
		fin.close();
		throw std::invalid_argument("NO_FILE_ERROR");
	}
	fin.close();
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->uploadImgF, (jlong)this->id, tools.str2jstring(filename.c_str())));
	return Image(re);
}
MessageSource Friend::SendMiraiCode(std::string msg, JNIEnv* env) {
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2F, tools.str2jstring(msg.c_str()), (jlong)this->id));
	if (re == "E1") {
		throw FriendException();
	}
	return MessageSource(re);
}
MessageSource Friend::SendMsg(std::string msg, JNIEnv* env){
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2FM, tools.str2jstring(msg.c_str()), (jlong)this->id));
	if (re == "E1") {
		throw FriendException();
	}
	logger->Info("C-Source:\""+re+"\"");
	return MessageSource(re);
}

/*��Ա��ʵ��*/
Member::Member(unsigned long long id, unsigned long long groupid, JNIEnv* env) {
	this->type = 3;
	this->id = id;
	this->Mute_id = config->Mute;
	this->groupid = groupid;
	this->Query_permission = config->QueryP;
	this->KickM = config->KickM;
	std::string temp = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->NickorNameM, (jlong)id, (jlong)groupid));
	if (temp == "E1") {
		throw MemberException(1);
	}
	if (temp == "E2") {
		throw MemberException(2);
	}
	this->nickOrNameCard = temp;
	this->permission = getPermission();
}
unsigned int Member::getPermission(JNIEnv* env){
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->QueryP, (jlong)id, (jlong)groupid));
	if (re == "E1") {
		throw MemberException(1);
	}
	if(re == "E2"){
		throw MemberException(2);
	}
	return stoi(re);
}
void Member::Mute(int time, JNIEnv* env) {
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, this->Mute_id, (jlong)this->id, (jlong)this->groupid, (jint)time));
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
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, this->KickM, (jlong)id, (jlong)groupid, tools.str2jstring(reason.c_str())));
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
		throw IOException("�ļ�������,λ��:C++���� uploadImg2Group(),�ļ���:" + filename);
	}
	fin.close();
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->uploadImgM, (jlong)groupid, (jlong)id, tools.str2jstring(filename.c_str())));
	return Image(re);
}
MessageSource Member::SendMiraiCode(std::string msg, JNIEnv* env) {
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2M, tools.str2jstring(msg.c_str()), (jlong)this->id, (jlong)this->groupid));
	if (re == "E1") {
		throw MemberException(1);
	}
	if (re == "E2") {
		throw MemberException(2);
	}
	return MessageSource(re);
}
MessageSource Member::SendMsg(std::string msg,JNIEnv* env) {
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2MM, tools.str2jstring(msg.c_str()), (jlong)this->id, (jlong)this->groupid));
	if (re == "E1") {
		throw MemberException(1);
	}
	if (re == "E2") {
		throw MemberException(2);
	}
	return MessageSource(re);
}

/*Ⱥ����ʵ��*/
Group::Group(unsigned long long id, JNIEnv* env) {
	this->type = 2;
	this->id = id;
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib,
		config->QueryN,
		(jlong)this->id));
	if (re == "E1") {
		throw GroupException();
	}
	this->nickOrNameCard = re;
}
std::vector<unsigned long long> Group::getMemberList() {
	std::string re = tools.jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib,
		config->QueryML,
		(jlong)this->id));
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
		logger->Error("�ļ�������,λ��:C++���� Group::uploadImg2(),�ļ���:" + filename);
		fin.close();
		//throw invalid_argument("NO_FILE_ERROR");
	}
	fin.close();
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->uploadImgG, (jlong)this->id, tools.str2jstring(filename.c_str())));
	return Image(re);
}
void Group::setMuteAll(bool sign, JNIEnv* env){
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->muteAll, (jlong)this->id, (jboolean)sign));
	if (re == "Y") return;
	if (re == "E1") throw GroupException();
	if (re == "E2") throw BotException(1);
}
Member Group::getOwner(JNIEnv* env) {
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->getowner, (jlong)this->id));
	if (re == "E1")throw GroupException();
	return Member(stoi(re), this->id);
}
MessageSource Group::SendMiraiCode(std::string msg, JNIEnv* env) {
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2G, tools.str2jstring(msg.c_str()), (jlong)this->id));
	if (re == "E1") {
		throw GroupException();
	}
	return MessageSource(re);
}
MessageSource Group::SendMsg(std::string msg, JNIEnv* env) {
	std::string re = tools.jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2GM, tools.str2jstring(msg.c_str()), (jlong)this->id));
	if (re == "E1") {
		throw GroupException();
	}
	return MessageSource(re);
}

/*������ʵ��*/
std::string Tools::jstring2str(jstring jstr, JNIEnv* env)
{	
	if (jstr == NULL) { 
		logger->Warning("�쳣:kotlin���ؿ��ַ���");
		return ""; 
	}
	jsize len = env->GetStringLength(jstr);
	const jchar* jcstr = env->GetStringChars(jstr, NULL);
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
	if (str == NULL || *str == '\0') { 
		logger->Warning("�쳣:������ַ�����kotlin");
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