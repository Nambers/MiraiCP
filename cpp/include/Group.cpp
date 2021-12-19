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

#include "Group.h"
#include "LowLevelAPI.h"
#include "Tools.h"
#include "Exception.h"
#include "Config.h"

namespace MiraiCP {
    using json = nlohmann::json;
    std::string Group::MemberListToString() {
        return Tools::VectorToString(getMemberList());
    }
    std::vector<Group::OnlineAnnouncement> Group::getAnnouncementsList(JNIEnv *env) {
        json j;
        j["source"] = this->toString();
        j["announcement"] = true;
        std::string re = Config::koperation(Config::RefreshInfo, j, env);
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
        std::string re = Config::koperation(Config::Announcement, j);
        if (re == "E1")
            MiraiCPThrow(IllegalArgumentException("无法根据fid找到群公告(群公告不存在)"));
        if (re == "E2")
            MiraiCPThrow(BotException());
        if (re == "E3")
            MiraiCPThrow(IllegalStateException("群公告状态异常"));
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
        std::string re = Config::koperation(Config::Announcement, j);
        if (re == "E1")
            MiraiCPThrow(BotException());
        return Group::OnlineAnnouncement::deserializeFromJson(json::parse(re));
    }

    Group::OnlineAnnouncement Group::OnlineAnnouncement::deserializeFromJson(json j) {
        Group::AnnouncementParams ap(
                j["params"]["sendToNewMember"],
                j["params"]["requireConfirmation"],
                j["params"]["isPinned"],
                j["params"]["showEditCard"],
                j["params"]["showPopup"]);
        return Group::OnlineAnnouncement(
                j["content"],
                ap,
                j["groupid"],
                j["senderid"],
                j["botid"],
                j["time"],
                j["fid"],
                j["confirmationNum"],
                j["imageid"]);
    }
    std::vector<unsigned long long> Group::getMemberList(JNIEnv *env) {
        nlohmann::json j;
        j["contactSource"] = this->toString();
        std::string re = Config::koperation(Config::QueryML, j, env);
        if (re == "E1") {
            MiraiCPThrow(GroupException());
        }
        return Tools::StringToVector(re);
    }
    Group::Group(QQID groupid, QQID botid, JNIEnv *env) : Contact() {
        this->_type = 2;
        this->_id = groupid;
        this->_botid = botid;
        refreshInfo(env);
    }
    void Group::quit(JNIEnv *env) {
        nlohmann::json j;
        j["source"] = this->toString();
        j["quit"] = true;
        Config::koperation(Config::RefreshInfo, j, env);
    }
    void Group::refreshInfo(JNIEnv *env) {
        std::string re = LowLevelAPI::getInfoSource(this->toString(), env);
        LowLevelAPI::info tmp = LowLevelAPI::info0(re);
        this->_nickOrNameCard = tmp.nickornamecard;
        this->_avatarUrl = tmp.avatarUrl;
        nlohmann::json j = nlohmann::json::parse(re)["setting"];
        this->setting.name = j["name"];
        this->setting.isMuteAll = j["isMuteAll"];
        this->setting.isAllowMemberInvite = j["isAllowMemberInvite"];
        this->setting.isAutoApproveEnabled = j["isAutoApproveEnabled"];
        this->setting.isAnonymousChatEnabled = j["isAnonymousChatEnabled"];
    }

    void Group::updateSetting(JNIEnv *env) {
        json j;
        json tmp;
        j["name"] = this->setting.name;
        j["isMuteAll"] = this->setting.isMuteAll;
        j["isAllowMemberInvite"] = this->setting.isAllowMemberInvite;
        j["isAutoApproveEnabled"] = this->setting.isAutoApproveEnabled;
        j["isAnonymousChatEnabled"] = this->setting.isAnonymousChatEnabled;
        tmp["source"] = j.dump();
        tmp["contactSource"] = this->toString();
        std::string re = Config::koperation(Config::GroupSetting, tmp, env);
        if (re == "E1")
            MiraiCPThrow(BotException());
        refreshInfo(env);
    }

    RemoteFile Group::sendFile(const std::string &path, const std::string &filepath, JNIEnv *env) {
        json tmp;
        json source;
        source["path"] = path;
        source["filepath"] = filepath;
        tmp["source"] = source.dump();
        tmp["contactSource"] = this->toString();
        std::string callback = Config::koperation(Config::SendFile, tmp, env);
        if (callback == "E2") MiraiCPThrow(UploadException("找不到" + filepath + "位置:C-uploadfile"));
        if (callback == "E3")
            MiraiCPThrow(UploadException("Upload error:路径格式异常，应为'/xxx.xxx'或'/xx/xxx.xxx'目前只支持群文件和单层路径, path:" + path));
        return RemoteFile::deserializeFromString(callback);
    }

    RemoteFile Group::getFile(const std::string &path, const std::string &id, JNIEnv *env) {
        // source 参数
        if (path.empty() || path == "/")
            return this->getFileById(id, env);
        json tmp;
        json j;
        tmp["id"] = id;
        tmp["path"] = path;
        j["source"] = tmp.dump();
        j["contactSource"] = this->toString();
        std::string re = Config::koperation(Config::RemoteFileInfo, j, env);
        if (re == "E2") MiraiCPThrow(RemoteAssetException("Get error: 文件路径不存在, path:" + path + ",id:" + id));
        return RemoteFile::deserializeFromString(re);
    }

    RemoteFile Group::getFileById(const std::string &id, JNIEnv *env) {
        json tmp;
        json j;
        tmp["id"] = id;
        j["source"] = tmp.dump();
        j["contactSource"] = this->toString();
        std::string re = Config::koperation(Config::RemoteFileInfo, j, env);
        if (re == "E1") MiraiCPThrow(RemoteAssetException("Get error: 文件路径不存在, id:" + id));
        return RemoteFile::deserializeFromString(re);
    }

    Member Group::getOwner(JNIEnv *env) {
        json j;
        j["contactSource"] = this->toString();
        std::string re = Config::koperation(Config::QueryOwner, j, env);
        return Member(stoi(re), this->id(), this->botid());
    }

    std::string Group::getFileListString(const std::string &path, JNIEnv *env) {
        json temp;
        json j;
        temp["id"] = "-1";
        temp["path"] = path;
        j["source"] = temp.dump();
        j["contactSource"] = this->toString();
        return Config::koperation(Config::RemoteFileInfo, j, env);
    }

    std::vector<Group::file_short_info> Group::getFileList(const std::string &path, JNIEnv *env) {
        std::vector<file_short_info> re = std::vector<file_short_info>();
        std::string tmp = getFileListString(path, env);
        json root = json::parse(tmp);
        for (auto &i: root) {
            file_short_info t;
            t.path = i[0];
            t.id = i[1];
            re.push_back(t);
        }
        return re;
    }
} // namespace MiraiCP
