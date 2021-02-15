#include "pch.h"

/*配置类实现*/
void Config::Init() {
	JNIEnv* env = genv;
	this->CPP_lib = (jclass)env->NewGlobalRef(env->FindClass("org/example/mirai/plugin/CPP_lib"));
	if (this->CPP_lib == NULL) {
		logger->Error("1");
		throw exception("初始化错误");
	}
	this->Query = env->GetStaticMethodID(CPP_lib, "QueryImgUrl", "(Ljava/lang/String;)Ljava/lang/String;");
	if (this->Query == NULL) {
		logger->Error("2");
		throw exception("初始化错误");
	}
	this->SendMsg2F = env->GetStaticMethodID(CPP_lib, "SendPrivateMSG", "(Ljava/lang/String;J)V");
	if (this->SendMsg2F == NULL) {
		logger->Error("3");
		throw exception("初始化错误");
	}
	this->NickorNameF = env->GetStaticMethodID(CPP_lib, "GetNick", "(J)Ljava/lang/String;");
	if (this->NickorNameF == NULL) {
		logger->Error("4");
		throw exception("初始化错误");
	}
	this->SendMsg2M = env->GetStaticMethodID(CPP_lib, "SendPrivateM2M", "(Ljava/lang/String;JJ)V");
	if (this->SendMsg2M == NULL) {
		logger->Error("5");
		throw exception("初始化错误");
	}
	this->NickorNameM = env->GetStaticMethodID(CPP_lib, "GetNameCard", "(JJ)Ljava/lang/String;");
	if (this->NickorNameM == NULL) {
		logger->Error("6");
		throw exception("初始化错误");
	}
	this->SendMsg2G = env->GetStaticMethodID(CPP_lib, "SendGroup", "(Ljava/lang/String;J)V");
	if (this->SendMsg2G == NULL) {
		logger->Error("7");
		throw exception("初始化错误");
	}
	this->Schedule = env->GetStaticMethodID(CPP_lib, "schedule", "(JI)V");
	if (this->Schedule == NULL) {
		logger->Error("8");
		throw exception("初始化错误");
	}
	this->Mute = env->GetStaticMethodID(CPP_lib, "muteM", "(JJI)Ljava/lang/String;");
	if (this->Mute == NULL) {
		logger->Error("9");
		throw exception("初始化错误");
	}
}
Config::~Config() {
	genv->DeleteGlobalRef(this->CPP_lib);
}

/*日志类实现*/
void Logger::init() {
	JNIEnv* env = genv;
	this->CPP_lib = (jclass)(env->NewGlobalRef(env->FindClass("org/example/mirai/plugin/CPP_lib")));
	this->sinfo = env->GetStaticMethodID(this->CPP_lib, "SendLog", "(Ljava/lang/String;)V");
	this->swarning = env->GetStaticMethodID(this->CPP_lib, "SendW", "(Ljava/lang/String;)V");
	this->serror = env->GetStaticMethodID(this->CPP_lib, "SendE", "(Ljava/lang/String;)V");
	if (this->CPP_lib == NULL) {
		logger->Error("1");
		throw exception("初始化错误");
	}
	if (this->sinfo == NULL) {
		logger->Error("2");
		throw exception("初始化错误");
	}
	if (this->swarning == NULL) {
		logger->Error("3");
		throw exception("初始化错误");
	}
	if (this->serror == NULL) {
		logger->Error("4");
		throw exception("初始化错误");
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

//定时任务实现
void SetScheduling(long time, int id) {
	genv->CallStaticVoidMethod(config->CPP_lib, config->Schedule, (jlong) time, (jint) id);
}

/*图片类实现*/
Image::Image(string imageId) {
	this->Query = config->Query;
	this->id = imageId;
}
string Image::queryURL() {
	return tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, this->Query, tools.str2jstring(this->id.c_str())));
}
vector<string> Image::GetImgIdFromMiraiCode(string MiraiCode) {
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
		logger->Error("文件不存在,位置:C++部分 uploadImg2Friend(),文件名:" + filename);
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
		logger->Error("文件不存在,位置:C++部分 uploadImg2Group(),文件名:" + filename);
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
		logger->Error("文件不存在,位置:C++部分 uploadImg2Group(),文件名:" + filename);
		fin.close();
		throw invalid_argument("NO_FILE_ERROR");
	}
	fin.close();
	jmethodID m = genv->GetStaticMethodID(config->CPP_lib, "uploadImgM", "(JJLjava/lang/String;)Ljava/lang/String;");
	string re = tools.jstring2str((jstring)genv->CallStaticObjectMethod(config->CPP_lib, m, (jlong)groupid, (jlong)qqid, tools.str2jstring(filename.c_str())));
	return Image(re);
}
string Image::toMiraiCode() {
	return "[mirai:image:" + this->id + "] ";
}

/*好友类实现*/
Friend::Friend(unsigned long id) {
	this->Send_Msg_id = config->SendMsg2F;
	this->NickorName_id = config->NickorNameF;
	this->id = id;
	jstring temp = (jstring)genv->CallStaticObjectMethod(config->CPP_lib, this->NickorName_id, (jlong)id, (jlong)id);
	this->nick = tools.jstring2str(temp);
}

/*成员类实现*/
Member::Member(unsigned long id, unsigned long groupid) {
	this->id = id;
	this->Mute_id = config->Mute;
	this->groupid = groupid;
	this->Send_Msg_id = config->SendMsg2M;
	this->NickorName_id = config->NickorNameM;
	jstring temp = (jstring)genv->CallStaticObjectMethod(config->CPP_lib, this->NickorName_id, (jlong)id, (jlong)groupid);
	this->nameCard = tools.jstring2str(temp);
}

/*群聊类实现*/
Group::Group(unsigned long id) {
	this->Send_Msg_id = config->SendMsg2G;
	this->id = id;
}

/*工具类实现*/
string Tools::jstring2str(jstring jStr)
{
	/*char* rtn = NULL;
	jclass   clsstring = config->env->FindClass("java/lang/String");
	jstring   strencode = config->env->NewStringUTF("GB2312");
	jmethodID   mid = config->env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
	jbyteArray   barr = (jbyteArray)config->env->CallObjectMethod(jstr, mid, strencode);
	jsize   alen = config->env->GetArrayLength(barr);
	jbyte* ba = config->env->GetByteArrayElements(barr, JNI_FALSE);
	if (alen > 0)
	{
		rtn = (char*)malloc(alen + 1);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	config->env->ReleaseByteArrayElements(barr, ba, 0);
	string stemp(rtn);
	free(rtn);
	return stemp;*/
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
	//获取String的class
	jclass string_clz = genv->FindClass("java/lang/String");
	//获取构造方法  public String(byte bytes[], String charsetName)
	jmethodID jmid = genv->GetMethodID(string_clz, "<init>", "([BLjava/lang/String;)V");
	//创建byte数组并赋值
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