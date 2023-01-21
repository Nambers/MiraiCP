// Copyright (c) 2022. Eritque arcus and contributors.
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

#pragma once

#include <nlohmann/detail/abi_macros.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN
namespace detail {

    /*!
@brief replace all occurrences of a substring by another string

@param[in,out] s  the string to manipulate; changed so that all
               occurrences of @a f are replaced with @a t
@param[in]     f  the substring to replace with @a t
@param[in]     t  the string to replace @a f

@pre The search string @a f must not be empty. **This precondition is
enforced with an assertion.**

@since version 2.0.0
*/
    template<typename StringType>
    inline void replace_substring(StringType &s, const StringType &f,
                                  const StringType &t) {
        JSON_ASSERT(!f.empty());
        for (auto pos = s.find(f);            // find first occurrence of f
             pos != StringType::npos;         // make sure f was found
             s.replace(pos, f.size(), t),     // replace with t, and
             pos = s.find(f, pos + t.size())) // find next occurrence of f
        {}
    }

    /*!
 * @brief string escaping as described in RFC 6901 (Sect. 4)
 * @param[in] s string to escape
 * @return    escaped string
 *
 * Note the order of escaping "~" to "~0" and "/" to "~1" is important.
 */
    template<typename StringType>
    inline StringType escape(StringType s) {
        replace_substring(s, StringType{"~"}, StringType{"~0"});
        replace_substring(s, StringType{"/"}, StringType{"~1"});
        return s;
    }

    /*!
 * @brief string unescaping as described in RFC 6901 (Sect. 4)
 * @param[in] s string to unescape
 * @return    unescaped string
 *
 * Note the order of escaping "~1" to "/" and "~0" to "~" is important.
 */
    template<typename StringType>
    static void unescape(StringType &s) {
        replace_substring(s, StringType{"~1"}, StringType{"/"});
        replace_substring(s, StringType{"~0"}, StringType{"~"});
    }

} // namespace detail
NLOHMANN_JSON_NAMESPACE_END
