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
#include "Friend.h"
#include "Group.h"
#include "IMiraiData.h"
#include "KtOperation.h"
#include "Logger.h"
#include "LowLevelAPI.h"
#include "Member.h"
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

    std::shared_ptr<Contact> Contact::deserializeToPointer(nlohmann::json j) {
        uint8_t thistype = j["type"];
        switch (thistype) {
            case MIRAI_FRIEND: {
                return std::make_shared<Friend>(std::move(j));
            }
            case MIRAI_MEMBER: {
                return std::make_shared<Member>(std::move(j));
            }
            case MIRAI_GROUP: {
                return std::make_shared<Group>(std::move(j));
            }
            default:
                assert(0);
                break;
        }

        return {};
        //        return Contact(j["type"],
        //                       j["id"],
        //                       j["groupid"],
        //                       j["nickornamecard"],
        //                       j["botid"],
        //                       j["anonymous"]);
    }

    //    void Contact::deserializeWriter(const nlohmann::json &source) {
    //        _type = static_cast<ContactType>(source["type"]);
    //        _id = source["id"];
    //        //        _groupid = source["groupid"];
    //        //        _nickOrNameCard = source["nickornamecard"];
    //        //        _botid = source["botid"];
    //        //        _anonymous = source["anonymous"];
    //    }

    //    Contact::Contact(const nlohmann::json &in_json)
    //        : Contact(in_json["id"], in_json["groupid"], static_cast<ContactType>(in_json["type"])) {
    //        // todo
    //    }
    //    Contact::Contact(QQID in_id, QQID in_botid, ContactType in_type) {
    //        // todo
    //    }


    MessageSource Contact::sendVoiceImpl(std::string path) const {
        json source{{"path", std::move(path)}};
        json j{{"source", source.dump()},
               {"contactSource", toString()}};
        std::string re = KtOperation::ktOperation(KtOperation::Voice, std::move(j));
        if (re == "E1") {
            throw UploadException("上传语音文件格式不对(必须为.amr/.silk)或文件不存在", MIRAICP_EXCEPTION_WHERE);
        } else if (re == "E2") {
            throw UploadException("上传语音文件大小超过服务器限制，一般限制在1MB上下", MIRAICP_EXCEPTION_WHERE);
        }
        return MessageSource::deserializeFromString(re);
    }

    void IContactData::deserialize(nlohmann::json in_json) {
        using Tools::json_stringmover;
        _nickOrNameCard = json_stringmover(in_json, "nickornamecard");
        _avatarUrl = json_stringmover(in_json, "avatarUrl");
    }

    void IContactData::refreshInfo() {
        // default to Friend
        std::string temp = LowLevelAPI::getInfoSource(internalToString());
        if (temp == "E1") {
            throw FriendException(MIRAICP_EXCEPTION_WHERE);
        }
        LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
        this->_nickOrNameCard = tmp.nickornamecard;
        this->_avatarUrl = tmp.avatarUrl;
    }

    nlohmann::json IContactData::internalToJson() const {
        return {{"nickornamecard", _nickOrNameCard}, {"id", _id}, {"botid", _botid}, {"type", _type}};
        // j["type"] = type();
        //            j["id"] = id();
        //            j["groupid"] = groupid();
        //            j["nickornamecard"] = nickOrNameCard();
        //            j["botid"] = botid();
    }

    nlohmann::json IContactData::getSign() const {
        return {{"MiraiCode", true}};
    }

    void IContactData::updateJson(json &json_to_update) const {
        json_to_update.update(toJson(), true);
    }

    nlohmann::json GroupRelatedData::internalToJson() const {
        auto result = IContactData::internalToJson();
        result["groupid"] = _groupid;
        return result;
    }

    nlohmann::json GroupRelatedData::getSign() const {
        auto ans = Super::getSign();
        ans["groupid"] = _groupid;
        return ans;
    }

    MessageSource Contact::quoteAndSend0(std::string msg, const MessageSource &ms) {
        json sign = InternalData->getSign();
        json obj{{"messageSource", ms.serializeToString()}, {"msg", std::move(msg)}, {"sign", sign.dump()}};
        std::string re = KtOperation::ktOperation(KtOperation::SendWithQuote, std::move(obj));
        MIRAICP_ERROR_HANDLE(re, "");
        return MessageSource::deserializeFromString(re);
    }

    Image Contact::uploadImg(const std::string &path) const {
        std::string re = LowLevelAPI::uploadImg0(path, toString());
        if (re == "E2")
            throw UploadException("上传图片大小超过30MB,路径:" + path, MIRAICP_EXCEPTION_WHERE);
        return Image::deserialize(re);
    }

    FlashImage Contact::uploadFlashImg(const std::string &path) const {
        std::string re = LowLevelAPI::uploadImg0(path, toString());
        if (re == "E2")
            throw UploadException("上传图片大小超过30MB,路径:" + path, MIRAICP_EXCEPTION_WHERE);
        return FlashImage::deserialize(re);
    }

    MessageSource Contact::sendMsgImpl(std::string msg, int retryTime, bool miraicode) const {
        if (msg.empty()) {
            throw IllegalArgumentException("不能发送空信息, 位置: Contact::SendMsg", MIRAICP_EXCEPTION_WHERE);
        }
        std::string re = LowLevelAPI::send0(std::move(msg), InternalData->toJson(), retryTime, miraicode,
                                            "reach a error area, Contact::SendMiraiCode");
        MIRAICP_ERROR_HANDLE(re, "");
        return MessageSource::deserializeFromString(re);
    }
} // namespace MiraiCP
