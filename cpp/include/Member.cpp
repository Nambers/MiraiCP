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

#include "Member.h"

namespace MiraiCP {
    using json = nlohmann::json;
    /*成员类实现*/
    Member::Member(QQID id, QQID groupid, QQID botid, JNIEnv *env)
        : Contact() {
        this->_type = 3;
        this->_id = id;
        this->_groupid = groupid;
        this->_botid = botid;
        refreshInfo(env);
    }

    void Member::refreshInfo(JNIEnv *env) {
        if (isAnonymous)
            return;
        std::string temp = LowLevelAPI::getInfoSource(this->serializationToString(), env);
        if (temp == "E1")
            MiraiCPThrow(MemberException(1));
        if (temp == "E2")
            MiraiCPThrow(MemberException(2));
        LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
        this->_nickOrNameCard = tmp.nickornamecard;
        this->_avatarUrl = tmp.avatarUrl;
        this->permission = getPermission();
        if (temp == "E1") {
            MiraiCPThrow(MemberException(1));
        }
        if (temp == "E2") {
            MiraiCPThrow(MemberException(2));
        }
    }

    unsigned int Member::getPermission(JNIEnv *env) const {
        if (isAnonymous) return 0;
        json j;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::QueryM, j, env);
        return stoi(re);
    }

    void Member::mute(int time, JNIEnv *env) {
        json j;
        j["time"] = time;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::MuteM, j, env);
        if (re == "E3") {
            MiraiCPThrow(BotException());
        }
        if (re == "E4") {
            MiraiCPThrow(MuteException());
        }
    }

    void Member::kick(const std::string &reason, JNIEnv *env) {
        json j;
        j["message"] = reason;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::KickM, j, env);
        if (re == "E3") {
            MiraiCPThrow(BotException());
        }
    }

    void Member::modifyAdmin(bool admin, JNIEnv *env) {
        if (isAnonymous) return;
        json j;
        j["admin"] = admin;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::ModifyAdmin, j, env);
        if (re == "E1") {
            MiraiCPThrow(BotException());
        }
    }

    void Member::sendNudge() {
        if (isAnonymous) return;
        json j;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::SendNudge, j);
        if (re == "E1")
            MiraiCPThrow(IllegalStateException("发送戳一戳失败，登录协议不为phone"));
    }

} // namespace MiraiCP
