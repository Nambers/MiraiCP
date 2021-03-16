#include "pch.h"

/*
配置类实现
throw: InitxException 即找不到对应签名
*/
void Config::Init() throw(InitException) {
	JNIEnv* env = genv;
	
	this->initexception = env->FindClass("java/lang/NoSuchMethodException");
	this->CPP_lib = (jclass)env->NewGlobalRef(env->FindClass("org/example/mirai/plugin/CPP_lib"));
	if (this->CPP_lib == NULL) {
		throw InitException("初始化错误", 1);
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
	this->uploadImgF= genv->GetStaticMethodID(config->CPP_lib, "uploadImgF", "(JLjava/lang/String;)Ljava/lang/String;");
	this->uploadImgG = genv->GetStaticMethodID(config->CPP_lib, "uploadImgG", "(JLjava/lang/String;)Ljava/lang/String;");
	this->uploadImgM = genv->GetStaticMethodID(config->CPP_lib, "uploadImgM", "(JJLjava/lang/String;)Ljava/lang/String;");
	this->muteAll = genv->GetStaticMethodID(config->CPP_lib, "muteGroup", "(JZ)Ljava/lang/String;");
	this->getowner = genv->GetStaticMethodID(config->CPP_lib, "queryOwner", "(J)Ljava/lang/String;");
	this->buildforward = genv->GetStaticMethodID(config->CPP_lib, "buildforward", "(Ljava/lang/String;)Ljava/lang/String;");
}
Config::~Config() {
	genv->DeleteGlobalRef(this->CPP_lib);
}

/*
日志类实现
throw: InitException 即找不到签名
*/
void Logger::init()throw(InitException) {
	JNIEnv* env = genv;
	this->CPP_lib = (jclass)(env->NewGlobalRef(env->FindClass("org/example/mirai/plugin/CPP_lib")));
	this->sinfo = env->GetStaticMethodID(this->CPP_lib, "SendLog", "(Ljava/lang/String;)V");
	this->swarning = env->GetStaticMethodID(this->CPP_lib, "SendW", "(Ljava/lang/String;)V");
	this->serror = env->GetStaticMethodID(this->CPP_lib, "SendE", "(Ljava/lang/String;)V");
	if (this->CPP_lib == NULL) {
		throw InitException("logger初始化错误", 1);
	}
	if (this->sinfo == NULL) {
		throw InitException("logger初始化错误", 2);
	}
	if (this->swarning == NULL) {
		throw InitException("logger初始化错误", 3);
	}
	if (this->serror == NULL) {
		throw InitException("logger初始化错误", 4);
	}
}
void Logger::Warning(string log) {
	genv->CallStaticVoidMethod(config->CPP_lib, this->swarning, tools.str2jstring(log.c_str()));
}
void Logger::Error(string log) {
	genv->CallStaticVoidMethod(config->CPP_lib, this->serror, tools.str2jstring(log.c_str()));
}
void Logger::Info(string log) {
	genv->CallStaticVoidMethod(config->CPP_lib, this->sinfo, tools.str2jstring(log.c_str()));
}
Logger::~Logger() {
	genv->DeleteGlobalRef(this->CPP_lib);
}

//消息源
MessageSource::MessageSource(string t) {
	this->source = t;
	const auto rawJsonLength = static_cast<int>(t.length());
	JSONCPP_STRING err;
	Json::Value root;
	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(t.c_str(), t.c_str() + rawJsonLength, &root,
		&err)) {
		//error
		logger->Error("JSON reader error");
		APIException("JSON reader error").raise();
	}
	this->ids = root["ids"].toStyledString();
	this->ids = tools.replace(this->ids, "\n", "");
	this->ids = tools.replace(this->ids, " ", "");
	this->internalids = root["internalIds"].toStyledString();
	this->internalids = tools.replace(this->internalids, "\n", "");
	this->internalids = tools.replace(this->internalids, " ", "");
}
//发送这个聊天记录
void ForwardMessage::sendTo(Contact* c) {
	Json::Value temp;
	temp["id"] = c->id;
	temp["groupid"] = c->groupid;
	temp["type"] = c->type;
	temp["content"] = sendmsg;
	string re = tools.jstring2str((jstring)genv->
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
	if (re == "E3") throw APIException("参数错误");
}

/*图片类实现*/
Image::Image(string imageId) {
	this->Query = config->Query;
	this->id = imageId;
}
string Image::queryURL() {
	return tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, this->Query, tools.str2jstring(this->id.c_str())));
}
vector<string> Image::GetImgIdsFromMiraiCode(string MiraiCode) {
	vector<string> result = vector<string>();
	string temp = MiraiCode;
	smatch m;
	regex re("\\[mirai:image:(.*?)\\]");
	while (std::regex_search(temp, m, re)) {
		result.push_back(m[1]);
		temp = m.suffix().str();
	}
	return result;
}
string Image::toMiraiCode() {
	return "[mirai:image:" + this->id + "]";
}

