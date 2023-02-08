// Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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

#include "Group.h"
#include "ExceptionHandle.h"
#include "Exceptions/IllegalArgument.h"
#include "Exceptions/IllegalState.h"
#include "Exceptions/RemoteAsset.h"
#include "Exceptions/Upload.h"
#include "JsonTools.h"
#include "KtOperation.h"
#include "LowLevelAPI.h"
#include "Member.h"
#include "Tools.h"


namespace MiraiCP {
#define LOC_CLASS_NAMESPACE Group

    using json = nlohmann::json;

    std::string Group::MemberListToString() {
        return Tools::VectorToString(getMemberList());
    }

    auto GetGroupFromPool(QQID groupid, QQID botid) noexcept {
        static std::unordered_map<QQID, std::unordered_map<QQID, std::shared_ptr<Group::DataType>>> Pool;
        auto &Val = Pool[botid][groupid];
        if (!Val) {
            Val = std::make_shared<Group::DataType>(groupid);
            Val->_botId = botid;
            Val->_id = groupid;
            Val->_type = MIRAI_GROUP;
        }
        return Val;
    }

    auto GetGroupFromPool(const json &in_json) {
        try {
            return GetGroupFromPool(in_json["id"], in_json["id2"]);
        } catch (const nlohmann::detail::exception &) {
            throw IllegalArgumentException("构造Group时传入的json异常", MIRAICP_EXCEPTION_WHERE);
        }
    }

    Group::Group(QQID groupId, QQID botId) : Contact(GetGroupFromPool(groupId, botId)) {
        forceRefreshNextTime();
    }

    Group::Group(json in_json) : Contact(GetGroupFromPool(in_json)) {
        auto ActualDataPtr = GetDataInternal();
        assert(ActualDataPtr != nullptr);

        bool needRefresh = false;

        if (in_json.contains("nickOrNameCard"))
            ActualDataPtr->_nickOrNameCard = json_stringmover(in_json, "nickOrNameCard");
        else
            needRefresh = true;
        if (in_json.contains("avatarUrl")) ActualDataPtr->_avatarUrl = json_stringmover(in_json, "avatarUrl");
        else
            needRefresh = true;

        if (needRefresh) forceRefreshNextTime();
    }

    std::vector<Group::OnlineAnnouncement> Group::getAnnouncementsList() {
        json j{{"contact",      toJson()},
               {"announcement", true}};
        std::string re = KtOperation::ktOperation(KtOperation::RefreshInfo, j);
        std::vector<OnlineAnnouncement> oa;
        for (const json &e: json::parse(re)) {
            oa.push_back(Group::OnlineAnnouncement::deserializeFromJson(e));
        }
        return oa;
    }

    void Group::OnlineAnnouncement::deleteThis() {
        json j{{"botId", botId},
               {"groupId", groupId},
               {"fid", fid},
               {"type", KtOperation::AnnouncementOperationCode::Delete}};
        std::string re = KtOperation::ktOperation(KtOperation::Announcement, j);
        if (re == "E1")
            throw IllegalArgumentException("无法根据fid找到群公告(群公告不存在)", MIRAICP_EXCEPTION_WHERE);
        if (re == "E3")
            throw IllegalStateException("群公告状态异常", MIRAICP_EXCEPTION_WHERE);
    }

    json Group::AnnouncementParams::serializeToJson() {
        return {
                {"sendToNewMember",     send2new},
                {"isPinned",            pinned},
                {"showEditCard",        showEditCard},
                {"showPopup",           showPopup},
                {"requireConfirmation", requireConfirm}};
    }

    Group::OnlineAnnouncement Group::OfflineAnnouncement::publishTo(const Group &g) {
        json j{{"botId",   g.botid()},
               {"groupId", g.id()},
               {"type",    KtOperation::AnnouncementOperationCode::Publish},
               {"source",  {{"content", content},
                                   {"params", params.serializeToJson()}}}};
        std::string re = KtOperation::ktOperation(KtOperation::Announcement, j);
        MIRAICP_ERROR_HANDLE(re, "");
        return Group::OnlineAnnouncement::deserializeFromJson(json::parse(re));
    }

    Group::OnlineAnnouncement Group::OnlineAnnouncement::deserializeFromJson(const json &j) {
        return Group::OnlineAnnouncement{
                j["content"],
                j["botId"],
                j["groupId"],
                j["senderId"],
                j["time"],
                j["fid"],
                j["imageId"],
                j["confirmationNum"],
                {j["params"]["sendToNewMember"],
                 j["params"]["requireConfirmation"],
                 j["params"]["isPinned"],
                 j["params"]["showEditCard"],
                 j["params"]["showPopup"]}};
    }

    std::vector<QQID> Group::getMemberList() {
        nlohmann::json j{{"contact", toJson()},
                         {"type", KtOperation::QueryBotListCode::MemberList}};
        std::string re = KtOperation::ktOperation(KtOperation::QueryBotList, j);
        MIRAICP_ERROR_HANDLE(re, "");
        return Tools::StringToVector(std::move(re));
    }

    void Group::quit() {
        nlohmann::json j{{"contact", toJson()},
                         {"quit",    true}};
        MIRAICP_ERROR_HANDLE(KtOperation::ktOperation(KtOperation::RefreshInfo, j), "");
    }

