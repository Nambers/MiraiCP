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

#include <jni.h>
#include <string>
#include <vector>

namespace MiraiCP {
    class Friend;  // forward declaration
    class Group;   // forward declaration
    class Contact; // forward declaration
    using QQID = unsigned long long;

    /// 当前bot账号信息
    class Bot {
    private:
        bool inited = false;
        std::string _nick;
        std::string _avatarUrl;

    public:
        /// 该botid
        QQID id;

    private:
        void check() {
            if (!this->inited) {
                refreshInfo();
                this->inited = true;
            }
        }

    public:
        /*!
         * @brief 刷新bot信息
         * @param env
         */
        void refreshInfo(JNIEnv *env = nullptr);

        /// 用id构建机器人
        explicit Bot(QQID i) : id(i) {}

        /// 取好友
        Friend getFriend(QQID i, JNIEnv *env = nullptr) const;

        /// 取群聊
        Group getGroup(QQID groupid, JNIEnv *env = nullptr) const;

        /// 昵称
        std::string nick() {
            check();
            return this->_nick;
        }

        /// 头像下载链接
        std::string avatarUrl() {
            check();
            return this->_avatarUrl;
        }

        /// 取好友列表
        std::vector<unsigned long long> getFriendList(JNIEnv *env = nullptr) const;

        /// 好友列表string形式返回，利于保存
        std::string FriendListToString();

        /// 取群列表
        std::vector<unsigned long long> getGroupList(JNIEnv *env = nullptr) const;

        /// 群列表string形式返回，利于保存
        std::string GroupListToString() const;

        bool operator==(const Contact &c) const;

        bool operator==(const Bot &b) const {
            return this->id == b.id;
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_BOT_H
