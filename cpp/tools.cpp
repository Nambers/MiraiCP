#include "pch.h"

/*
������ʵ��
throw: InitxException ���Ҳ�����Ӧǩ��
*/
void Config::Init() throw(InitException) {
	JNIEnv* env = genv;
	this->initexception = env->FindClass("java/lang/NoSuchMethodException");
	this->CPP_lib = (jclass)env->NewGlobalRef(env->FindClass("org/example/mirai/plugin/CPP_lib"));
	if (this->CPP_lib == NULL) {
		throw InitException("��ʼ������", 1);
	}
	this->Query = env->GetStaticMethodID(CPP_lib, "QueryImgUrl", "(Ljava/lang/String;)Ljava/lang/String;");
	if (this->Query == NULL) {
		throw InitException("��ʼ������", 2);
	}
	this->SendMsg2F = env->GetStaticMethodID(CPP_lib, "SendPrivateMSG", "(Ljava/lang/String;J)Ljava/lang/String;");
	if (this->SendMsg2F == NULL) {
		throw InitException("��ʼ������", 3);
	}
	this->SendMsg2FM = env->GetStaticMethodID(CPP_lib, "SendPrivateMSGM", "(Ljava/lang/String;J)Ljava/lang/String;");
	if (this->SendMsg2FM == NULL) {
		throw InitException("��ʼ������", 3);
	}
	this->NickorNameF = env->GetStaticMethodID(CPP_lib, "GetNick", "(J)Ljava/lang/String;");
	if (this->NickorNameF == NULL) {
		throw InitException("��ʼ������", 4);
	}
	this->SendMsg2M = env->GetStaticMethodID(CPP_lib, "SendPrivateM2M", "(Ljava/lang/String;JJ)Ljava/lang/String;");
	if (this->SendMsg2M == NULL) {
		throw InitException("��ʼ������", 5);
	}
	this->SendMsg2MM = env->GetStaticMethodID(CPP_lib, "SendPrivateM2MM", "(Ljava/lang/String;JJ)Ljava/lang/String;");
	if (this->SendMsg2MM == NULL) {
		throw InitException("��ʼ������", 5);
	}
	this->NickorNameM = env->GetStaticMethodID(CPP_lib, "GetNameCard", "(JJ)Ljava/lang/String;");
	if (this->NickorNameM == NULL) {
		throw InitException("��ʼ������", 6);
	}
	this->SendMsg2G = env->GetStaticMethodID(CPP_lib, "SendGroup", "(Ljava/lang/String;J)Ljava/lang/String;");
	if (this->SendMsg2G == NULL) {
		throw InitException("��ʼ������", 7);
	}
	this->SendMsg2GM = env->GetStaticMethodID(CPP_lib, "SendGroupM", "(Ljava/lang/String;J)Ljava/lang/String;");
	if (this->SendMsg2GM == NULL) {
		throw InitException("��ʼ������", 7);
	}
	this->Schedule = env->GetStaticMethodID(CPP_lib, "schedule", "(JI)V");
	if (this->Schedule == NULL) {
		throw InitException("��ʼ������", 8);
	}
	this->Mute = env->GetStaticMethodID(CPP_lib, "muteM", "(JJI)Ljava/lang/String;");
	if (this->Mute == NULL) {
		throw InitException("��ʼ������", 9);
	}
	this->QueryP = env->GetStaticMethodID(CPP_lib, "queryM", "(JJ)Ljava/lang/String;");
	if (this->QueryP == NULL) {
		throw InitException("��ʼ������", 10);
	}
	this->KickM = env->GetStaticMethodID(CPP_lib, "kickM", "(JJLjava/lang/String;)Ljava/lang/String;");
	if (this->KickM == NULL) {
		throw InitException("��ʼ������", 11);
	}
}
Config::~Config() {
	genv->DeleteGlobalRef(this->CPP_lib);
}