    void Group::updateSetting(GroupData::GroupSetting newSetting) {
        json j{{"name",                   std::move(newSetting.name)},
               {"isMuteAll",              newSetting.isMuteAll},
               {"isAllowMemberInvite",    newSetting.isAllowMemberInvite},
               {"isAutoApproveEnabled",   newSetting.isAutoApproveEnabled},
               {"isAnonymousChatEnabled", newSetting.isAnonymousChatEnabled},
               {"contact",                toJson()}};
        MIRAICP_ERROR_HANDLE(KtOperation::ktOperation(KtOperation::GroupSetting, j), "");
        InternalData->forceRefreshNextTime();
    }

    RemoteFile Group::sendFile(const std::string &path, const std::string &filepath) {
        json j{{"path",     path},
               {"filePath", filepath},
               {"contact",  toJson()}};
        auto re = KtOperation::ktOperation(KtOperation::SendFile, j);
        if (re == "E2") throw UploadException("找不到" + filepath + "位置:C-uploadfile", MIRAICP_EXCEPTION_WHERE);
        if (re == "E3")
            throw UploadException(
                    "Upload error:路径格式异常，应为'/xxx.xxx'或'/xx/xxx.xxx'目前只支持群文件和单层路径, path:" + path,
                    MIRAICP_EXCEPTION_WHERE);
        MIRAICP_ERROR_HANDLE(re, "");
        return RemoteFile::deserializeFromString(re);
    }

    RemoteFile Group::getFile(const std::string &path, const std::string &id) {
        // source 参数
        if (path.empty() || path == "/")
            return this->getFileById(id);
        json j{{"id",      id},
               {"path",    path},
               {"contact", toJson()}};
        std::string re = KtOperation::ktOperation(KtOperation::RemoteFileInfo, j);
        if (re == "E2")
            throw RemoteAssetException("Get error: 文件路径不存在, path:" + path + ",id:" + id,
                                       MIRAICP_EXCEPTION_WHERE);
        MIRAICP_ERROR_HANDLE(re, "");
        return RemoteFile::deserializeFromString(re);
    }

    RemoteFile Group::getFileById(const std::string &id) {
        json j{{"id",      id},
               {"path",    "-1"},
               {"contact", toJson()}};
        std::string re = KtOperation::ktOperation(KtOperation::RemoteFileInfo, j);
        if (re == "E1") throw RemoteAssetException("Get error: 文件路径不存在, id:" + id, MIRAICP_EXCEPTION_WHERE);
        MIRAICP_ERROR_HANDLE(re, "");
        return RemoteFile::deserializeFromString(re);
    }

    Member Group::getOwner() {
        std::string re = KtOperation::ktOperation(KtOperation::QueryOwner, toJson());
        MIRAICP_ERROR_HANDLE(re, "");
        return Member(std::stoll(re), this->id(), this->botid());
    }

    std::string Group::getFileListString(const std::string &path) {
        json j{{"id",      "-1"},
               {"path",    path},
               {"contact", toJson()}};
        auto re = KtOperation::ktOperation(KtOperation::RemoteFileInfo, j);
        MIRAICP_ERROR_HANDLE(re, "");
        return re;
    }

    std::vector<Group::file_short_info> Group::getFileList(const std::string &path) {
        std::vector<file_short_info> re = std::vector<file_short_info>();
        std::string tmp = getFileListString(path);
        json root = json::parse(tmp);
        for (auto &i: root) {
            file_short_info t;
            t.path = i[0];
            t.id = i[1];
            re.push_back(t);
        }
        return re;
    }

    Member Group::getMember(QQID memberid) {
        return Member(memberid, this->id(), this->botid());
    }

    Member Group::operator[](QQID a) {
        return getMember(a);
    }

    IMPL_GETTER(setting)

    void GroupData::refreshInfo() {
        std::string re = LowLevelAPI::getInfoSource(internalToJson());
        LowLevelAPI::info tmp = LowLevelAPI::info0(re);
        this->_nickOrNameCard = std::move(tmp.nickOrNameCard);
        this->_avatarUrl = std::move(tmp.avatarUrl);
        nlohmann::json j = nlohmann::json::parse(re)["setting"];
        this->_setting.name = json_stringmover(j, "name");
        this->_setting.isMuteAll = j["isMuteAll"];
        this->_setting.isAllowMemberInvite = j["isAllowMemberInvite"];
        this->_setting.isAutoApproveEnabled = j["isAutoApproveEnabled"];
        this->_setting.isAnonymousChatEnabled = j["isAnonymousChatEnabled"];
    }

    void GroupData::deserialize(nlohmann::json in_json) {
        _groupId = in_json["groupId"];
        IContactData::deserialize(std::move(in_json));
    }

    std::optional<Member> Group::queryCurrentHonorMember(HonorMemberType type) {
        auto re = KtOperation::ktOperation(KtOperation::queryHonorMember,
                                           json{{"honorType", static_cast<int>(type)},
                                                {"contact",   toJson()}});
        if (re == "E1") {
            return std::nullopt;
        }
        return Member(json::parse(re));
    }

#undef LOC_CLASS_NAMESPACE
} // namespace MiraiCP
