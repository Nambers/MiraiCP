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

#include "Tools.h"
#include "loaderApiInternal.h"
#include <regex>


namespace MiraiCP::Tools {
    // 工具函数实现

    std::string replace(std::string str, std::string_view from, std::string_view to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substd::string of 'from'
        }
        return str;
    }

    std::vector<QQID> StringToVector(std::string temp) {
        std::vector<QQID> result;
        temp.erase(temp.begin());
        temp.pop_back();
        std::regex ws_re("[,]+");
        std::vector<std::string> v(std::sregex_token_iterator(temp.begin(), temp.end(), ws_re, -1),
                                   std::sregex_token_iterator());
        result.reserve(v.size());
        std::for_each(v.begin(), v.end(), [&](auto &&s) { result.emplace_back(std::stoull(s)); });
        // for (auto &&s: v)
        //     result.emplace_back(std::stoull(s));
        return result;
    }

    std::string escapeFromMiraiCode(const std::string &s) {
        //[	\[
        //]	\]
        //:	\:
        //,	\,
        //\	\\ /
        return replace(replace(replace(replace(replace(s,
                                                       "\\\\", "\\"),
                                               "\\,", ","),
                                       "\\:", ":"),
                               "\\]", "]"),
                       "\\[", "[");
    }

    std::string escapeToMiraiCode(const std::string &s) {
        //[	\[
        //]	\]
        //:	\:
        //,	\,
        //\	\\ /
        return replace(replace(replace(replace(replace(s,
                                                       "\\", "\\\\"),
                                               ",", "\\,"),
                                       ":", "\\:"),
                               "]", "\\]"),
                       "[", "\\[");
    }

    bool starts_with(std::string_view f, std::string_view s) { return f.rfind(s, 0) == 0; }

    bool icompareChar(const char &c1, const char &c2) {
        return c1 == c2 || std::toupper(c1) == std::toupper(c2);
    }

    bool iequal(std::string_view str1, std::string_view str2) {
        return ((str1.size() == str2.size()) &&
                std::equal(str1.begin(), str1.end(), str2.begin(), &icompareChar));
    }

    std::vector<std::string> split(const std::string &text, const std::string &delim) {
        std::regex ws_re(delim + "+");
        return {std::sregex_token_iterator(text.begin(), text.end(), ws_re, -1), std::sregex_token_iterator()};
    }
} // namespace MiraiCP::Tools
