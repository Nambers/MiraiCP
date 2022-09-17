// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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

#include "Contact.h"
#include "Exception.h"
#include "KtOperation.h"
#include "Logger.h"
#include "LowLevelAPI.h"
#include "Tools.h"

namespace MiraiCP {
    using json = nlohmann::json;

    //    Contact Contact::deserialize(const std::string &source) {
    //        json j;
    //        try {
    //            j = json::parse(source);
    //        } catch (json::parse_error &e) {
    //            Logger::logger.error("json序列化错误 Contact::deserializationFromString");
    //            Logger::logger.error(source);
    //            Logger::logger.error(e.what());
    //        }
    //        return Contact::deserialize(j);
    //    }

    std::shared_ptr<Contact> Contact::deserialize(nlohmann::json j) {
        uint8_t thistype = j["type"];
        switch (thistype) {
            case MIRAI_FRIEND: {
            }
                // TODO(Antares): 返回实际子类对象的指针
                //  没有的field就不要构造，比如anonymous，Friend没有就不要往里面填
                // case xxx:
        }
        assert(0);
        return {};
        //        return Contact(j["type"],
        //                       j["id"],
        //                       j["groupid"],
        //                       j["nickornamecard"],
        //                       j["botid"],
        //                       j["anonymous"]);
    }

    void Contact::deserializeWriter(const nlohmann::json &source) {
        _type = static_cast<ContactType>(source["type"]);
        _id = source["id"];
        //        _groupid = source["groupid"];
        //        _nickOrNameCard = source["nickornamecard"];
        //        _botid = source["botid"];
        //        _anonymous = source["anonymous"];
    }

    //    Contact::operator ContactWithSendSupport() const {
    //        return {type(), id(),
    //                //groupid(), nickOrNameCard(), botid(), _anonymous
    //        };
    //    }

    MessageSource ContactWithSendSupport::sendMsg0(std::string msg, int retryTime, bool miraicode) const {
        if (msg.empty()) {
            throw IllegalArgumentException("不能发送空信息, 位置: Contact::SendMsg", MIRAICP_EXCEPTION_WHERE);
        }
        std::string re = LowLevelAPI::send0(std::move(msg), this->toJson(), retryTime, miraicode,
                                            "reach a error area, Contact::SendMiraiCode");
        MIRAICP_ERROR_HANDLE(re, "");
        return MessageSource::deserializeFromString(re);
    }

    MessageSource ContactWithSendSupport::quoteAndSend0(const std::string &msg, const MessageSource &ms) {
        json obj;
        json sign;
        obj["messageSource"] = ms.serializeToString();
        obj["msg"] = msg;
        sign["MiraiCode"] = true;
        // sign["groupid"] = this->groupid();
        obj["sign"] = sign.dump();
        std::string re = KtOperation::ktOperation(KtOperation::SendWithQuote, obj);
        MIRAICP_ERROR_HANDLE(re, "");
        return MessageSource::deserializeFromString(re);
    }

    Image ContactWithSendSupport::uploadImg(const std::string &path) const {
        std::string re = LowLevelAPI::uploadImg0(path, this->toString());
        if (re == "E2")
            throw UploadException("上传图片大小超过30MB,路径:" + path, MIRAICP_EXCEPTION_WHERE);
        return Image::deserialize(re);
    }

    FlashImage ContactWithSendSupport::uploadFlashImg(const std::string &path) const {
        std::string re = LowLevelAPI::uploadImg0(path, this->toString());
        if (re == "E2")
            throw UploadException("上传图片大小超过30MB,路径:" + path, MIRAICP_EXCEPTION_WHERE);
        return FlashImage::deserialize(re);
    }

    MessageSource ContactWithSendSupport::sendVoice0(const std::string &path) {
        json j;
        json source;
        source["path"] = path;
        j["source"] = source.dump();
        j["contactSource"] = this->toString();
        std::string re = KtOperation::ktOperation(KtOperation::Voice, j);
        if (re == "E1") {
            throw UploadException("上传语音文件格式不对(必须为.amr/.silk)或文件不存在", MIRAICP_EXCEPTION_WHERE);
        } else if (re == "E2") {
            throw UploadException("上传语音文件大小超过服务器限制，一般限制在1MB上下", MIRAICP_EXCEPTION_WHERE);
        }
        return MessageSource::deserializeFromString(re);
    }

    ContactWithSendSupport::ContactWithSendSupport(QQID id, QQID botid, ContactType type) : Contact(id, botid, type) {}
    //Contact(type, id, gid, name, botid, anonymous) {}

} // namespace MiraiCP
