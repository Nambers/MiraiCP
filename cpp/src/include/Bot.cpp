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
#include "Bot.h"
#include "Config.h"
#include "Friend.h"
#include "Group.h"
#include "LowLevelAPI.h"
#include "Tools.h"

namespace MiraiCP {
    Group Bot::getGroup(QQID groupid, JNIEnv *env) const {
        return {groupid, this->id, env};
    }

    Friend Bot::getFriend(QQID i, JNIEnv *env) const {
        return Friend(i, this->id, env);
    }

    bool Bot::operator==(const Contact &c) const {
        return this->id == c.id();
    }

    void Bot::refreshInfo(JNIEnv *env) {
        if (this->id == 0)
            return;
        nlohmann::json j;
        j["source"] = Contact(4, 0, 0, "", this->id).toString();
        LowLevelAPI::info tmp = LowLevelAPI::info0(KtOperation::ktOperation(KtOperation::RefreshInfo, j, env));
        this->_avatarUrl = tmp.avatarUrl;
        this->_nick = tmp.nickornamecard;
    }

    std::vector<QQID> Bot::getFriendList(JNIEnv *env) const {
        nlohmann::json j;
        j["botid"] = this->id;
        std::string temp = KtOperation::ktOperation(KtOperation::QueryBFL, j, env);
        return Tools::StringToVector(std::move(temp));
    }

    std::string Bot::FriendListToString() {
        return Tools::VectorToString(getFriendList());
    }

    std::vector<QQID> Bot::getGroupList(JNIEnv *env) const {
        nlohmann::json j;
        j["botid"] = this->id;
        std::string temp = KtOperation::ktOperation(KtOperation::QueryBGL, j, env);
        return Tools::StringToVector(std::move(temp));
    }

    std::string Bot::GroupListToString() const {
        return Tools::VectorToString(getGroupList());
    }
} // namespace MiraiCP
