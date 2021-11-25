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

#ifndef MIRAICP_PRO_LOWLEVELAPI_H
#define MIRAICP_PRO_LOWLEVELAPI_H

#include "Config.h"

namespace MiraiCP {
    /// 较底层api
    class LowLevelAPI {
    public:
        /// @brief 抽象封装底层发送信息接口
        /// @param content 信息字符串
        /// @param c 目标Contact->serialization()
        /// @param miraicode 是否为miraicode格式
        /// @param env JNIEnv
        /// @return
        static std::string send0(const std::string &content, nlohmann::json c, int retryTime, bool miraicode, JNIEnv *env,
                                 const std::string &errorInfo = "");

        /// @brief 取该联系人的一些信息
        /// @param c 该联系人Contact->serializationToString()
        /// @return json格式字符串，待解析
        static inline std::string getInfoSource(const std::string &c, JNIEnv *env = ThreadManager::getEnv()) {
            nlohmann::json j;
            j["source"] = c;
            return Config::koperation(Config::RefreshInfo, j, env);
        }

        /*!
         * @brief 上传图片
         * @param path 本地地址
         * @param c 上传的对象, Contact->serializationToString()
         * @param env JNIEnv
         * @return string 待解析json
         */
        static inline std::string uploadImg0(const std::string &path, const std::string &c, JNIEnv *env = ThreadManager::getEnv()) {
            nlohmann::json j;
            j["fileName"] = path;
            j["source"] = c;
            return Config::koperation(Config::UploadImg, j, env);
        }

        /// 每个对象的必有信息
        struct info {
            std::string nickornamecard;
            std::string avatarUrl;
        };

        /// 获取每个对象必有信息
        /// @see LowLevelAPI::info
        static info info0(const std::string &source);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_LOWLEVELAPI_H
