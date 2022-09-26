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
#include "commonTypes.h"
#include <string>
#include <vector>


namespace MiraiCP {
    class Friend;       // forward declaration
    class Group;        // forward declaration
    struct InternalBot; // forward declaration

    /// 当前bot账号信息
    class Bot {
        std::shared_ptr<InternalBot> InternalData;

    public:
        /// 用id构造Bot对象
        explicit Bot(QQID in_id);

    public:
        /// 取好友
        Friend getFriend(QQID i) const;

        /// 取群聊
        Group getGroup(QQID groupid) const;

        /// @brief 昵称
        /// @note 并发情形下并不保证能得到正确结果
        std::string nick();

        /// @brief 头像下载链接
        std::string avatarUrl();

        /// @brief 取好友列表
        std::vector<QQID> getFriendList() const;

        /// @brief 好友列表string形式返回，利于保存
        std::string FriendListToString() const;

        /// @brief 取群列表
        std::vector<QQID> getGroupList() const;

        /// @brief 群列表string形式返回，利于保存
        std::string GroupListToString() const;

        bool operator==(const Bot &b) const;

    private:
        /*!
         * @brief 刷新bot信息
         * @param env
         */
        void refreshInfo();
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_BOT_H
