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

#ifndef MIRAICP_PRO_BOT_H
#define MIRAICP_PRO_BOT_H

#include "Contact.h"
#include "SdkType.h"
#include <string>
#include <vector>


namespace MiraiCP {
    class Friend;       // forward declaration
    class Group;        // forward declaration

    /// 当前bot账号信息
    class Bot: public Contact {
    public:
        /**
         * 用 id 构造 Bot 对象
         * @param in_id Bot 的 qq id
         */
        explicit Bot(QQID in_id);

    public:
        /**
         * 取 Bot 下对应 id 的好友
         * @param i 好友的 qq id
         * @return 好友对象
         */
        [[nodiscard]] Friend getFriend(QQID i) const;

        /**
         * 取群聊
         * @param groupid 群聊的 id
         * @return 群聊对象
         */
        [[nodiscard]] Group getGroup(QQID groupid) const;

        /// @brief 昵称
        std::string nick();

        /// @brief 头像下载链接
        std::string avatarUrl();

        /// @brief 取好友列表
        [[nodiscard]] std::vector<QQID> getFriendList() const;

        /// @brief 好友列表string形式返回，利于保存
        [[nodiscard]] std::string FriendListToString() const;

        /// @brief 取群列表
        [[nodiscard]] std::vector<QQID> getGroupList() const;

        /// @brief 群列表string形式返回，利于保存
        [[nodiscard]] std::string GroupListToString() const;

        bool operator==(const Bot &b) const;
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_BOT_H
