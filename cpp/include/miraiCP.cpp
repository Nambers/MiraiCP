// Copyright (C) 2020-2021 Eritque arcus and contributors.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or any later version(in your opinion).
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <miraiCP.hpp>

namespace MiraiCP {

// 开始MiraiCP实现代码

// 静态成员

    std::map<std::string, ThreadManager::ThreadInfo> ThreadManager::threads = std::map<std::string, ThreadInfo>();
    std::recursive_mutex ThreadManager::mtx = std::recursive_mutex();
    JavaVM *ThreadManager::gvm = nullptr;
    long ThreadManager::JNIVersion = 0;

    jclass Config::CPP_lib = nullptr;
    jmethodID Config::KOperation = nullptr;

    Logger Logger::logger = Logger();
    [[deprecated("Use Logger::logger instead")]]
    Logger *const logger = &Logger::logger;

    PluginLogger *CPPPlugin::pluginLogger = nullptr;
    CPPPlugin *CPPPlugin::plugin = nullptr;

    std::map<int, std::string> SingleMessage::messageType = {
            {-2, "QuoteReply"},
            {-1, "unSupportMessage"},
            {0,  "plainText"},
            {1,  "at"},
            {2,  "atAll"},
            {3,  "image"},
            {4,  "app"},
            {5,  "service"}
    };

    Event Event::processor = Event();
    [[deprecated("Use Event::processor instead")]]
    Event *const procession = &Event::processor;

// 结束静态成员

    void ThreadManager::setEnv(JNIEnv *e) {
        mtx.lock();
        if (!ThreadManager::included(ThreadManager::getThreadId())) {
            ThreadInfo tmp{
                    e,
                    false
            };
            ThreadManager::threads.insert(std::pair<std::string, ThreadInfo>(ThreadManager::getThreadId(), tmp));
        } else {
            ThreadManager::threads[ThreadManager::getThreadId()].e = e;
        }
        mtx.unlock();
    }

    void ThreadManager::newEnv(const char *threadName) {
        JNIEnv *env = nullptr;
        JavaVMAttachArgs args{
                JNIVersion,
                const_cast<char *>(threadName),
                nullptr
        };
        gvm->AttachCurrentThread((void **) &env, &args);
        ThreadInfo tmp{env, true};
        ThreadManager::threads.insert(std::pair<std::string, ThreadInfo>(ThreadManager::getThreadId(), tmp));
        Logger::logger.info("refresh env");
    };

    void ThreadManager::detach() {
        mtx.lock();
        if (ThreadManager::included(ThreadManager::getThreadId())) {
            bool att = ThreadManager::threads[ThreadManager::getThreadId()].attach;
            ThreadManager::threads.erase(ThreadManager::getThreadId());
            if (att)
                gvm->DetachCurrentThread();
        }
        mtx.unlock();
    }

    bool ThreadManager::included(const std::string &id) {
        if (ThreadManager::threads.empty() || ThreadManager::threads.count(id) == 0)
            return false;
        return true;
    }

    JNIEnv *ThreadManager::getEnv(const std::string &file, int loc, const std::string &func) {
        mtx.lock();
        if (!ThreadManager::included(getThreadId())) {
            ThreadManager::newEnv();
        }
        JNIEnv *tmp = ThreadManager::threads[ThreadManager::getThreadId()].e;
        ThreadManager::threads[ThreadManager::getThreadId()].stack.push(file, loc, func);
        mtx.unlock();
        return tmp;
    }

/*
日志类实现
throw: InitException 即找不到签名
*/

    void Logger_interface::init(JNIEnv *env) {
        this->log = env->GetStaticMethodID(Config::CPP_lib, "KSendLog", "(Ljava/lang/String;I)V");
    }

    void Logger_interface::registerHandle(Logger_interface::Action action) {
        this->loggerhandler.action = std::move(action);
    }

    void Logger_interface::setHandleState(bool state) {
        this->loggerhandler.enable = state;
    }

