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
#include "KtOperation.h"
#include "Exception.h"
#include "LowLevelAPI.h"

namespace MiraiCP {
    using json = nlohmann::json;
    /*成员类实现*/
    Member::Member(QQID id, QQID groupid, QQID botid, JNIEnv *env)
        : Contact() {
        this->_type = MIRAI_MEMBER;
        this->_id = id;
        this->_groupid = groupid;
        this->_botid = botid;
        refreshInfo(env);
    }

    void Member::refreshInfo(JNIEnv *env) {
        if (isAnonymous)
            return;
        std::string temp = LowLevelAPI::getInfoSource(this->toString(), env);
        if (temp == "E1")
            throw MemberException(1, MIRAICP_EXCEPTION_WHERE);
        if (temp == "E2")
            throw MemberException(2, MIRAICP_EXCEPTION_WHERE);
        LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
        this->_nickOrNameCard = tmp.nickornamecard;
        this->_avatarUrl = tmp.avatarUrl;
        this->permission = getPermission();
        if (temp == "E1") {
            throw MemberException(1, MIRAICP_EXCEPTION_WHERE);
        }
        if (temp == "E2") {
            throw MemberException(2, MIRAICP_EXCEPTION_WHERE);
        }
    }

    unsigned int Member::getPermission(JNIEnv *env) const {
        if (isAnonymous) return 0;
        json j;
        j["contactSource"] = this->toString();
        std::string re = KtOperation::ktOperation(KtOperation::QueryM, j, env);
        return stoi(re);
    }

    void Member::mute(int time, JNIEnv *env) {
        json j;
        j["time"] = time;
        j["contactSource"] = this->toString();
        std::string re = KtOperation::ktOperation(KtOperation::MuteM, j, env);
        if (re == "E4")
            throw MuteException(MIRAICP_EXCEPTION_WHERE);
    }

    void Member::kick(const std::string &reason, JNIEnv *env) {
        json j;
        j["message"] = reason;
        j["contactSource"] = this->toString();
        KtOperation::ktOperation(KtOperation::KickM, j, env);
    }

    void Member::modifyAdmin(bool admin, JNIEnv *env) {
        if (isAnonymous) return;
        json j;
        j["admin"] = admin;
        j["contactSource"] = this->toString();
        KtOperation::ktOperation(KtOperation::ModifyAdmin, j, env);
        refreshInfo(env);
    }

    void Member::changeNameCard(std::string_view newName, JNIEnv* env) {
        if (isAnonymous) return;
        json j;
        j["contactSource"] = this->toString();
        j["newName"] = newName;
        KtOperation::ktOperation(KtOperation::ChangeNameCard, j, env);
        refreshInfo(env);
    }

    void Member::sendNudge() {
        if (isAnonymous) return;
        json j;
        j["contactSource"] = this->toString();
        std::string re = KtOperation::ktOperation(KtOperation::SendNudge, j);
        if (re == "E1")
            throw IllegalStateException("发送戳一戳失败，登录协议不为phone", MIRAICP_EXCEPTION_WHERE);
    }

} // namespace MiraiCP
