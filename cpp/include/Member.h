#ifndef MIRAICP_PRO_MEMBER_H
#define MIRAICP_PRO_MEMBER_H

#include "LowLevelAPI.h"

namespace MiraiCP {
    /// 群成员类声明
    class Member : public Contact {
    public:
        /// @brief 权限等级
        ///     - OWNER群主 为 2
        ///     - ADMINISTRATOR管理员 为 1
        ///     - MEMBER群成员 为 0
        /// @note 上面那些变量在constants.h中有定义
        unsigned int permission = 0;

        /// @brief 更改群成员权限
        /// @param admin 如果为true为更改到管理员
        /// @param env
        void modifyAdmin(bool admin, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__));

        /// @brief 构建群成员对象
        /// @param qqid 该成员q号
        /// @param groupid 所在群号
        /// @param botid 机器人id
        /// @example 在事件中构建Member对象
        /// @code
        ///  Member(this.sender.id, this.group.id, this.bot.id)
        /// @endcode
        explicit Member(QQID qqid, QQID groupid, QQID botid,
                        JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        explicit Member(Contact c) : Contact(std::move(c)) {
            this->isAnonymous = this->_anonymous;
            refreshInfo();
        };

        /// 是否是匿名群成员, 如果是匿名群成员一些功能会受限
        bool isAnonymous = false;

        /// 重新获取(刷新)群成员信息
        void refreshInfo(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) override {
            if (isAnonymous)
                return;
            std::string temp = LowLevelAPI::getInfoSource(this, env);
            if (temp == "E1")
                throw MemberException(1);
            if (temp == "E2")
                throw MemberException(2);
            LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
            this->_nickOrNameCard = tmp.nickornamecard;
            this->_avatarUrl = tmp.avatarUrl;
            this->permission = getPermission();
            if (temp == "E1") {
                throw MemberException(1);
            }
            if (temp == "E2") {
                throw MemberException(2);
            }
        }

        /// 发送语音
        MessageSource sendVoice(const std::string &path, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            return Contact::sendVoice0(path, env);
        }

        /// 获取权限，会在构造时调用，请使用permission缓存变量
        /// @see Member::permission
        unsigned int getPermission(JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /*!
         * 禁言当前对象，单位是秒，最少0秒最大30天，如果为0或者为负则unmute
         * @throws BotException, MuteException
        */
        void mute(int time, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /// 取消禁言
        /// @throws BotException, MuteException
        void unMute(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            mute(0, env);
        }

        /*! 踢出这个群成员
        * @param reason - 原因
         * @example 踢出群成员
         * @code
         * Event::processor.registerEvent([](GroupMessageEvent e) {
            try {
                Member m = Member(qqid, e.group.id(), e.bot.id());
                m.Kick("this_is_reason");
            }
            catch (BotException err) {
                //权限不足
                Logger::logger.error(err.what());
            }
            catch (MemberException err) {
                if (err.type == 1) {
                    //找不到群
                }
                if (err.type == 2) {
                    //找不到群成员
                }
            }
            });
         * @endcode
        */
        void kick(const std::string &reason, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /// At一个群成员
        At at() {
            /*返回at这个人的miraicode*/
            return At(this->id());
        }

        /*!
         * @brief 发送戳一戳
         * @warning 发送戳一戳的前提是登录该bot的协议是phone
         * @throw MiraiCP::BotException, MiraiCP::IllegalStateException
         */
        void sendNudge() {
            if (isAnonymous) return;
            json j;
            j["contactSource"] = this->serializationToString();
            std::string re = Config::koperation(Config::SendNudge, j);
            if (re == "E1")
                throw IllegalStateException("发送戳一戳失败，登录协议不为phone");
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MEMBER_H