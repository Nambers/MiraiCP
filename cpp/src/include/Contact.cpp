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
    }

    MessageSource Contact::sendVoiceImpl(std::string path) const {
        json j{{"path", std::move(path)},
               {"contact", toJson()}};
        std::string re = KtOperation::ktOperation(KtOperation::Voice, j);
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
        return {{"id", _id}, {"botId", _botId}, {"type", _type}};
    }

    void IContactData::updateJson(json &json_to_update) const {
        json_to_update.update(toJson(), true);
    }

    nlohmann::json GroupRelatedData::internalToJson() const {
        auto result = IContactData::internalToJson();
        result["groupId"] = _groupId;
        return result;
    }

    MessageSource Contact::quoteAndSend0(std::string msg, const MessageSource &ms) {
        json obj{{"messageSource", ms.serializeToString()},
                 {"msg",           std::move(msg)},
                 {"contact",        toJson()}};
        std::string re = KtOperation::ktOperation(KtOperation::SendWithQuote, obj);
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
