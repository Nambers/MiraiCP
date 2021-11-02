#include "LowLevelAPI.h"

namespace MiraiCP {
    Image Contact::uploadImg(const std::string &path, JNIEnv *env) {
        std::string re = LowLevelAPI::uploadImg0(path, this, env);
        if (re == "E2")
            MiraiCPThrow(UploadException("上传图片大小超过30MB,路径:" + path));
        return Image(re);
    }

    Contact Contact::deserializationFromString(const std::string &source) {
        json j;
        try {
            j = json::parse(source);
        } catch (json::parse_error &e) {
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
            MiraiCPThrow(UploadException("上传语音文件格式不对(必须为.amr/.silk)或文件不存在"));
        else if (re == "E2")
            MiraiCPThrow(UploadException("上传语音文件大小超过服务器限制，一般限制在1MB上下"));
        return MessageSource::deserializeFromString(re);
    }
} // namespace MiraiCP
