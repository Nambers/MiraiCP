#ifndef MIRAICP_PRO_FRIEND_H
#define MIRAICP_PRO_FRIEND_H

#include "LowLevelAPI.h"

namespace MiraiCP {
    /// 好友类声明
    class Friend : public Contact {
    public:
        /*!
         * @brief 构建好友对象
         * @param friendid q号
         * @param botid 对应机器人id
         */
        explicit Friend(QQID friendid, QQID botid, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        explicit Friend(Contact c) : Contact(std::move(c)) { refreshInfo(); };

        /// 删除好友(delete是C++关键字
        void deleteFriend(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            nlohmann::json j;
            j["source"] = this->serializationToString();
            j["quit"] = true;
            Config::koperation(Config::RefreshInfo, j, env);
        }

        void refreshInfo(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) override {
            std::string temp = LowLevelAPI::getInfoSource(this, env);
            if (temp == "E1") {
                throw FriendException();
            }
            LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
            this->_nickOrNameCard = tmp.nickornamecard;
            this->_avatarUrl = tmp.avatarUrl;
        }

        /*!
         * @brief 发送戳一戳
         * @warning 发送戳一戳的前提是登录该bot的协议是phone
         * @throw MiraiCP::BotException, MiraiCP::IllegalStateException
         */
        void sendNudge() {
            json j;
            j["contactSource"] = this->serializationToString();
            std::string re = Config::koperation(Config::SendNudge, j);
            if (re == "E1")
                throw IllegalStateException("发送戳一戳失败，登录协议不为phone");
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_FRIEND_H
