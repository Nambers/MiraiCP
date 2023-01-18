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

#ifndef MIRAICP_PRO_MESSAGESOURCE_H
#define MIRAICP_PRO_MESSAGESOURCE_H

#include "MiraiCPMacros.h"
// -----------------------
#include "commonTypes.h"
#include <string>


namespace MiraiCP {
    class MiraiCodeable; // forward declaration
    /*! 消息源声明
     * @doxygenEg{1014, message.cpp, 回复信息}
    */
    class MessageSource {
    public:
        /// 消息的ids
        std::string ids;
        /// 消息的internalids
        std::string internalids;
        /// 消息源序列化
        std::string source;

        MessageSource() = default;

        /// @deprecated 用Contact.quoteAndSendMessage, since v2.8.1
        ShouldNotUse("Use Contact.quoteAndSendMessage") MessageSource
                quoteAndSendMiraiCode(MiraiCodeable *msg, QQID groupid = 0,
                                      void *env = nullptr) const = delete;

        /// @deprecated use Contact.quoteAndSendMessage, since v2.8.1
        ShouldNotUse("Use Contact.quoteAndSendMessage") MessageSource
                quoteAndSendMsg(const std::string &c, QQID groupid = 0,
                                void * = nullptr) const = delete;

        /// @deprecated use Contact.quoteAndSendMessage, since v2.8.1
        ShouldNotUse("Use Contact.quoteAndSendMessage") MessageSource
                quoteAndSendMiraiCode(const std::string &c, QQID groupid = 0,
                                      void * = nullptr) const = delete;

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
         * {"ids":"", "internalIds":""}
         * @endcode
         */
        static MessageSource deserializeFromString(const std::string &source);

        std::string serializeToString() const;

        /// @brief 撤回该信息
        void recall() const;

        bool operator==(const MessageSource &ms) const {
            return this->ids == ms.ids && this->internalids == ms.internalids;
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MESSAGESOURCE_H
