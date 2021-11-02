#ifndef MIRAICP_PRO_BOT_H
#define MIRAICP_PRO_BOT_H

#include "Friend.h"
#include "Group.h"
#include "ThreadManager.h"

namespace MiraiCP {
    /// 当前bot账号信息
    class Bot {
    private:
        bool inited = false;
        std::string _nick;
        std::string _avatarUrl;

        void check() {
            if (!this->inited) {
                refreshInfo();
                this->inited = true;
            }
        }

    public:
        /// 该botid
        QQID id;

        /*!
         * @brief 刷新bot信息
         * @param env
         */
        void refreshInfo(JNIEnv *env = ThreadManager::getEnv()) {
            nlohmann::json j;
            j["source"] = Contact(4, 0, 0, "", this->id).serializationToString();
            LowLevelAPI::info tmp = LowLevelAPI::info0(Config::koperation(Config::RefreshInfo, j, env));
            this->_avatarUrl = tmp.avatarUrl;
            this->_nick = tmp.nickornamecard;
        }

        /// 用id构建机器人
        explicit Bot(QQID i) : id(i) {}

        /// 取好友
        Friend getFriend(QQID i, JNIEnv *env = ThreadManager::getEnv()) const {
            return Friend(i, this->id, env);
        }

        /// 取群聊
        Group getGroup(QQID groupid, JNIEnv *env = ThreadManager::getEnv()) const {
            return Group(groupid, this->id, env);
        }

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
        std::vector<unsigned long long> getFriendList(JNIEnv *env = ThreadManager::getEnv()) {
            nlohmann::json j;
            j["botid"] = this->id;
            std::string temp = Config::koperation(Config::QueryBFL, j, env);
            return Tools::StringToVector(temp);
        }

        /// 好友列表string形式返回，利于保存
        std::string FriendListToString() {
            return Tools::VectorToString(getFriendList());
        }

        /// 取群列表
        std::vector<unsigned long long> getGroupList(JNIEnv *env = ThreadManager::getEnv()) {
            nlohmann::json j;
            j["botid"] = this->id;
            std::string temp = Config::koperation(Config::QueryBGL, j, env);
            return Tools::StringToVector(temp);
        }

        /// 群列表string形式返回，利于保存
        std::string GroupListToString() {
            return Tools::VectorToString(getGroupList());
        }

        bool operator==(const Contact &c) const {
            return this->id == c.id();
        }

        bool operator==(const Bot &b) const {
            return this->id == b.id;
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_BOT_H
