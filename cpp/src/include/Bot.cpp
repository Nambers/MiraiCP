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
#include <mutex>

namespace MiraiCP {
    template<class implClass>
    class IContactInternalImpl {
    private:
        std::atomic_bool inited = false;
        std::mutex mtx;
    public:
        using poolType = std::unordered_map<MiraiCP::QQID, std::shared_ptr<implClass>>;

        IContactInternalImpl() = default;

        virtual ~IContactInternalImpl() = default;

        virtual void refreshInfo(MiraiCP::Contact *) = 0;

        // for auto cast
        void request_refresh(Contact *c) {
            if (!MiraiCP::LowLevelAPI::checkSafeCall()) return;
            if (!inited.exchange(true)) {
                std::lock_guard<std::mutex> lck(mtx);
                refreshInfo(c);
            }
        }

        void force_refresh() {
            inited = false;
        }
    };

    class InternalBot : public IContactInternalImpl<InternalBot> {
    public:
        void refreshInfo(Contact *c) override {
            auto bot = dynamic_cast<Bot *>(c);
            nlohmann::json j;
            j["source"] = Contact(4, 0, 0, "", c->id()).toString();
            LowLevelAPI::info tmp = LowLevelAPI::info0(KtOperation::ktOperation(KtOperation::RefreshInfo, j));
            bot->_avatarUrl = tmp.avatarUrl;
            bot->_nickOrNameCard = tmp.nickornamecard;
        }

        ~InternalBot() override = default;
    };

    static InternalBot::poolType BotPool;

    inline std::shared_ptr<InternalBot> get_bot(QQID id) {
        static std::mutex mtx;
        std::lock_guard<std::mutex> lck(mtx);
        auto &Ptr = BotPool.try_emplace(id).first->second;
        if (!Ptr) Ptr.reset(new InternalBot);
        return Ptr;
    }

    Group Bot::getGroup(QQID groupid) const {
        return {groupid, this->id};
    }

    Friend Bot::getFriend(QQID i) const {
        return Friend(i, this->id);
    }

    //    void Bot::refreshInfo() {
    //        _InternalBot->request_refresh(id);
    //    }

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
        _InternalBot->request_refresh(this);
    }

    Bot::Bot(QQID in_id) : Contact(4, 0, 0, "", in_id), _InternalBot(get_bot(in_id)), id(in_id) {
        _InternalBot->force_refresh();
    }

    std::string Bot::nick() {
        check();
        return _nickOrNameCard;
    }

    std::string Bot::avatarUrl() {
        check();
        return _avatarUrl;
    }
} // namespace MiraiCP