/*好友类实现*/
Friend::Friend(unsigned long long id) {
	this->type = 1;
	this->id = id;
	string temp = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->NickorNameF, (jlong)id, (jlong)id));
	if (temp == "E1") {
		throw FriendException();
	}
	this->nick = temp;
}
Image Friend::uploadImg(string filename) {
	ifstream fin(filename);
	if (!fin) {
		logger->Error("文件不存在,位置:C-Friend::uploadImg(),文件名:" + filename);
		fin.close();
		throw invalid_argument("NO_FILE_ERROR");
	}
	fin.close();
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->uploadImgF, (jlong)this->id, tools.str2jstring(filename.c_str())));
	return Image(re);
}
MessageSource Friend::SendMiraiCode(string msg)throw(FriendException) {
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2F, tools.str2jstring(msg.c_str()), (jlong)this->id));
	if (re == "E1") {
		throw FriendException();
	}
	return MessageSource(re);
}
MessageSource Friend::SendMsg(string msg)throw(FriendException) {
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2FM, tools.str2jstring(msg.c_str()), (jlong)this->id));
	if (re == "E1") {
		throw FriendException();
	}
	return MessageSource(re);
}

/*成员类实现*/
Member::Member(unsigned long long id, unsigned long long groupid) {
	this->type = 3;
	this->id = id;
	this->Mute_id = config->Mute;
	this->groupid = groupid;
	this->Query_permission = config->QueryP;
	this->KickM = config->KickM;
	string temp = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->NickorNameM, (jlong)id, (jlong)groupid));
	if (temp == "E1") {
		throw MemberException(1);
	}
	if (temp == "E2") {
		throw MemberException(2);
	}
	this->nameCard = temp;
	this->permission = getPermission();
}
int Member::getPermission() throw(MemberException) {
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->QueryP, (jlong)id, (jlong)groupid));
	if (re == "E1") {
		throw MemberException(1);
	}
	if(re == "E2"){
		throw MemberException(2);
	}
	return stoi(re);
}
void Member::Mute(int time)throw(MuteException, MemberException, BotException) {
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, this->Mute_id, (jlong)this->id, (jlong)this->groupid, (jint)time));
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
void Member::Kick(string reason) throw(BotException, MemberException) {
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, this->KickM, (jlong)id, (jlong)groupid, tools.str2jstring(reason.c_str())));
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
Image Member::uploadImg(string filename) {
	ifstream fin(filename);
	if (!fin) {
		fin.close();
		throw IOException("文件不存在,位置:C++部分 uploadImg2Group(),文件名:" + filename);
	}
	fin.close();
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->uploadImgM, (jlong)groupid, (jlong)id, tools.str2jstring(filename.c_str())));
	return Image(re);
}
MessageSource Member::SendMiraiCode(string msg)throw(MemberException) {
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2M, tools.str2jstring(msg.c_str()), (jlong)this->id, (jlong)this->groupid));
	if (re == "E1") {
		throw MemberException(1);
	}
	if (re == "E2") {
		throw MemberException(2);
	}
	return MessageSource(re);
}
MessageSource Member::SendMsg(string msg) throw(MemberException) {
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2MM, tools.str2jstring(msg.c_str()), (jlong)this->id, (jlong)this->groupid));
	if (re == "E1") {
		throw MemberException(1);
	}
	if (re == "E2") {
		throw MemberException(2);
	}
	return MessageSource(re);
}

