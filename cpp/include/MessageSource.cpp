#include "Config.h"
#include "LowLevelAPI.h"

namespace MiraiCP {
    void MessageSource::recall(JNIEnv *env) const {
        json j;
        j["source"] = this->serializeToString();
        std::string re = Config::koperation(Config::Recall, j, env);
        if (re == "Y") return;
        if (re == "E1") MiraiCPThrow(BotException());
        if (re == "E2") MiraiCPThrow(RecallException());
    }

    MessageSource::MessageSource(std::string ids,
                                 std::string internalids,
                                 std::string source)
        : ids(std::move(ids)),
          internalids(std::move(internalids)),
          source(std::move(source)) {}

    std::string MessageSource::serializeToString() const {
        return source;
    }

    MessageSource MessageSource::deserializeFromString(const std::string &source) {
        json j = json::parse(source);
        try {
            return {j["ids"].dump(), j["internalIds"].dump(), source};
        } catch (json::type_error &e) {
            Logger::logger.error("消息源序列化出错，格式不符合(MessageSource::deserializeFromString)");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
            throw e;
        }
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
            MiraiCPThrow(IllegalArgumentException("参数不能为空, 位置: Contact::SendMsg"));
        }
        std::string re = LowLevelAPI::send0(msg, this, retryTime, miraicode, env,
                                            "reach a error area, Contact::SendMiraiCode");
        if (re == "ET")
            MiraiCPThrow(TimeOutException("发送消息过于频繁导致的tx服务器未能即使响应, 位置: Contact::SendMsg"));
        if (Tools::starts_with(re, "EBM"))
            MiraiCPThrow(BotIsBeingMutedException(std::stoi(re.substr(3))));
        return MessageSource::deserializeFromString(re);
    }
} // namespace MiraiCP