    void Logger::log0(const std::string &content, int level, JNIEnv *env) {
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        json j;
        j["id"] = -2;
        j["log"] = content;
        env->CallStaticVoidMethod(Config::CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }

    void IdLogger::log0(const std::string &content, int level, JNIEnv *env) {
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        json j;
        j["id"] = id;
        j["log"] = content;
        env->CallStaticVoidMethod(Config::CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }

    void PluginLogger::log0(const std::string &content, int level, JNIEnv *env) {
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        json j;
        j["id"] = -1;
        j["name"] = CPPPlugin::plugin->config.id;
        j["log"] = content;
        env->CallStaticVoidMethod(Config::CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }

/*
配置类实现
throw: InitxException 即找不到对应签名
*/

    void Config::construct(JNIEnv *env) {
        Config::CPP_lib = reinterpret_cast<jclass>(env->NewGlobalRef(
                env->FindClass("tech/eritquearcus/miraicp/shared/CPP_lib")));
        if (Config::CPP_lib == nullptr) {
            throw APIException("初始化错误，找不到CPP_lib类");
        }
        Config::KOperation = env->GetStaticMethodID(CPP_lib, "KOperation", "(Ljava/lang/String;)Ljava/lang/String;");
    }

    void Config::destruct() {
        ThreadManager::getEnv(__FILE__, __LINE__)->DeleteGlobalRef(Config::CPP_lib);
    }

    std::string
    Config::koperation(operation_set type, json &data, JNIEnv *env, bool catchErr, const std::string &errorInfo) {
        json j;
        j["type"] = type;
        j["data"] = data;
        std::string re = Tools::jstring2str((jstring) env->CallStaticObjectMethod(Config::CPP_lib, Config::KOperation,
                                                                                  Tools::str2jstring(j.dump().c_str(),
                                                                                                     env)), env);
        if (catchErr) ErrorHandle(re, errorInfo);
        return re;
    }

    Event::~Event() {
        Node0 *temp[] = {GMHead, PMHead, GHead, NFHead, MJHead, MLHead, RHead, BHead, TOHead};
        for (Node0 *ptr: temp) {
            Node0 *now = ptr;
            Node0 *t = nullptr;
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

    void MessageSource::recall(JNIEnv *env) const {
        json j;
        j["source"] = this->serializeToString();
        std::string re = Config::koperation(Config::Recall, j, env);
        if (re == "Y") return;
        if (re == "E1") throw BotException();
        if (re == "E2") throw RecallException();
    }

    MessageSource::MessageSource(std::string ids, std::string internalids, std::string source) : ids(std::move(
            ids)), internalids(std::move(internalids)), source(std::move(source)) {}

    std::string MessageSource::serializeToString() const {
        return source;
    }

    MessageSource MessageSource::deserializeFromString(const std::string &source) {
        json j = json::parse(source);
        try {
            return {j["ids"].dump(), j["internalIds"].dump(), source};
        }
        catch (json::type_error &e) {
            Logger::logger.error("消息源序列化出错，格式不符合(MessageSource::deserializeFromString)");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
            throw e;
        }
    }

//远程文件(群文件)
    RemoteFile RemoteFile::deserializeFromString(const std::string &source) {
        json j;
        try {
            j = json::parse(source);
        }
        catch (json::parse_error &e) {
            Logger::logger.error("格式化json失败，RemoteFile::deserializeFromString");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
            throw e;
        }
        try {
            struct Dinfo d{
                    j["dinfo"]["url"],
                    j["dinfo"]["md5"],
                    j["dinfo"]["sha1"]
            };
            struct Finfo f{
                    j["finfo"]["size"],
                    j["finfo"]["uploaderid"],
                    j["finfo"]["downloadtime"],
                    j["finfo"]["uploadtime"],
                    j["finfo"]["lastmodifytime"]
            };
            return RemoteFile(j["id"], j["internalid"], j["name"], j["finfo"]["size"], j["path"], d, f);
        }
        catch (json::type_error &e) {
            Logger::logger.error("json格式化失败，位置:RemoteFile");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
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
    MessageSource ForwardMessage::sendTo(Contact *c, JNIEnv *env) {
        json temp;
        json text;
        text["id"] = c->id();
        text["groupid"] = c->groupid();
        text["type"] = c->type();
        text["content"] = sendmsg;
        temp["text"] = text.dump();
        temp["botid"] = c->botid();
        std::string re = Config::koperation(Config::Buildforward, temp, env);
        //TODO:https://github.com/mamoe/mirai/issues/1371
        return MessageSource::deserializeFromString(re);
    }

    ForwardMessage::ForwardMessage(Contact *c, std::initializer_list<ForwardNode> nodes) {
        json root;
        json value;
        root["type"] = c->type();
        root["id"] = c->id();
        root["id2"] = c->groupid();
        for (const ForwardNode &node: nodes) {
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

    std::string Image::queryURL(JNIEnv *env) {
        json j;
        j["id"] = this->id;
        std::string re = Config::koperation(Config::QueryImgUrl, j, env);
        if (re == "E1")
            throw RemoteAssetException("图片id格式错误");
        return re;
    }

    std::string Image::toMiraiCode() const {
        return "[mirai:image:" + Tools::escapeToMiraiCode(this->id) + "]";
    }

    MessageSource MessageSource::quoteAndSendMiraiCode(const std::string &content, QQID groupid, JNIEnv *env) const {
        json obj;
        json sign;
        obj["messageSource"] = this->serializeToString();
        obj["msg"] = content;
        sign["MiraiCode"] = true;
        sign["groupid"] = groupid;
        obj["sign"] = sign.dump();
        std::string re = Config::koperation(Config::SendWithQuote, obj, env);
        return MessageSource::deserializeFromString(re);
    }

    MessageSource MessageSource::quoteAndSendMsg(const std::string &content, QQID groupid, JNIEnv *env) const {
        json obj;
        json sign;
        obj["messageSource"] = this->serializeToString();
        obj["msg"] = content;
        sign["MiraiCode"] = false;
        sign["groupid"] = groupid;
        obj["sign"] = sign.dump();
        std::string re = Config::koperation(Config::SendWithQuote, obj, env);
        return MessageSource::deserializeFromString(re);
    }

    MessageSource Contact::sendMsg0(const std::string &msg, int retryTime, bool miraicode, JNIEnv *env) {
        if (msg.empty()) {
            Logger::logger.warning("警告:发送空信息, 位置: Contact::SendMsg");
            throw IllegalArgumentException("参数不能为空, 位置: Contact::SendMsg");
        }
        std::string re = LowLevelAPI::send0(msg, this, retryTime, miraicode, env,
                                            "reach a error area, Contact::SendMiraiCode");
        if (re == "ET")
            throw TimeOutException("发送消息过于频繁导致的tx服务器未能即使响应, 位置: Contact::SendMsg");
        if (Tools::starts_with(re, "EBM"))
            throw BotIsBeingMutedException(std::stoi(re.substr(3)));
        return MessageSource::deserializeFromString(re);
    }

    Image Contact::uploadImg(const std::string &path, JNIEnv *env) {
        std::string re = LowLevelAPI::uploadImg0(path, this, env);
        if (re == "E2")
            throw UploadException("上传图片大小超过30MB,路径:" + path);
        return Image(re);
    }

/*好友类实现*/
    Friend::Friend(QQID id, QQID botid, JNIEnv *env) : Contact() {
        this->_type = 1;
        this->_id = id;
        this->_botid = botid;
        refreshInfo(env);
    }

/*成员类实现*/
    Member::Member(QQID id, QQID groupid, QQID botid, JNIEnv *env)
            : Contact() {
        this->_type = 3;
        this->_id = id;
        this->_groupid = groupid;
        this->_botid = botid;
        refreshInfo(env);
    }

    unsigned int Member::getPermission(JNIEnv *env) {
        if (isAnonymous) return 0;
        json j;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::QueryM, j, env);
        return stoi(re);
    }

    void Member::mute(int time, JNIEnv *env) {
        json j;
        j["time"] = time;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::MuteM, j, env);
        if (re == "E3") {
            throw BotException();
        }
        if (re == "E4") {
            throw MuteException();
        }
    }

    void Member::kick(const std::string &reason, JNIEnv *env) {
        json j;
        j["message"] = reason;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::KickM, j, env);
        if (re == "E3") {
            throw BotException();
        }
    }

    void Member::modifyAdmin(bool admin, JNIEnv *env) {
        if (isAnonymous)return;
        json j;
        j["admin"] = admin;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::ModifyAdmin, j, env);
        if (re == "E1") {
            throw BotException();
        }
    }

/*群聊类实现*/
    Group::Group(QQID groupid, QQID botid, JNIEnv *env) : Contact() {
        this->_type = 2;
        this->_id = groupid;
        this->_botid = botid;
        refreshInfo(env);
    }

    void Group::OnlineAnnouncement::deleteThis() {
        json j, i;
        i["botid"] = this->botid;
        i["groupid"] = this->groupid;
        i["fid"] = this->fid;
        i["type"] = 1;
        j["identify"] = i.dump();
        std::string re = Config::koperation(Config::Announcement, j);
        if (re == "E1")
            throw IllegalArgumentException("无法根据fid找到群公告(群公告不存在)");
        if (re == "E2")
            throw BotException();
        if (re == "E3")
            throw IllegalStateException("群公告状态异常");
    }

    json Group::AnnouncementParams::serializeToJson() {
        json j;
        j["sendToNewMember"] = this->send2new;
        j["isPinned"] = this->pinned;
        j["showEditCard"] = this->showEditCard;
        j["showPopup"] = this->showPopup;
        j["requireConfirmation"] = this->requireConfirm;
        return j;
    }

    Group::OnlineAnnouncement Group::OfflineAnnouncement::publishTo(const Group &g) {
        json j, i, s;
        i["botid"] = g.botid();
        i["groupid"] = g.id();
        i["type"] = 2;
        j["identify"] = i.dump();
        s["content"] = this->content;
        s["params"] = this->params.serializeToJson();
        j["source"] = s.dump();
        std::string re = Config::koperation(Config::Announcement, j);
        if (re == "E1")
            throw BotException();
        return Group::OnlineAnnouncement::deserializeFromJson(json::parse(re));
    }

    Group::OnlineAnnouncement Group::OnlineAnnouncement::deserializeFromJson(json j) {
        Group::AnnouncementParams ap(
                j["params"]["sendToNewMember"],
                j["params"]["requireConfirmation"],
                j["params"]["isPinned"],
                j["params"]["showEditCard"],
                j["params"]["showPopup"]
        );
        return Group::OnlineAnnouncement(
                j["content"],
                ap,
                j["groupid"],
                j["senderid"],
                j["botid"],
                j["time"],
                j["fid"],
                j["confirmationNum"],
                j["imageid"]
        );
    }

    void Group::updateSetting(JNIEnv *env) {
        json j;
        json tmp;
        j["name"] = this->setting.name;
        j["isMuteAll"] = this->setting.isMuteAll;
        j["isAllowMemberInvite"] = this->setting.isAllowMemberInvite;
        j["isAutoApproveEnabled"] = this->setting.isAutoApproveEnabled;
        j["isAnonymousChatEnabled"] = this->setting.isAnonymousChatEnabled;
        tmp["source"] = j.dump();
        tmp["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::GroupSetting, tmp, env);
        if (re == "E1")
            throw BotException();
        refreshInfo(env);
    }

    RemoteFile Group::sendFile(const std::string &path, const std::string &filepath, JNIEnv *env) {
        json tmp;
        json source;
        source["path"] = path;
        source["filepath"] = filepath;
        tmp["source"] = source.dump();
        tmp["contactSource"] = this->serializationToString();
        std::string callback = Config::koperation(Config::SendFile, tmp, env);
        if (callback == "E2") throw UploadException("找不到" + filepath + "位置:C-uploadfile");
        if (callback == "E3")
            throw UploadException("Upload error:路径格式异常，应为'/xxx.xxx'或'/xx/xxx.xxx'目前只支持群文件和单层路径, path:" + path);
        return RemoteFile::deserializeFromString(callback);
    }

    RemoteFile Group::getFile(const std::string &path, const std::string &id, JNIEnv *env) {
        // source 参数
        json tmp;
        json j;
        tmp["id"] = id;
        tmp["path"] = path;
        j["source"] = tmp.dump();
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::RemoteFileInfo, j, env);
        if (re == "E2") throw RemoteAssetException("Get error: 文件路径不存在, path:" + path);
        return RemoteFile::deserializeFromString(re);
    }

    Member Group::getOwner(JNIEnv *env) {
        json j;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::QueryOwner, j, env);
        return Member(stoi(re), this->id(), this->botid());
    }

    std::string Group::getFileListString(const std::string &path, JNIEnv *env) {
        json temp;
        json j;
        temp["id"] = "-1";
        temp["path"] = path;
        j["source"] = temp.dump();
        j["contactSource"] = this->serializationToString();
        return Config::koperation(Config::RemoteFileInfo, j, env);
    }

    std::vector<Group::file_short_info> Group::getFileList(const std::string &path, JNIEnv *env) {
        std::vector<file_short_info> re = std::vector<file_short_info>();
        std::string tmp = getFileListString(path, env);
        json root = json::parse(tmp);
        for (auto &i: root) {
            file_short_info t;
            t.path = i[0];
            t.id = i[1];
            re.push_back(t);
        }
        return re;
    }

    MessageChain PrivateMessageEvent::nextMessage(long time, bool halt, JNIEnv *env) {
        json j;
        j["contactSource"] = this->sender.serializationToString();
        j["time"] = time;
        j["halt"] = halt;
        std::string r = Config::koperation(Config::NextMsg, j, env);
        if (r == "-1")
            throw TimeOutException("取下一条信息超时");
        json re = json::parse(r);
        return MessageChain::deserializationFromMiraiCode(re["message"]).plus(
                MessageSource::deserializeFromString(re["messageSource"]));
    }

    MessageChain GroupMessageEvent::nextMessage(long time, bool halt, JNIEnv *env) {
        json j;
        j["contactSource"] = this->group.serializationToString();
        j["time"] = time;
        j["halt"] = halt;
        std::string r = Config::koperation(Config::NextMsg, j, env);
        if (r == "-1")
            throw TimeOutException("取下一条信息超时");
        json re = json::parse(r);
        return MessageChain::deserializationFromMiraiCode(re["message"]).plus(
                MessageSource::deserializeFromString(re["messageSource"]));
    }

    MessageChain GroupMessageEvent::senderNextMessage(long time, bool halt, JNIEnv *env) {
        json j;
        j["contactSource"] = this->sender.serializationToString();
        j["time"] = time;
        j["halt"] = halt;
        std::string r = Config::koperation(Config::NextMsg, j, env);
        if (r == "-1")
            throw TimeOutException("取下一条信息超时");
        json re = json::parse(r);
        return MessageChain::deserializationFromMiraiCode(re["message"]).plus(
                MessageSource::deserializeFromString(re["messageSource"]));
    }

/*工具类实现*/
    std::string Tools::jstring2str(jstring jStr, JNIEnv *env) {
        if (!jStr) {
            Logger::logger.error("警告:kotlin部分返回空字符串, 位置:Tools::jstring2str");
            return "";
        }
        std::u16string s = reinterpret_cast<const char16_t *>(env->GetStringChars(jStr, nullptr));
        if (s.length() == 0) {
            Logger::logger.error("警告:kotlin部分返回空字符串, 位置:Tools::jstring2str");
            return "";
        }
        std::string x;
        utf8::utf16to8(s.begin(), s.end(), std::back_inserter(x));
        return x;
    }

    jstring Tools::str2jstring(const char *stra, JNIEnv *env) {
        if (!stra) {
            Logger::logger.error("警告:C++部分传入空字符串，位置:Tools::str2jstring");
        }
        std::string str(stra);
        std::vector<unsigned short> utf16line;
        utf8::utf8to16(str.begin(), str.end(), std::back_inserter(utf16line));
        auto *c = new jchar[utf16line.size()];
        for (int i = 0; i < utf16line.size(); i++) {
            c[i] = utf16line[i];
        }
        return env->NewString((jchar *) c, (jsize) utf16line.size());
    }

    template<typename T>
    std::string Tools::VectorToString(std::vector<T> a, const std::string &separator) {
        std::stringstream ss;
        for (size_t i = 0; i < a.size(); ++i) {
            if (i != 0)
                ss << separator;
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
        result.reserve(v.size());
        for (auto &&s: v)
            result.push_back(std::stoull(s));
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
    }

    Contact Contact::deserializationFromString(const std::string &source) {
        json j;
        try {
            j = json::parse(source);
        }
        catch (json::parse_error &e) {
            Logger::logger.error("json序列化错误 Contact::deserializationFromString");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
        }
        return Contact::deserializationFromJson(j);
    }

    Contact Contact::deserializationFromJson(nlohmann::json j) {
        return Contact(j["type"],
                       j["id"],
                       j["groupid"],
                       j["nickornamecard"],
                       j["botid"],
                       j["anonymous"]);
    }

    MessageSource Contact::sendVoice0(const std::string &path, JNIEnv *env) {
        json j;
        json source;
        source["path"] = path;
        j["source"] = source.dump();
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::Voice, j, env);
        if (re == "E1")
            throw UploadException("上传语音文件格式不对(必须为.amr/.silk)或文件不存在");
        else if (re == "E2")
            throw UploadException("上传语音文件大小超过服务器限制，一般限制在1MB上下");
        return MessageSource::deserializeFromString(re);
    }

    json PluginConfig::serialize() {
        json j;
        j["name"] = name;
        j["version"] = version;
        j["author"] = author;
        j["description"] = description;
        j["time"] = time;
        j["id"] = id;
        return j;
    }
// 结束MiraiCP实现代码
}
//开始对接JNI接口代码

/*
* 名称:Java_com_example_plugin_CPP_1lib_Verify
* 作用:判断是否连接上本插件，勿改
* 参数:env 必备，job 必备
* 返回值:jstring (用str2jstring把string类型转成jsrting) 发送返回的字符串
*/
JNIEXPORT jstring Verify(JNIEnv *env, jobject) {
    using namespace MiraiCP;
    ThreadManager::setEnv(env);
    MiraiCP::ThreadManager::JNIVersion = env->GetVersion();
    try {
        //初始化日志模块
        Config::construct();
        Logger::logger.init();
        enrollPlugin();
        if (CPPPlugin::plugin == nullptr) {
            Logger::logger.error("无插件实例加载");
        } else {
            CPPPlugin::pluginLogger = new PluginLogger(&Logger::logger);
            CPPPlugin::plugin->onEnable();
        }
    }
    catch (MiraiCPException &e) {
        e.raise();
    }
    json j = CPPPlugin::plugin->config.serialize();
    j["MiraiCPversion"] = MiraiCPVersion;
    return Tools::str2jstring(j.dump().c_str());//验证机制，返回当前SDK版本
}
/* 插件结束事件*/
JNIEXPORT jobject PluginDisable(JNIEnv *env, jobject job) {
    using namespace MiraiCP;
    ThreadManager::setEnv(env);
    CPPPlugin::plugin->onDisable();
    CPPPlugin::plugin = nullptr;
    return job;
}
/*返回空值*/
JNIEXPORT jstring returnNull() {
    return MiraiCP::Tools::str2jstring("MIRAICP_NULL");
}
/*
* 消息解析分流
*/
JNIEXPORT jstring Event(JNIEnv *env, jobject, jstring content) {
    using namespace MiraiCP;
    ThreadManager::setEnv(env);
    std::string tmp = Tools::jstring2str(content, env);
    json j;
    try {
        j = json::parse(tmp);
    }
    catch (json::parse_error &e) {
        APIException("格式化json错误").raise();
        Logger::logger.error("For debug:" + j.dump());
        Logger::logger.error(e.what(), false);
        return returnNull();
    }
    ThreadManager::getThread()->stack.push(__FILE__, __LINE__, "source: " + tmp);
    try {
        switch ((int) j["type"]) {
            case 1: {
                //GroupMessage
                Event::processor.broadcast<GroupMessageEvent>(
                        GroupMessageEvent(j["group"]["botid"],
                                          Group(Group::deserializationFromJson(j["group"])),
                                          Member(Member::deserializationFromJson(j["member"])),
                                          MessageChain::deserializationFromMiraiCode(j["message"].get<std::string>())
                                                  .plus(MessageSource::deserializeFromString(j["source"]))
                        )
                );
                break;
            }
            case 2: {
                //私聊消息
                Event::processor.broadcast<PrivateMessageEvent>(
                        PrivateMessageEvent(j["friend"]["botid"],
                                            Friend(Friend::deserializationFromJson(j["friend"])),
                                            MessageChain::deserializationFromMiraiCode(j["message"])
                                                    .plus(MessageSource::deserializeFromString(j["source"]))
                        ));
                break;
            }
            case 3:
                //群聊邀请
                Event::processor.broadcast<GroupInviteEvent>(
                        GroupInviteEvent(
                                j["source"]["botid"],
                                j["request"],
                                j["source"]["inviternick"],
                                j["source"]["inviterid"],
                                j["source"]["groupname"],
                                j["source"]["groupid"]
                        ));
                break;
            case 4:
                //好友
                Event::processor.broadcast<NewFriendRequestEvent>(
                        NewFriendRequestEvent(
                                j["source"]["botid"],
                                j["request"],
                                j["source"]["fromid"],
                                j["source"]["fromgroupid"],
                                j["source"]["fromnick"],
                                j["source"]["message"]
                        ));
                break;
            case 5:
                //新成员加入
                Event::processor.broadcast<MemberJoinEvent>(
                        MemberJoinEvent(
                                j["group"]["botid"],
                                j["jointype"],
                                Member(Member::deserializationFromJson(j["member"])),
                                Group(Group::deserializationFromJson(j["group"])),
                                j["inviterid"]
                        ));
                break;
            case 6:
                //群成员退出
                Event::processor.broadcast<MemberLeaveEvent>(MemberLeaveEvent(
                        j["group"]["botid"],
                        j["leavetype"],
                        j["memberid"],
                        Group(Group::deserializationFromJson(j["group"])),
                        j["operatorid"]
                ));
                break;
            case 7:
                Event::processor.broadcast<RecallEvent>(RecallEvent(
                        j["botid"],
                        j["etype"],
                        j["time"],
                        j["authorid"],
                        j["operatorid"],
                        j["ids"],
                        j["internalids"],
                        j["groupid"]
                ));
                break;
            case 9:
                Event::processor.broadcast<BotJoinGroupEvent>(BotJoinGroupEvent(
                        j["group"]["botid"],
                        j["etype"],
                        Group(Group::deserializationFromJson(j["group"])),
                        j["inviterid"]
                ));
                break;
            case 10:
                Event::processor.broadcast<GroupTempMessageEvent>(GroupTempMessageEvent(
                        j["group"]["botid"],
                        Group(Group::deserializationFromJson(j["group"])),
                        Member(Member::deserializationFromJson(j["member"])),
                        MessageChain::deserializationFromMiraiCode(j["message"])
                                .plus(MessageSource::deserializeFromString(j["source"]))
                ));
                break;
            case 11:
                Event::processor.broadcast<BotOnlineEvent>(BotOnlineEvent(j["botid"]));
                break;
            case 12:
                Event::processor.broadcast<TimeOutEvent>(TimeOutEvent(j["msg"]));
                break;
            case 13:
                Event::processor.broadcast<NudgeEvent>(NudgeEvent(Contact::deserializationFromJson(j["from"]),
                                                                  Contact::deserializationFromJson(j["target"]),
                                                                  j["botid"]));
                break;
            case 14:
                Event::processor.broadcast(BotLeaveEvent(j["groupid"], j["botid"]));
                break;
            case 15: {
                std::optional<Group> a;
                std::optional<Member> b;
                Contact temp = Contact::deserializationFromJson(j["group"]);
                if (temp.id() == 0)
                    a = std::nullopt;
                else
                    a = Group(temp);
                temp = Contact::deserializationFromJson(j["inviter"]);
                if (temp.id() == 0)
                    b = std::nullopt;
                else
                    b = Member(temp);
                Event::processor.broadcast(MemberJoinRequestEvent(a, b, temp.botid(), j["requestData"]));
                break;
            }
            default:
                throw APIException("Unreachable code");
        }
    }
    catch (json::type_error &e) {
        Logger::logger.error("json格式化异常,位置C-Handle");
        Logger::logger.error(e.what(), false);
        return Tools::str2jstring("ERROR");
    } catch (MiraiCPException &e) {
        Logger::logger.error("MiraiCP error:" + e.what());
        return Tools::str2jstring("ERROR");
    }
    return returnNull();
}

int registerMethods(JNIEnv *env, const char *className,
                    JNINativeMethod *gMethods, int numMethods) {
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        return JNI_FALSE;
    }
    //注册native方法
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

JNINativeMethod method_table[] = {
        {(char *) "Verify",        (char *) "()Ljava/lang/String;",                   (jstring *) Verify},
        {(char *) "Event",         (char *) "(Ljava/lang/String;)Ljava/lang/String;", (jstring *) Event},
        {(char *) "PluginDisable", (char *) "()Ljava/lang/Void;",                     (jobject *) PluginDisable}
};

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    assert(env != nullptr);
    MiraiCP::ThreadManager::gvm = vm;
    // 注册native方法
    if (!registerMethods(env, "tech/eritquearcus/miraicp/shared/CPP_lib", method_table, 3)) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

//结束对接JNI接口代码