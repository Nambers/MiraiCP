// Copyright (c) 2021-2021. Eritque arcus and contributors.
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

#include "LowLevelAPI.h"
#include "Contact.h"
#include "Exception.h"
#include "Tools.h"
#include "Config.h"

namespace MiraiCP {
    using json = nlohmann::json;
    [[deprecated("Use sendMessage")]] MessageSource Contact::sendMsg(std::vector<std::string> msg, int retryTime, JNIEnv *env) {
        return sendMsg0(Tools::VectorToString(std::move(msg)), retryTime, false, env);
    }
    MessageSource Contact::sendMsg0(const std::string &msg, int retryTime, bool miraicode, JNIEnv *env) const {
        if (msg.empty()) {
            MiraiCPThrow(IllegalArgumentException("不能发送空信息, 位置: Contact::SendMsg"));
        }
        std::string re = LowLevelAPI::send0(msg, this->toJson(), retryTime, miraicode, env,
                                            "reach a error area, Contact::SendMiraiCode");
        if (re == "ET")
            MiraiCPThrow(TimeOutException("发送消息过于频繁导致的tx服务器未能即使响应, 位置: Contact::SendMsg"));
        if (Tools::starts_with(re, "EBM"))
            MiraiCPThrow(BotIsBeingMutedException(std::stoi(re.substr(3))));
        return MessageSource::deserializeFromString(re);
    }
    MessageSource Contact::quoteAndSend0(const std::string &msg, MessageSource ms, JNIEnv *env) {
        json obj;
        json sign;
        obj["messageSource"] = ms.serializeToString();
        obj["msg"] = msg;
        sign["MiraiCode"] = true;
        sign["groupid"] = this->groupid();
        obj["sign"] = sign.dump();
        std::string re = Config::koperation(Config::SendWithQuote, obj, env);
        return MessageSource::deserializeFromString(re);
    }

    Image Contact::uploadImg(const std::string &path, JNIEnv *env) const {
        std::string re = LowLevelAPI::uploadImg0(path, this->toString(), env);
        if (re == "E2")
            MiraiCPThrow(UploadException("上传图片大小超过30MB,路径:" + path));
        return Image::deserialize(re);
    }


    Contact Contact::deserialize(const std::string &source) {
        json j;
        try {
            j = json::parse(source);
        } catch (json::parse_error &e) {
            Logger::logger.error("json序列化错误 Contact::deserializationFromString");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
        }
        return Contact::deserialize(j);
    }

    Contact Contact::deserialize(nlohmann::json j) {
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
        j["contactSource"] = this->toString();
        std::string re = Config::koperation(Config::Voice, j, env);
        if (re == "E1")
            MiraiCPThrow(UploadException("上传语音文件格式不对(必须为.amr/.silk)或文件不存在"));
        else if (re == "E2")
            MiraiCPThrow(UploadException("上传语音文件大小超过服务器限制，一般限制在1MB上下"));
        return MessageSource::deserializeFromString(re);
    }
} // namespace MiraiCP
