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
#include "Tools.h"


namespace MiraiCP {
    using json = nlohmann::json;
    auto GetFriendPool(QQID id, QQID botid) noexcept {
        static std::unordered_map<QQID, std::unordered_map<QQID, std::shared_ptr<Friend::DataType>>> Pool;
        auto &val = Pool[botid][id];
        if (!val) {
            val = std::make_shared<Friend::DataType>();
            val->_id = id;
            val->_botId = botid;
            val->_type = MIRAI_FRIEND;
        }
        return val;
    }

    auto GetFriendPool(const json &in_json) {
        try {
            return GetFriendPool(in_json["id"], in_json["id2"]);
        } catch (const nlohmann::detail::exception &) {
            throw IllegalArgumentException("构造Friend时传入的json异常", MIRAICP_EXCEPTION_WHERE);
        }
    }

    /*好友类实现*/
    Friend::Friend(QQID id, QQID botid) : Contact(GetFriendPool(id, botid)) {
        forceRefreshNextTime(); //InternalData->requestRefresh();
    }

    Friend::Friend(nlohmann::json in_json) : Contact(GetFriendPool(in_json)) {
        auto ActualDataPtr = GetDataInternal();
        assert(ActualDataPtr != nullptr);

        bool needRefresh = false;

        if (in_json.contains("avatarUrl")) ActualDataPtr->_nickOrNameCard = Tools::json_stringmover(in_json, "nickOrNameCard");
        else
            needRefresh = true;
        if (in_json.contains("avatarUrl")) ActualDataPtr->_avatarUrl = Tools::json_stringmover(in_json, "avatarUrl");
        else
            needRefresh = true;

        if (needRefresh) forceRefreshNextTime();
    }

    void Friend::deleteFriend() {
        json j{{"contact", this->toJson()}, {"quit", true}};
        MIRAICP_ERROR_HANDLE(KtOperation::ktOperation(KtOperation::RefreshInfo, j), "");
    }

    //    void Friend::refreshInfo() {
    //        InternalData->requestRefresh();
    //        //        std::string temp = LowLevelAPI::getInfoSource(this->toString());
    //        //        if (temp == "E1") {
    //        //            throw FriendException(MIRAICP_EXCEPTION_WHERE);
    //        //        }
    //        //        LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
    //        //        this->_nickOrNameCard = tmp.nickOrNameCard;
    //        //        this->_avatarUrl = tmp.avatarUrl;
    //    }

    void Friend::sendNudge() {
        std::string re = KtOperation::ktOperation(KtOperation::SendNudge, toJson());
        if (re == "E1")
            throw IllegalStateException("发送戳一戳失败，登录协议不为phone/ipad", MIRAICP_EXCEPTION_WHERE);
    }
} // namespace MiraiCP