/*
��־��ʵ��
throw: InitException ���Ҳ���ǩ��
*/
void Logger::init()throw(InitException) {
	JNIEnv* env = genv;
	this->CPP_lib = (jclass)(env->NewGlobalRef(env->FindClass("org/example/mirai/plugin/CPP_lib")));
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

//��ʱ����ʵ��
void SetScheduling(long time, int id) {
	genv->CallStaticVoidMethod(config->CPP_lib, config->Schedule, (jlong) time, (jint) id);
}

/*ͼƬ��ʵ��*/
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
Image Image::uploadImg2Friend(unsigned long id, string filename) {
	ifstream fin(filename);
	if (!fin) {
		logger->Error("�ļ�������,λ��:C-uploadImg2Friend(),�ļ���:" + filename);
		fin.close();
		throw invalid_argument("NO_FILE_ERROR");
	}
	fin.close();
	jmethodID m = genv->GetStaticMethodID(config->CPP_lib, "uploadImgF", "(JLjava/lang/String;)Ljava/lang/String;");
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, m, (jlong)id, tools.str2jstring(filename.c_str())));
	return Image(re);
}
Image Image::uploadImg2Group(unsigned long groupid, string filename) {
	ifstream fin(filename);
	if (!fin) {
		logger->Error("�ļ�������,λ��:C++���� uploadImg2Group(),�ļ���:" + filename);
		fin.close();
		throw invalid_argument("NO_FILE_ERROR");
	}
	fin.close();
	jmethodID m = genv->GetStaticMethodID(config->CPP_lib, "uploadImgG", "(JLjava/lang/String;)Ljava/lang/String;");
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, m, (jlong)groupid, tools.str2jstring(filename.c_str())));
	return Image(re);
}
Image Image::uploadImg2Member(unsigned long groupid, unsigned long qqid, string filename) {
	ifstream fin(filename);
	if (!fin) {
		fin.close();
		throw IOException("�ļ�������,λ��:C++���� uploadImg2Group(),�ļ���:" + filename);
	}
	fin.close();
	jmethodID m = genv->GetStaticMethodID(config->CPP_lib, "uploadImgM", "(JJLjava/lang/String;)Ljava/lang/String;");
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, m, (jlong)groupid, (jlong)qqid, tools.str2jstring(filename.c_str())));
	return Image(re);
}
string Image::toMiraiCode() {
	return "[mirai:image:" + this->id + "] ";
}

/*������ʵ��*/
Friend::Friend(unsigned long id) {
	this->id = id;
}
void Friend::init()throw(FriendException) {
	string temp = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, config->NickorNameF, (jlong)id, (jlong)id));
	if(temp == "E1"){
		throw FriendException();
	}
	this->nick = temp;
}

/*��Ա��ʵ��*/
Member::Member(unsigned long id, unsigned long groupid) {
	this->id = id;
	this->Mute_id = config->Mute;
	this->groupid = groupid;
	this->Query_permission = config->QueryP;
	this->KickM = config->KickM;
}
void Member::init() throw(MemberException){
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
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, this->Query_permission, (jlong)id, (jlong)groupid));
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

/*Ⱥ����ʵ��*/
Group::Group(unsigned long id) {
	this->id = id;
}

/*������ʵ��*/
string Tools::jstring2str(jstring jStr)
{
	if (!jStr)
		return "";

	const jclass stringClass = genv->GetObjectClass(jStr);
	const jmethodID getBytes = genv->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
	const jbyteArray stringJbytes = (jbyteArray)genv->CallObjectMethod(jStr, getBytes, genv->NewStringUTF("UTF-8"));

	size_t length = (size_t)genv->GetArrayLength(stringJbytes);
	jbyte* pBytes = genv->GetByteArrayElements(stringJbytes, NULL);

	std::string ret = std::string((char*)pBytes, length);
	genv->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

	genv->DeleteLocalRef(stringJbytes);
	genv->DeleteLocalRef(stringClass);
	return ret;
}
jstring Tools::str2jstring(const char* pat)
{
	//��ȡString��class
	jclass string_clz = genv->FindClass("java/lang/String");
	//��ȡ���췽��  public String(byte bytes[], String charsetName)
	jmethodID jmid = genv->GetMethodID(string_clz, "<init>", "([BLjava/lang/String;)V");
	//����byte���鲢��ֵ
	jsize size = (jsize)strlen(pat);
	jbyteArray bytes = genv->NewByteArray(size);
	genv->SetByteArrayRegion(bytes, 0, size, (jbyte*)pat);

	//charsetName
	jstring charsetName = genv->NewStringUTF("GB2312");

	jstring temp = (jstring)genv->NewObject(string_clz, jmid, bytes, charsetName);

	genv->DeleteLocalRef(bytes);
	genv->DeleteLocalRef(string_clz);
	return temp;
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