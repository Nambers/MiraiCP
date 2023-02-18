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

#ifndef MIRAICP_PRO_TOOLS_H
#define MIRAICP_PRO_TOOLS_H

#include "MiraiCPMacros.h"
// -----------------------
#include "SdkType.h"
#include <json_fwd.hpp>
#include <sstream>
#include <string>
#include <vector>


namespace MiraiCP {
    /// @brief 工具类声明, 常用的一些转换工具
    namespace Tools {
        /*!
         * @brief 替换全部在一个字符串中.
         * @param str 原字符串.
         * @param from 需要被替换的字符.
         * @param to 替换到的字符.
         * @return 返回替换后的字符串.
         * @note 来源:https://stackoverflow.com/a/24315631/14646226
         */
        MIRAICP_EXPORT std::string replace(std::string str, std::string_view from, std::string_view to);

        /// @brief 任意类型的vector格式化输出
        /// @param a vector
        /// @return string
        /// @note dev: 尽量避免直接使用，而是使用其特化
        template<typename T>
        inline std::string VectorToString(const std::vector<T> &a, const std::string &separator = ",") {
            std::stringstream ss;
            for (auto it = a.begin(); it != a.end(); ++it) {
                if (it != a.begin()) ss << separator;
                ss << *it;
            }

            return ss.str();
        }

        /// @brief unsigned long long 类型的vector格式化输出，VectorToString<T>的特化
        /// @param a QQID vector
        /// @return string
        template<>
        inline std::string VectorToString(const std::vector<QQID> &a, const std::string &separator) {
            std::string ss;
            for (auto it = a.begin(); it != a.end(); ++it) {
                if (it != a.begin()) ss += separator;
                ss += std::to_string(*it);
            }

            return ss;
        }

        /// @brief string 类型的vector格式化输出，VectorToString<T>的特化
        /// @param a string vector
        /// @return string
        template<>
        inline std::string VectorToString(const std::vector<std::string> &a, const std::string &separator) {
            std::string ans;
            for (auto it = a.begin(); it != a.end(); ++it) {
                if (it != a.begin()) ans += separator;
                ans += *it;
            }

            return ans;
        }

        /// @brief 从string格式化到vector
        /// @param temp string
        /// @return vector
        MIRAICP_EXPORT std::vector<QQID> StringToVector(std::string temp);

        /// @brief 从miraicode转义到正常
        /// @param s 经过miraicode转义的字符串
        /// @return 原字符串
        MIRAICP_EXPORT std::string escapeFromMiraiCode(const std::string &s);

        /// @brief 转义miraicode格式
        MIRAICP_EXPORT std::string escapeToMiraiCode(const std::string &s);

        /// starts_with, from <https://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-stdstring-starts-with-a-certain-string-and-convert-a>
        MIRAICP_EXPORT bool starts_with(std::string_view f, std::string_view s);

        /// compare char with case-insensitive
        MIRAICP_EXPORT bool icompareChar(const char &c1, const char &c2);

        /// case insensitive string compare from https://thispointer.com/c-case-insensitive-string-comparison-using-stl-c11-boost-library/
        MIRAICP_EXPORT bool iequal(std::string_view str1, std::string_view str2);

        /// from https://www.zhihu.com/question/36642771, delim is regex(ignore last `+`)
        MIRAICP_EXPORT std::vector<std::string> split(const std::string &text, const std::string &delim);

        /// @brief id pair工具结构体声明，仅内部使用
        /// @note dev: 为保证可读性请仅在局部作用域使用，且使用聚合初始化
        struct idpair {
            QQID id1;
            QQID id2;

            bool operator==(const idpair &other) const {
                return id1 == other.id1 && id2 == other.id2;
            }
        };


    }; // namespace Tools
} // namespace MiraiCP

/// @brief id pair工具结构体散列特化，用于unordered_map
template<>
struct std::hash<MiraiCP::Tools::idpair> {
    using idpair = MiraiCP::Tools::idpair;
    size_t operator()(const idpair in_pair) const {
        auto First = reinterpret_cast<const char *>(&in_pair);
        size_t Val = 14695981039346656037ULL; // see: type_traits.h (MSVC)
        for (size_t Idx = 0; Idx < sizeof(idpair); ++Idx) {
            Val ^= static_cast<size_t>(First[Idx]);
            Val *= 1099511628211ULL; // see: type_traits.h (MSVC)
        }
        return Val;
    }
};
#endif //MIRAICP_PRO_TOOLS_H
