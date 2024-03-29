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
#include "ContactDataType/GroupRelatedData.h"
#include "Exception.h"
#include "ExceptionHandle.h"
#include "JsonTools.h"
#include "KtOperation.h"
#include "LowLevelAPI.h"
#include "Tools.h"


namespace MiraiCP {
#define LOC_CLASS_NAMESPACE Member

    struct MemberData : public GroupRelatedData {
        typedef IContactData Super;
        unsigned int _permission = 0;
        std::string _specialTitle;
        /// 是否是匿名群成员, 如果是匿名群成员一些功能会受限
        bool _anonymous = false;

        explicit MemberData(QQID in_groupid) : GroupRelatedData(in_groupid) {}

        void deserialize(nlohmann::json in_json) override;
        void refreshInfo() override;
    };

    using json = nlohmann::json;

    auto GetMemberFromPool(QQID id, QQID groupId, QQID botid) noexcept {
        using Tools::idpair;
        static std::unordered_map<idpair, std::unordered_map<QQID, std::shared_ptr<Member::DataType>>> Pool;
        idpair pr{botid, groupId};
        auto &Val = Pool[pr][id];
        if (!Val) {
            Val = std::make_shared<Member::DataType>(groupId);
            Val->_id = id;
            Val->_botId = botid;
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
        auto ActualDataPtr = GET_DATA_INTERNAL();
        assert(ActualDataPtr != nullptr);
        bool needrefresh = false;
        if (in_json.contains("nickOrNameCard")) ActualDataPtr->_nickOrNameCard = json_stringmover(in_json, "nickOrNameCard");
        else
            needrefresh = true;
        if (in_json.contains("avatarUrl")) ActualDataPtr->_avatarUrl = json_stringmover(in_json, "avatarUrl");
        else
            needrefresh = true;
        if (in_json.contains("anonymous")) ActualDataPtr->_anonymous = in_json["anonymous"].get<bool>();
        else
            needrefresh = true;

        if (needrefresh) forceRefreshNextTime();
    }

    void Member::mute(long long sec) const {
        std::string re = KtOperation::ktOperation(KtOperation::MuteM, json{{"time", sec}, {"contact", toJson()}});
        if (re == "E4")
            throw MuteException(MIRAICP_EXCEPTION_WHERE);
    }

    void Member::kick(std::string reason) {
        json j{{"message", std::move(reason)}, {"contact", toJson()}};
        KtOperation::ktOperation(KtOperation::KickM, j);
        forceRefreshNextTime();
    }

    void Member::modifyAdmin(bool admin) {
        if (anonymous()) return;
        json j{{"admin", admin}, {"contact", toJson()}};
        KtOperation::ktOperation(KtOperation::ModifyAdmin, j);
        forceRefreshNextTime();
    }

    void Member::changeSpecialTitle(std::string_view title) {
        if (anonymous()) return;
        json j{{"title", title}, {"contact", toJson()}};
        KtOperation::ktOperation(KtOperation::ChangeSpecialTitle, j);
        forceRefreshNextTime();
    }

    void Member::changeNameCard(std::string_view newName) {
        if (anonymous()) return;
        json j{{"contact", toJson()}, {"newName", newName}};
        KtOperation::ktOperation(KtOperation::ChangeNameCard, j);
        forceRefreshNextTime();
    }

    void Member::sendNudge() {
        if (anonymous()) return;
        std::string re = KtOperation::ktOperation(KtOperation::SendNudge, toJson());
        if (re == "E1")
            throw IllegalStateException("发送戳一戳失败，登录协议不为phone", MIRAICP_EXCEPTION_WHERE);
    }

    void MemberData::deserialize(nlohmann::json in_json) {
        _groupId = in_json["groupId"];
        _anonymous = in_json["anonymous"].get<bool>();
        IContactData::deserialize(std::move(in_json));
    }

    void MemberData::refreshInfo() {
        if (_anonymous) return;

        std::string result = LowLevelAPI::getInfoSource(internalToJson());

        if (result == "E1")
            throw MemberGroupNotFoundException(MIRAICP_EXCEPTION_WHERE);
        if (result == "E2")
            throw MemberNotFoundException(MIRAICP_EXCEPTION_WHERE);

        {
            LowLevelAPI::info tmp = LowLevelAPI::info0(result);
            this->_nickOrNameCard = tmp.nickOrNameCard;
            this->_avatarUrl = tmp.avatarUrl;
        }

        {
            result = KtOperation::ktOperation(KtOperation::QueryM, internalToJson());
            MIRAICP_ERROR_HANDLE(result, "");
            auto tmp = nlohmann::json::parse(result);
            _permission = tmp["permission"];
            _specialTitle = json_stringmover(tmp, "specialTitle");
        }
    }

    IMPL_GETTER(anonymous)
    IMPL_GETTER(permission)
    IMPL_GETTER(specialTitle)
    NOLOCK_IMPL_GETTER(groupId)
#undef LOC_CLASS_NAMESPACE
} // namespace MiraiCP
