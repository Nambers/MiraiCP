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
#include "ContactDataType/GroupRelatedData.h"
#include "Exception.h"
#include "ExceptionHandle.h"
#include "Friend.h"
#include "Group.h"
#include "JsonTools.h"
#include "KtOperation.h"
#include "LowLevelAPI.h"
#include "Member.h"


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
        _nickOrNameCard = json_stringmover(in_json, "nickOrNameCard");
        _avatarUrl = json_stringmover(in_json, "avatarUrl");
    }

    void IContactData::refreshInfo() {
        // default to Friend
        std::string temp = LowLevelAPI::getInfoSource(internalToJson());
        MIRAICP_ERROR_HANDLE(temp, "");
        LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
        this->_nickOrNameCard = tmp.nickOrNameCard;
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

    MessageSource Contact::quoteAndSendImpl(std::string msg, const MessageSource &ms) const {
        json obj{{"messageSource", ms.serializeToString()},
                 {"msg", std::move(msg)},
                 {"contact", toJson()}};
        std::string re = KtOperation::ktOperation(KtOperation::SendWithQuote, obj);
        return MessageSource::deserializeFromString(re);
    }

    Image Contact::uploadImg(const std::string &path) const {
        std::string re = LowLevelAPI::uploadImg0(path, toJson());
        if (re == "E2")
            throw UploadException("上传图片大小超过30MB,路径:" + path, MIRAICP_EXCEPTION_WHERE);
        return Image::deserialize(re);
    }

    FlashImage Contact::uploadFlashImg(const std::string &path) const {
        std::string re = LowLevelAPI::uploadImg0(path, toJson());
        if (re == "E2")
            throw UploadException("上传图片大小超过30MB,路径:" + path, MIRAICP_EXCEPTION_WHERE);
        return FlashImage::deserialize(re);
    }

    MessageSource Contact::sendMsgImpl(std::string msg) const {
        if (msg.empty()) {
            throw IllegalArgumentException("不能发送空信息, 位置: Contact::SendMsg", MIRAICP_EXCEPTION_WHERE);
        }
        nlohmann::json j{{"message", std::move(msg)}, {"contact", toJson()}};
        auto re = KtOperation::ktOperation(KtOperation::Send, j, true, "reach a error area, Contact::sendMsgImpl");
        return MessageSource::deserializeFromString(re);
    }

    void Contact::SetInternalData(std::shared_ptr<IContactData> Data) { InternalData = std::move(Data); }

    Contact::Contact(std::shared_ptr<IContactData> Data) {
        SetInternalData(std::move(Data));
    }

    ContactType Contact::type() const { return InternalData->_type; }

    QQID Contact::id() const { return InternalData->_id; }

    QQID Contact::botid() const { return InternalData->_botId; }

    nlohmann::json Contact::toJson() const { return InternalData->toJson(); }

    void Contact::updateJson(json &j) const { InternalData->updateJson(j); }

    std::string Contact::toString() const { return toJson().dump(); }

    void Contact::refreshInfo() {
        InternalData->requestRefresh();
    }

    void Contact::forceRefreshNextTime() {
        InternalData->forceRefreshNextTime();
    }

    void Contact::forceRefreshNow() {
        forceRefreshNextTime();
        refreshInfo();
    }

    bool Contact::operator==(const Contact &c) const {
        return id() == c.id() && InternalData->_type == c.InternalData->_type;
    }

    MessageSource Contact::unpackMsg(const MessageChain &msg) const {
        return sendMsgImpl(msg.toString());
    }

    MessageSource Contact::unpackMsg(const MiraiCodeable &msg) const {
        return sendMsgImpl(MessageChain::deserializationFromMiraiCode(msg.toMiraiCode()).toString());
    }

    MessageSource Contact::unpackMsg(std::string msg) const {
        return sendMsgImpl(MessageChain(PlainText(std::move(msg))).toString());
    }

    MessageSource Contact::unpackQuote(const SingleMessage &s, const MessageSource &ms) const {
        return quoteAndSendImpl(MessageChain(s).toString(), ms);
    }

    MessageSource Contact::unpackQuote(const std::string &s, const MessageSource &ms) const {
        return quoteAndSendImpl(s, ms);
    }

    MessageSource Contact::unpackQuote(const MessageChain &mc, const MessageSource &ms) const {
        return quoteAndSendImpl(mc.toString(), ms);
    }

    std::string internal::getNickFromIContactPtr(IContactData *p) {
        p->requestRefresh();
        std::shared_lock<std::shared_mutex> local_lck(p->getMutex());
        return p->_nickOrNameCard;
    }

    std::string internal::getAvatarUrlFromIContactPtr(IContactData *p) {
        p->requestRefresh();
        std::shared_lock<std::shared_mutex> local_lck(p->getMutex());
        return p->_avatarUrl;
    }
} // namespace MiraiCP
