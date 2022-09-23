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
#include "Friend.h"
#include "Group.h"
#include "KtOperation.h"
#include "LowLevelAPI.h"
#include "Tools.h"
#include <atomic>
#include <memory>
#include <mutex>

namespace MiraiCP {


    struct InternalBot : public IMiraiData {
        std::string _avatarUrl;
        std::string _nickOrNameCard;
        QQID _id;
        explicit InternalBot(QQID in_botid) : _id(in_botid) {}
        ~InternalBot() override = default;

        void deserialize(nlohmann::json in_json) override {} // should never be called

        nlohmann::json toJson() const override {
            return {{"botid", _id}, {"type", MIRAI_OTHERTYPE}};
        }

        void refreshInfo() override {
            nlohmann::json j{{"source", toString()}};
            LowLevelAPI::info tmp = LowLevelAPI::info0(KtOperation::ktOperation(KtOperation::RefreshInfo, j));
            _avatarUrl = tmp.avatarUrl;
            _nickOrNameCard = tmp.nickornamecard;
        }
    };

    inline std::unordered_map<QQID, std::shared_ptr<InternalBot>> &getBotPool() {
        static std::unordered_map<QQID, std::shared_ptr<InternalBot>> BotPool;
        return BotPool;
    }


    inline std::shared_ptr<InternalBot> get_bot(QQID id) {
        static std::mutex mtx;
        std::lock_guard<std::mutex> lck(mtx);
        auto &Ptr = getBotPool().try_emplace(id).first->second;
        if (!Ptr) Ptr = std::make_shared<InternalBot>(id);
        return Ptr;
    }

    Group Bot::getGroup(QQID groupid) const {
        return {groupid, this->id};
    }

    Friend Bot::getFriend(QQID i) const {
        return {i, this->id};
    }

    std::vector<QQID> Bot::getFriendList() const {
        nlohmann::json j;
        j["botid"] = this->id;
        std::string temp = KtOperation::ktOperation(KtOperation::QueryBFL, j);
        return Tools::StringToVector(std::move(temp));
    }

    std::string Bot::FriendListToString() const {
        return Tools::VectorToString(getFriendList());
    }

    std::vector<QQID> Bot::getGroupList() const {
        nlohmann::json j;
        j["botid"] = this->id;
        std::string temp = KtOperation::ktOperation(KtOperation::QueryBGL, j);
        return Tools::StringToVector(std::move(temp));
    }

    std::string Bot::GroupListToString() const {
        return Tools::VectorToString(getGroupList());
    }

    void Bot::check() {
        InternalData->request_refresh();
    }

    Bot::Bot(QQID in_id) : InternalData(get_bot(in_id)), id(in_id) {
        InternalData->force_refresh_nexttime();
    }

    std::string Bot::nick() {
        check();
        return InternalData->_nickOrNameCard;
    }

    std::string Bot::avatarUrl() {
        check();
        return InternalData->_avatarUrl;
    }
} // namespace MiraiCP
