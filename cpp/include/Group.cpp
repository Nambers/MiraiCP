#include "Group.h"

namespace MiraiCP {
    /*群聊类实现*/
    Group::Group(QQID groupid, QQID botid, JNIEnv *env) : Contact() {
        this->_type = 2;
        this->_id = groupid;
        this->_botid = botid;
        refreshInfo(env);
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

    void Group::updateSetting(JNIEnv *env) {
        json j;
        json tmp;
        j["name"] = this->setting.name;
        j["isMuteAll"] = this->setting.isMuteAll;
        j["isAllowMemberInvite"] = this->setting.isAllowMemberInvite;
        j["isAutoApproveEnabled"] = this->setting.isAutoApproveEnabled;
        j["isAnonymousChatEnabled"] = this->setting.isAnonymousChatEnabled;
        tmp["source"] = j.dump();
        tmp["contactSource"] = this->serializationToString();
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
        tmp["contactSource"] = this->serializationToString();
        std::string callback = Config::koperation(Config::SendFile, tmp, env);
        if (callback == "E2") MiraiCPThrow(UploadException("找不到" + filepath + "位置:C-uploadfile"));
        if (callback == "E3")
            MiraiCPThrow(UploadException("Upload error:路径格式异常，应为'/xxx.xxx'或'/xx/xxx.xxx'目前只支持群文件和单层路径, path:" + path));
        return RemoteFile::deserializeFromString(callback);
    }

    RemoteFile Group::getFile(const std::string &path, const std::string &id, JNIEnv *env) {
        // source 参数
        json tmp;
        json j;
        tmp["id"] = id;
        tmp["path"] = path;
        j["source"] = tmp.dump();
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::RemoteFileInfo, j, env);
        if (re == "E2") MiraiCPThrow(RemoteAssetException("Get error: 文件路径不存在, path:" + path + ",id:" + id));
        return RemoteFile::deserializeFromString(re);
    }

    RemoteFile Group::getFileById(const std::string &id, JNIEnv *env) {
        json tmp;
        json j;
        tmp["id"] = id;
        j["source"] = tmp.dump();
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::RemoteFileInfo, j, env);
        if (re == "E1") MiraiCPThrow(RemoteAssetException("Get error: 文件路径不存在, id:" + id));
        return RemoteFile::deserializeFromString(re);
    }

    Member Group::getOwner(JNIEnv *env) {
        json j;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::QueryOwner, j, env);
        return Member(stoi(re), this->id(), this->botid());
    }

    std::string Group::getFileListString(const std::string &path, JNIEnv *env) {
        json temp;
        json j;
        temp["id"] = "-1";
        temp["path"] = path;
        j["source"] = temp.dump();
        j["contactSource"] = this->serializationToString();
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
