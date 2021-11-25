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

#include "Friend.h"

namespace MiraiCP {
    using json = nlohmann::json;
    /*好友类实现*/
    Friend::Friend(QQID id, QQID botid, JNIEnv *env) : Contact() {
        this->_type = 1;
        this->_id = id;
        this->_botid = botid;
        refreshInfo(env);
    }

    void Friend::deleteFriend(JNIEnv *env) {
        json j;
        j["source"] = this->serializationToString();
        j["quit"] = true;
        Config::koperation(Config::RefreshInfo, j, env);
    }

    void Friend::refreshInfo(JNIEnv *env) {
        std::string temp = LowLevelAPI::getInfoSource(this->serializationToString(), env);
        if (temp == "E1") {
            MiraiCPThrow(FriendException());
        }
        LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
        this->_nickOrNameCard = tmp.nickornamecard;
        this->_avatarUrl = tmp.avatarUrl;
    }

    void Friend::sendNudge() {
        json j;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::SendNudge, j);
        if (re == "E1")
            MiraiCPThrow(IllegalStateException("发送戳一戳失败，登录协议不为phone"));
    }
} // namespace MiraiCP
