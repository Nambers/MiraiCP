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

#include "Friend.h"
#include "Exception.h"
#include "KtOperation.h"
#include "LowLevelAPI.h"

namespace MiraiCP {
    using json = nlohmann::json;
    /*好友类实现*/
    Friend::Friend(QQID id, QQID botid) : Contact(id, botid, MIRAI_FRIEND) {
        // todo
    }
    Friend::Friend(nlohmann::json in_json) :Contact(in_json){
        // todo
    }
    void Friend::deleteFriend() {
        json j;
        j["source"] = this->toString();
        j["quit"] = true;
        KtOperation::ktOperation(KtOperation::RefreshInfo, j);
    }

    void Friend::refreshInfo() {
        std::string temp = LowLevelAPI::getInfoSource(this->toString());
        if (temp == "E1") {
            throw FriendException(MIRAICP_EXCEPTION_WHERE);
        }
        LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
        this->_nickOrNameCard = tmp.nickornamecard;
        this->_avatarUrl = tmp.avatarUrl;
    }

    void Friend::sendNudge() {
        json j;
        j["contactSource"] = this->toString();
        std::string re = KtOperation::ktOperation(KtOperation::SendNudge, j);
        if (re == "E1")
            throw IllegalStateException("发送戳一戳失败，登录协议不为phone/ipad", MIRAICP_EXCEPTION_WHERE);
    }
} // namespace MiraiCP
