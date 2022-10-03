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

#include "Group.h"
#include "Exception.h"
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
            Val->_botid = botid;
            Val->_id = groupid;
            Val->_type = MIRAI_GROUP;
        }
        return Val;
    }

    auto GetGroupFromPool(const json &in_json) {
        try {
            // todo(Antares): group 的 in_json 的 groupid 项是空值就很离谱，上游这里为什么这样设计？
            return GetGroupFromPool(in_json["id"], in_json["botid"]);
        } catch (const nlohmann::detail::exception &) {
            throw IllegalArgumentException("构造Group时传入的json异常", MIRAICP_EXCEPTION_WHERE);
        }
    }

    Group::Group(QQID groupid, QQID botid) : Contact(GetGroupFromPool(groupid, botid)) {
        forceRefreshNextTime();
    }

    Group::Group(json in_json) : Contact(GetGroupFromPool(in_json)) {
        auto ActualDataPtr = GetDataInternal();
        assert(ActualDataPtr != nullptr);
        ActualDataPtr->_nickOrNameCard = Tools::json_stringmover(in_json, "nickornamecard");
        if (in_json.contains("avatarUrl"))
            ActualDataPtr->_avatarUrl = Tools::json_stringmover(in_json, "avatarUrl");
        else
            forceRefreshNextTime();
    }

    std::vector<Group::OnlineAnnouncement> Group::getAnnouncementsList() {
        json j{{"source", toString()}, {"announcement", true}};
        std::string re = KtOperation::ktOperation(KtOperation::RefreshInfo, std::move(j));
        std::vector<OnlineAnnouncement> oa;
        for (const json &e: json::parse(re)) {
            oa.push_back(Group::OnlineAnnouncement::deserializeFromJson(e));
        }
        return oa;
    }

    void Group::OnlineAnnouncement::deleteThis() {
        json i{{"botid", botid}, {"groupid", groupid}, {"fid", fid}, {"type", 1}};
        json j{{"identify", i.dump()}};
        std::string re = KtOperation::ktOperation(KtOperation::Announcement, std::move(j));
        if (re == "E1")
            throw IllegalArgumentException("无法根据fid找到群公告(群公告不存在)", MIRAICP_EXCEPTION_WHERE);
        if (re == "E3")
            throw IllegalStateException("群公告状态异常", MIRAICP_EXCEPTION_WHERE);
    }

    json Group::AnnouncementParams::serializeToJson() {
        return {
                {"sendToNewMember", send2new},
                {"isPinned", pinned},
                {"showEditCard", showEditCard},
                {"showPopup", showPopup},
                {"requireConfirmation", requireConfirm}};
    }

    Group::OnlineAnnouncement Group::OfflineAnnouncement::publishTo(const Group &g) {
        json i{{"botid", g.botid()}, {"groupid", g.id()}, {"type", 2}};
        json s{{"content", content}, {"params", params.serializeToJson()}};
        json j{{"identify", i.dump()}, {"source", s.dump()}};
        std::string re = KtOperation::ktOperation(KtOperation::Announcement, std::move(j));
        return Group::OnlineAnnouncement::deserializeFromJson(json::parse(re));
    }

    Group::OnlineAnnouncement Group::OnlineAnnouncement::deserializeFromJson(const json &j) {
        return Group::OnlineAnnouncement{
                j["content"],
                j["botid"],
                j["groupid"],
                j["senderid"],
                j["time"],
                j["fid"],
                j["imageid"],
                j["confirmationNum"],
                {j["params"]["sendToNewMember"],
                 j["params"]["requireConfirmation"],
                 j["params"]["isPinned"],
                 j["params"]["showEditCard"],
                 j["params"]["showPopup"]}};
    }

    std::vector<unsigned long long> Group::getMemberList() {
        nlohmann::json j{{"contactSource", toString()}};
        std::string re = KtOperation::ktOperation(KtOperation::QueryML, std::move(j));
        if (re == "E1") {
            throw GroupException(MIRAICP_EXCEPTION_WHERE);
        }
        return Tools::StringToVector(std::move(re));
    }

    void Group::quit() {
        nlohmann::json j{{"source", toString()}, {"quit", true}};
        KtOperation::ktOperation(KtOperation::RefreshInfo, std::move(j));
    }

    void Group::updateSetting() {
        auto settings = setting();
        json j{
                {"name", std::move(settings.name)},
                {"isMuteAll", settings.isMuteAll},
                {"isAllowMemberInvite", settings.isAllowMemberInvite},
                {"isAutoApproveEnabled", settings.isAutoApproveEnabled},
                {"isAnonymousChatEnabled", settings.isAnonymousChatEnabled}};
        json tmp{{"source", j.dump()}, {"contactSource", toString()}};
        std::string re = KtOperation::ktOperation(KtOperation::GroupSetting, std::move(tmp));
        InternalData->forceRefreshNextTime();
    }

    RemoteFile Group::sendFile(const std::string &path, const std::string &filepath) {
        json source{{"path", path}, {"filepath", filepath}};
        json tmp{{"source", source.dump()}, {"contactSource", toString()}};
        std::string result = KtOperation::ktOperation(KtOperation::SendFile, std::move(tmp));
        if (result == "E2") throw UploadException("找不到" + filepath + "位置:C-uploadfile", MIRAICP_EXCEPTION_WHERE);
        if (result == "E3")
            throw UploadException("Upload error:路径格式异常，应为'/xxx.xxx'或'/xx/xxx.xxx'目前只支持群文件和单层路径, path:" + path, MIRAICP_EXCEPTION_WHERE);
        return RemoteFile::deserializeFromString(result);
    }

    RemoteFile Group::getFile(const std::string &path, const std::string &id) {
        // source 参数
        if (path.empty() || path == "/")
            return this->getFileById(id);
        json tmp{{"id", id}, {"path", path}};
        json j{{"source", tmp.dump()}, {"contactSource", toString()}};
        std::string re = KtOperation::ktOperation(KtOperation::RemoteFileInfo, std::move(j));
        if (re == "E2") throw RemoteAssetException("Get error: 文件路径不存在, path:" + path + ",id:" + id, MIRAICP_EXCEPTION_WHERE);
        return RemoteFile::deserializeFromString(re);
    }

    RemoteFile Group::getFileById(const std::string &id) {
        json tmp{{"id", id}};
        json j{{"source", tmp.dump()}, {"contactSource", toString()}};
        std::string re = KtOperation::ktOperation(KtOperation::RemoteFileInfo, std::move(j));
        if (re == "E1") throw RemoteAssetException("Get error: 文件路径不存在, id:" + id, MIRAICP_EXCEPTION_WHERE);
        return RemoteFile::deserializeFromString(re);
    }

    Member Group::getOwner() {
        json j{{"contactSource", toString()}};
        std::string re = KtOperation::ktOperation(KtOperation::QueryOwner, std::move(j));
        return Member(stoi(re), this->id(), this->botid());
    }

    std::string Group::getFileListString(const std::string &path) {
        json temp{{"id", "-1"}, {"path", path}};
        json j{{"source", temp.dump()}, {"contactSource", toString()}};
        return KtOperation::ktOperation(KtOperation::RemoteFileInfo, std::move(j));
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
        std::string re = LowLevelAPI::getInfoSource(internalToString());
        LowLevelAPI::info tmp = LowLevelAPI::info0(re);
        this->_nickOrNameCard = std::move(tmp.nickornamecard);
        this->_avatarUrl = std::move(tmp.avatarUrl);
        nlohmann::json j = nlohmann::json::parse(re)["setting"];
        this->_setting.name = Tools::json_stringmover(j, "name");
        this->_setting.isMuteAll = j["isMuteAll"];
        this->_setting.isAllowMemberInvite = j["isAllowMemberInvite"];
        this->_setting.isAutoApproveEnabled = j["isAutoApproveEnabled"];
        this->_setting.isAnonymousChatEnabled = j["isAnonymousChatEnabled"];
    }

    void GroupData::deserialize(nlohmann::json in_json) {
        _groupid = in_json["groupid"];
        IContactData::deserialize(std::move(in_json));
    }

#undef LOC_CLASS_NAMESPACE
} // namespace MiraiCP
