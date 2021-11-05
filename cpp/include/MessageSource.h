// Copyright (c) 2021-2021. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_MESSAGESOURCE_H
#define MIRAICP_PRO_MESSAGESOURCE_H

#include "MiraiCode.h"

namespace MiraiCP {
    using QQID = unsigned long long;
    /// 消息源声明
    class MessageSource {
    public:
        /// 消息的ids
        std::string ids;
        /// 消息的internalids
        std::string internalids;
        /// 消息源序列化
        std::string source;

        MessageSource() = default;

        /**
         * @brief 回复(引用并发送miraicode)
         * @param msg - MiraiCodeable类型指针 - 内容
         * @see MessageSource::quoteAndSendMiraiCode
        */
        [[deprecated("Use Contact.quoteAndSendMessage")]] MessageSource
        quoteAndSendMiraiCode(MiraiCodeable *msg, QQID groupid = 0,
                              JNIEnv *env = ThreadManager::getEnv()) const {
            return quoteAndSendMiraiCode(msg->toMiraiCode(), groupid, env);
        }

        /**
         * @brief 回复(引用并发送)
         * @param c 引用后发送的内容, 为纯文本
         * @param groupid 如果为发送给群成员需把该群成员的groupid传入以帮助获取到该成员
         * @return MessageSource
         * @example 回复(引用并发送)
         * @code
         *  e.messageSource.quoteAndSendMsg("HI");
         * @endcode
         */
        [[deprecated("Use Contact.quoteAndSendMessage")]] MessageSource
        quoteAndSendMsg(const std::string &c, QQID groupid = 0,
                        JNIEnv * = ThreadManager::getEnv()) const;

        /**
         * @brief 回复(引用并发送)
         * @param c 引用后发送的内容, 为MiraiCode形式
         * @param groupid 如果为发送给群成员需把该群成员的groupid传入以帮助获取到该成员
         * @return MessageSource
         */
        [[deprecated("Use Contact.quoteAndSendMessage")]] MessageSource
        quoteAndSendMiraiCode(const std::string &c, QQID groupid = 0,
                              JNIEnv * = ThreadManager::getEnv()) const;

        /*!
         * @brief 构建消息源
         * @param ids
         * @param internalids
         * @param source
         */
        MessageSource(std::string ids, std::string internalids, std::string source);

        /*!
         * @brief 从json字符串反序列化到MessageSource对象
         * @note json应该为以下格式
         * @code
         * {"ids":"", "internalids":""}
         * @endcode
         */
        static MessageSource deserializeFromString(const std::string &source);

        std::string serializeToString() const;

        /*!
         * @brief 撤回该信息
         * @example 撤回信息
         * @code
         * Event::processor.registerEvent([](GroupMessageEvent e) {
            try {
                e.messageSource.recall();
                e.group.SendMsg("hi").recall();
            }
            catch (MiraiCPException &e) {
                Logger::logger.error("错误");
            }
            });
         * @endcode
        */
        void recall(JNIEnv * = ThreadManager::getEnv()) const;
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MESSAGESOURCE_H
