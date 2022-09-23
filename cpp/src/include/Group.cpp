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
        forceRefreshNexttime();
    }

    Group::Group(json in_json) : Contact(GetGroupFromPool(in_json)) {
        auto ActualDataPtr = GetDataInternal();
        assert(ActualDataPtr != nullptr);
        ActualDataPtr->_nickOrNameCard = Tools::json_stringmover(in_json, "nickornamecard");
        if (in_json.contains("avatarUrl")) ActualDataPtr->_avatarUrl = Tools::json_stringmover(in_json, "avatarUrl");
        else
            forceRefreshNexttime();
    }

    std::vector<Group::OnlineAnnouncement> Group::getAnnouncementsList() {
        json j;
        j["source"] = this->toString();
        j["announcement"] = true;
        std::string re = KtOperation::ktOperation(KtOperation::RefreshInfo, j);
        std::vector<OnlineAnnouncement> oa;
        for (const json &e: json::parse(re)) {
            oa.push_back(Group::OnlineAnnouncement::deserializeFromJson(e));
        }
        return oa;
    }

    void Group::OnlineAnnouncement::deleteThis() {
        json j, i;
        i["botid"] = this->botid;
        i["groupid"] = this->groupid;
        i["fid"] = this->fid;
        i["type"] = 1;
        j["identify"] = i.dump();
        std::string re = KtOperation::ktOperation(KtOperation::Announcement, j);
        if (re == "E1")
            throw IllegalArgumentException("无法根据fid找到群公告(群公告不存在)", MIRAICP_EXCEPTION_WHERE);
        if (re == "E3")
            throw IllegalStateException("群公告状态异常", MIRAICP_EXCEPTION_WHERE);
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
        std::string re = KtOperation::ktOperation(KtOperation::Announcement, j);
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
        nlohmann::json j;
        j["contactSource"] = this->toString();
        std::string re = KtOperation::ktOperation(KtOperation::QueryML, j);
        if (re == "E1") {
            throw GroupException(MIRAICP_EXCEPTION_WHERE);
        }
        return Tools::StringToVector(std::move(re));
    }

    void Group::quit() {
        nlohmann::json j;
        j["source"] = this->toString();
        j["quit"] = true;
        KtOperation::ktOperation(KtOperation::RefreshInfo, j);
    }

    void Group::updateSetting() {
        json j;
        json tmp;
        auto settings = setting();
        j["name"] = settings.name;
        j["isMuteAll"] = settings.isMuteAll;
        j["isAllowMemberInvite"] = settings.isAllowMemberInvite;
        j["isAutoApproveEnabled"] = settings.isAutoApproveEnabled;
        j["isAnonymousChatEnabled"] = settings.isAnonymousChatEnabled;
        tmp["source"] = j.dump();
        tmp["contactSource"] = this->toString();
        std::string re = KtOperation::ktOperation(KtOperation::GroupSetting, tmp);
        InternalData->force_refresh_nexttime();
    }

    RemoteFile Group::sendFile(const std::string &path, const std::string &filepath) {
        json tmp;
        json source;
        source["path"] = path;
        source["filepath"] = filepath;
        tmp["source"] = source.dump();
        tmp["contactSource"] = this->toString();
        std::string callback = KtOperation::ktOperation(KtOperation::SendFile, tmp);
        if (callback == "E2") throw UploadException("找不到" + filepath + "位置:C-uploadfile", MIRAICP_EXCEPTION_WHERE);
        if (callback == "E3")
            throw UploadException("Upload error:路径格式异常，应为'/xxx.xxx'或'/xx/xxx.xxx'目前只支持群文件和单层路径, path:" + path, MIRAICP_EXCEPTION_WHERE);
        return RemoteFile::deserializeFromString(callback);
    }

    RemoteFile Group::getFile(const std::string &path, const std::string &id) {
        // source 参数
        if (path.empty() || path == "/")
            return this->getFileById(id);
        json tmp;
        json j;
        tmp["id"] = id;
        tmp["path"] = path;
        j["source"] = tmp.dump();
        j["contactSource"] = this->toString();
        std::string re = KtOperation::ktOperation(KtOperation::RemoteFileInfo, j);
        if (re == "E2") throw RemoteAssetException("Get error: 文件路径不存在, path:" + path + ",id:" + id, MIRAICP_EXCEPTION_WHERE);
        return RemoteFile::deserializeFromString(re);
    }

    RemoteFile Group::getFileById(const std::string &id) {
        json tmp;
        json j;
        tmp["id"] = id;
        j["source"] = tmp.dump();
        j["contactSource"] = this->toString();
        std::string re = KtOperation::ktOperation(KtOperation::RemoteFileInfo, j);
        if (re == "E1") throw RemoteAssetException("Get error: 文件路径不存在, id:" + id, MIRAICP_EXCEPTION_WHERE);
        return RemoteFile::deserializeFromString(re);
    }

    Member Group::getOwner() {
        json j;
        j["contactSource"] = this->toString();
        std::string re = KtOperation::ktOperation(KtOperation::QueryOwner, j);
        return Member(stoi(re), this->id(), this->botid());
    }

    std::string Group::getFileListString(const std::string &path) {
        json temp;
        json j;
        temp["id"] = "-1";
        temp["path"] = path;
        j["source"] = temp.dump();
        j["contactSource"] = this->toString();
        return KtOperation::ktOperation(KtOperation::RemoteFileInfo, j);
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

#undef LOC_CLASS_NAMESPACE
    void GroupData::refreshInfo() {
        std::string re = LowLevelAPI::getInfoSource(this->toString());
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
} // namespace MiraiCP
