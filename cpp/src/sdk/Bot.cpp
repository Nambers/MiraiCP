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

#include "Bot.h"
#include "Friend.h"
#include "Group.h"
#include "KtOperation.h"
#include "LowLevelAPI.h"
#include "Tools.h"
#include <atomic>
#include <memory>
#include <mutex>


namespace MiraiCP {
    inline std::shared_ptr<IContactData> get_bot(QQID id) {
        static std::unordered_map<QQID, std::shared_ptr<IContactData>> BotPool;
        static std::mutex mtx;
        std::lock_guard<std::mutex> lck(mtx);
        auto &Ptr = BotPool.try_emplace(id).first->second;
        if (!Ptr){
            Ptr = std::make_shared<IContactData>();
            Ptr->_id = id;
            Ptr->_type = MIRAI_BOT;
            Ptr->_botId = id;
            Ptr->forceRefreshNextTime();
        }
        return Ptr;
    }

    Group Bot::getGroup(QQID groupid) const {
        return {groupid, InternalData->_id};
    }

    Friend Bot::getFriend(QQID i) const {
        return {i, InternalData->_id};
    }

    std::vector<QQID> Bot::getFriendList() const {
        nlohmann::json j{{"contact", toJson()},
                         {"type",    KtOperation::QueryBotListCode::FriendList}};
        std::string temp = KtOperation::ktOperation(KtOperation::QueryBotList, j);
        return Tools::StringToVector(std::move(temp));
    }

    std::string Bot::FriendListToString() const {
        return Tools::VectorToString(getFriendList());
    }

    std::vector<QQID> Bot::getGroupList() const {
        nlohmann::json j{{"contact", toJson()},
                         {"type",    KtOperation::QueryBotListCode::GroupList}};
        std::string temp = KtOperation::ktOperation(KtOperation::QueryBotList, j);
        return Tools::StringToVector(std::move(temp));
    }

    std::string Bot::GroupListToString() const {
        return Tools::VectorToString(getGroupList());
    }

    Bot::Bot(QQID in_id): Contact(get_bot(in_id)) {
    }

    std::string Bot::nick() {
        refreshInfo();
        MIRAICP_DATALOCK;
        return InternalData->_nickOrNameCard;
    }

    std::string Bot::avatarUrl() {
        refreshInfo();
        MIRAICP_DATALOCK;
        return InternalData->_avatarUrl;
    }
} // namespace MiraiCP
