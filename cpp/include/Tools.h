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

#include <functional>
#include <jni.h>
#include <sstream>
#include <string>
#include <vector>

#if defined(__clang__) || defined(__GNUC__)
#define MIRAICP_CPP_STANDARD __cplusplus
#elif defined(_MSC_VER)
#define MIRAICP_CPP_STANDARD _MSVC_LANG
#endif


#define MiraiCP_defer(code)                              \
    auto __defered_statement_wrapper__ = [&]() { code }; \
    Tools::MiraiCPDefer<void> __defered_object__(__defered_statement_wrapper__);


//#if MIRAICP_CPP_STANDARD >= 201703L
//#define get_return_type std::invoke_result_t
//#else
//#define get_return_type std::result_of_t
//#endif

namespace MiraiCP {
    /// @brief 工具类声明, 常用的一些转换工具, 如需转码使用std::filesystem
    /// @class Tools
    namespace Tools {
        /*!
         * @name jstring2str
         * @brief string类型转码转换到jstring类型, UTF16 -> UTF8
         * @note 来源https://blog.csdn.net/chunleixiahe/article/details/51394116
         * @param jstr 转换内容,jstring类型
         * @param env 可选，JNIEnv
         * @return 内容转换成jstring类型
         */
        std::string jstring2str(jstring jstr, JNIEnv * = nullptr);
        /*!
         * @name str2jstring
         * @brief string类型到jsting类型 UTF8 -> UTF16
         * @note 来源https://blog.csdn.net/chunleixiahe/article/details/51394116
         * @param stra const char*(string.c_str()转换的内容)
         * @param env 可选JNIEnv
         * @return 转换后jstring类型
         */
        jstring str2jstring(const char *stra, JNIEnv * = nullptr);
        /*!
         * @brief 替换全部在一个字符串中.
         * @param str 原字符串.
         * @param from 需要被替换的字符.
         * @param to 替换到的字符.
         * @return 返回替换后的字符串.
         * @note 来源:https://stackoverflow.com/a/24315631/14646226
         */
        std::string replace(std::string str, std::string_view from, std::string_view to);
        /// @brief long long 类型的vector格式化输出
        /// @param a vector
        /// @return string
        template<typename T>
        std::string VectorToString(std::vector<T> a, const std::string &separator = ",") {
            std::stringstream ss;
            for (size_t i = 0; i < a.size(); ++i) {
                if (i != 0)
                    ss << separator;
                ss << a[i];
            }
            std::string s = ss.str();
            return s;
        }
        /// @brief 从string格式化到vector
        /// @param temp string
        /// @return vector
        std::vector<unsigned long long> StringToVector(std::string temp);
        /// @brief 从miraicode转义到正常
        /// @param s 经过miraicode转义的字符串
        /// @return 原字符串
        std::string escapeFromMiraiCode(const std::string &s);
        /// @brief 转义miraicode格式
        std::string escapeToMiraiCode(const std::string &s);
        /// starts_with, from <https://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-stdstring-starts-with-a-certain-string-and-convert-a>
        bool starts_with(std::string_view f, std::string_view s);
        /// compare char with case-insensitive
        bool icompareChar(const char &c1, const char &c2);
        /// case insensitive string compare from https://thispointer.com/c-case-insensitive-string-comparison-using-stl-c11-boost-library/
        bool iequal(std::string_view str1, std::string_view str2);
        /// from https://www.zhihu.com/question/36642771, delim is regex(ignore last `+`)
        std::vector<std::string> split(const std::string &text, const std::string &delim);

        /// defer class
        /// @see MiraiCP_defer
        template<typename RT_TYPE>
        class MiraiCPDefer {
        public:
            std::function<RT_TYPE()> defer_func;

            template<class F>
            MiraiCPDefer(F &&func) : defer_func(func) {
            }

            virtual ~MiraiCPDefer() {
                defer_func();
            }
        };
    }; // namespace Tools
} // namespace MiraiCP

// #undef get_return_type

#endif //MIRAICP_PRO_TOOLS_H
