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

namespace MiraiCP {
#define LOC_CLASS_NAMESPACE Member
    using json = nlohmann::json;
    /*成员类实现*/
    Member::Member(QQID id, QQID groupid, QQID botid)
        : Contact(id, botid, MIRAI_MEMBER) {
        // todo
        refreshInfo();
    }
    Member::Member(nlohmann::json in_json) : Contact(in_json) {
        // todo
    }
    IMPL_GETTER(anonymous)
    //
    //    void Member::refreshInfo() {
    //        this->isAnonymous = this->_anonymous;
    //        if (isAnonymous)
    //            return;
    //        std::string temp = LowLevelAPI::getInfoSource(this->toString());
    //        if (temp == "E1")
    //            throw MemberException(1, MIRAICP_EXCEPTION_WHERE);
    //        if (temp == "E2")
    //            throw MemberException(2, MIRAICP_EXCEPTION_WHERE);
    //        LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
    //        this->_nickOrNameCard = tmp.nickornamecard;
    //        this->_avatarUrl = tmp.avatarUrl;
    //        if (isAnonymous) {
    //            this->permission = 0;
    //        } else {
    //            json j;
    //            j["contactSource"] = this->toString();
    //            std::string re = KtOperation::ktOperation(KtOperation::QueryM, j);
    //            this->permission = stoi(re);
    //        }
    //        if (temp == "E1") {
    //            throw MemberException(1, MIRAICP_EXCEPTION_WHERE);
    //        }
    //        if (temp == "E2") {
    //            throw MemberException(2, MIRAICP_EXCEPTION_WHERE);
    //        }
    //    }
    //
    //    unsigned int Member::getPermission() const {
    //        if (isAnonymous) return 0;
    //    }

    void Member::mute(int time) {
        json j;
        j["time"] = time;
        j["contactSource"] = this->toString();
        std::string re = KtOperation::ktOperation(KtOperation::MuteM, j);
        if (re == "E4")
            throw MuteException(MIRAICP_EXCEPTION_WHERE);
    }

    void Member::kick(const std::string &reason) {
        json j;
        j["message"] = reason;
        j["contactSource"] = this->toString();
        KtOperation::ktOperation(KtOperation::KickM, j);
    }

    void Member::modifyAdmin(bool admin) {
        if (anonymous()) return;
        json j;
        j["admin"] = admin;
        j["contactSource"] = this->toString();
        KtOperation::ktOperation(KtOperation::ModifyAdmin, j);
        refreshInfo();
    }

    void Member::changeNameCard(std::string_view newName) {
        if (anonymous()) return;
        json j;
        j["contactSource"] = this->toString();
        j["newName"] = newName;
        KtOperation::ktOperation(KtOperation::ChangeNameCard, j);
        refreshInfo();
    }

    void Member::sendNudge() {
        if (anonymous()) return;
        json j;
        j["contactSource"] = this->toString();
        std::string re = KtOperation::ktOperation(KtOperation::SendNudge, j);
        if (re == "E1")
            throw IllegalStateException("发送戳一戳失败，登录协议不为phone", MIRAICP_EXCEPTION_WHERE);
    }
    void Member::refreshInfo() {
        InternalData->request_refresh();
    }


    void MemberData::deserialize(nlohmann::json in_json) {
        _groupid = in_json["groupid"];
        _anonymous = in_json["anonymous"].get<bool>();
        IContactData::deserialize(in_json);
    }

    void MemberData::refreshInfo() {
        if (_anonymous) return;

        auto tempserialize = toString();
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

#undef LOC_CLASS_NAMESPACE
} // namespace MiraiCP
