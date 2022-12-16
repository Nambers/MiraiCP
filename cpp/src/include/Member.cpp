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

#include "Member.h"
#include "Exception.h"
#include "KtOperation.h"
#include "LowLevelAPI.h"
#include "Tools.h"


namespace MiraiCP {
#define LOC_CLASS_NAMESPACE Member
    using json = nlohmann::json;


    auto GetMemberFromPool(QQID id, QQID groupid, QQID botid) noexcept {
        using Tools::idpair;
        static std::unordered_map<idpair, std::unordered_map<QQID, std::shared_ptr<Member::DataType>>> Pool;
        idpair pr{botid, groupid};
        auto &Val = Pool[pr][id];
        if (!Val) {
            Val = std::make_shared<Member::DataType>(groupid);
            Val->_id = id;
            Val->_botid = botid;
            Val->_type = MIRAI_MEMBER;
        }
        return Val;
    }

    auto GetMemberFromPool(const json &in_json) {
        try {
            return GetMemberFromPool(in_json["id"], in_json["groupId"], in_json["botId"]);
        } catch (const nlohmann::detail::exception &) {
            throw IllegalArgumentException("构造Member时传入的json异常", MIRAICP_EXCEPTION_WHERE);
        }
    }

    Member::Member(QQID id, QQID groupid, QQID botid)
        : Contact(GetMemberFromPool(id, groupid, botid)) {
        forceRefreshNextTime();
    }

    Member::Member(nlohmann::json in_json) : Contact(GetMemberFromPool(in_json)) {
        auto ActualDataPtr = GetDataInternal();
        assert(ActualDataPtr != nullptr);
        bool needrefresh = false;
        if (in_json.contains("nickornamecard")) ActualDataPtr->_nickOrNameCard = Tools::json_stringmover(in_json, "nickornamecard");
        else
            needrefresh = true;
        if (in_json.contains("avatarUrl")) ActualDataPtr->_avatarUrl = Tools::json_stringmover(in_json, "avatarUrl");
        else
            needrefresh = true;
        if (in_json.contains("anonymous")) ActualDataPtr->_anonymous = in_json["anonymous"].get<bool>();
        else
            needrefresh = true;

        if (needrefresh) forceRefreshNextTime();
    }

    void Member::mute(long long sec) const {
        json j{{"time", sec}, {"contactSource", toString()}};
        std::string re = KtOperation::ktOperation(KtOperation::MuteM, std::move(j));
        if (re == "E4")
            throw MuteException(MIRAICP_EXCEPTION_WHERE);
    }

    void Member::kick(std::string reason) {
        json j{{"message", std::move(reason)}, {"contactSource", toString()}};
        KtOperation::ktOperation(KtOperation::KickM, std::move(j));
        forceRefreshNextTime();
    }

    void Member::modifyAdmin(bool admin) {
        if (anonymous()) return;
        json j{{"admin", admin}, {"contactSource", toString()}};
        KtOperation::ktOperation(KtOperation::ModifyAdmin, std::move(j));
        forceRefreshNextTime();
    }

    void Member::changeNameCard(std::string_view newName) {
        if (anonymous()) return;
        json j{{"contactSource", toString()}, {"newName", newName}};
        KtOperation::ktOperation(KtOperation::ChangeNameCard, std::move(j));
        forceRefreshNextTime();
    }

    void Member::sendNudge() {
        if (anonymous()) return;
        json j{{"contactSource", toString()}};
        std::string re = KtOperation::ktOperation(KtOperation::SendNudge, std::move(j));
        if (re == "E1")
            throw IllegalStateException("发送戳一戳失败，登录协议不为phone", MIRAICP_EXCEPTION_WHERE);
    }

    void MemberData::deserialize(nlohmann::json in_json) {
        _groupid = in_json["groupid"];
        _anonymous = in_json["anonymous"].get<bool>();
        IContactData::deserialize(std::move(in_json));
    }

    void MemberData::refreshInfo() {
        if (_anonymous) return;

        auto tempserialize = internalToString();
        std::string result = LowLevelAPI::getInfoSource(tempserialize);

        if (result == "E1")
            throw MemberException(1, MIRAICP_EXCEPTION_WHERE);
        if (result == "E2")
            throw MemberException(2, MIRAICP_EXCEPTION_WHERE);

        {
            LowLevelAPI::info tmp = LowLevelAPI::info0(result);
            this->_nickOrNameCard = tmp.nickornamecard;
            this->_avatarUrl = tmp.avatarUrl;
        }

        {
            json j{{"contactSource", std::move(tempserialize)}};
            _permission = stoi(KtOperation::ktOperation(KtOperation::QueryM, std::move(j)));
        }
    }

    IMPL_GETTER(anonymous)
    IMPL_GETTER(permission)

#undef LOC_CLASS_NAMESPACE
} // namespace MiraiCP