/*群聊类实现*/
Group::Group(unsigned long long id) {
	this->type = 2;
	this->id = id;
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib,
		config->QueryN,
		(jlong)this->id));
	if (re == "E1") {
		throw GroupException();
	}
	this->name = re;
	re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib,
		config->QueryML,
		(jlong)this->id));
	if (re == "E1") {
		throw GroupException();
	}
	this->memberlist = re;
}
vector<unsigned long long> Group::getMemberList() {
	vector<unsigned long long> result;
	string temp = this->memberlist;
	temp.erase(temp.begin());
	temp.pop_back();
	std::regex ws_re("[,]+");
	std::vector<std::string> v(std::sregex_token_iterator(temp.begin(), temp.end(), ws_re, -1),
		std::sregex_token_iterator());
	for (auto&& s : v)
		result.push_back(atoi(s.c_str()));
	return result;
}
string Group::MemberListToString() {
	vector<unsigned long long> a = getMemberList();
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
Image Group::uploadImg(string filename) {
	ifstream fin(filename);
	if (!fin) {
		logger->Error("文件不存在,位置:C++部分 Group::uploadImg2(),文件名:" + filename);
		fin.close();
		//throw invalid_argument("NO_FILE_ERROR");
	}
	fin.close();
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->uploadImgG, (jlong)this->id, tools.str2jstring(filename.c_str())));
	return Image(re);
}
void Group::setMuteAll(bool sign)throw(GroupException, BotException) {
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->muteAll, (jlong)this->id, (jboolean)sign));
	if (re == "Y")return;
	if (re == "E1") throw GroupException();
	if (re == "E2") throw BotException(1);
}
Member Group::getOwner() {
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->getowner, (jlong)this->id));
	if (re == "E1")throw GroupException();
	return Member(stoi(re), this->id);
}
MessageSource Group::SendMiraiCode(string msg)throw(GroupException) {
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2G, tools.str2jstring(msg.c_str()), (jlong)this->id));
	if (re == "E1") {
		throw GroupException();
	}
	return MessageSource(re);
}
MessageSource Group::SendMsg(string msg) throw(GroupException) {
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->SendMsg2GM, tools.str2jstring(msg.c_str()), (jlong)this->id));
	if (re == "E1") {
		throw GroupException();
	}
	return MessageSource(re);
}

/*工具类实现*/
string Tools::jstring2str(jstring jstr)
{	
	if (jstr == NULL) { 
		logger->Warning("异常:kotlin返回空字符串");
		return ""; 
	}
	jsize len = genv->GetStringLength(jstr);
	const jchar* jcstr = genv->GetStringChars(jstr, NULL);
	int size = 0;
	char* str = (char*)malloc(static_cast<size_t>(len) * 2 + 1);
	if ((size = WideCharToMultiByte(CP_ACP, 0, LPCWSTR(jcstr), len, str, len * 2 + 1, NULL, NULL)) == 0)
	{
		logger->Warning("异常:kotlin返回空字符串");
		return "";
	}
	genv->ReleaseStringChars(jstr, jcstr);
	str[size] = 0;
	return str;
}
jstring Tools::str2jstring(const char* str)
{
	jstring rtn = 0;
	int slen = strlen(str);
	unsigned short* buffer = 0;
	if (slen == 0)
		rtn = (genv)->NewStringUTF(str);
	else
	{
		int length = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str, slen, NULL, 0);
		buffer = (unsigned short*)malloc(static_cast<size_t>(length) * 2 + 1);
		if (MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str, slen, (LPWSTR)buffer, length) > 0)
			rtn = (genv)->NewString((jchar*)buffer, length);
	}
	if (buffer)
		free(buffer);
	return rtn;
}
string Tools::JLongToString(jlong qqid) {
	auto id = [qqid]() -> string {
		stringstream stream;
		stream << qqid;
		string a;
		stream >> a;
		stream.clear();
		return a;
	};
	return id();
}
string Tools::JsonToString(const Json::Value& root)
{
	std::ostringstream stream;
	Json::StreamWriterBuilder stream_builder;
	stream_builder.settings_["emitUTF8"] = true;//Config emitUTF8
	std::unique_ptr<Json::StreamWriter> writer(stream_builder.newStreamWriter());
	writer->write(root, &stream);
	return stream.str();
}