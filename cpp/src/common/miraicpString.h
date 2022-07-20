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

#ifndef MIRAICP_PRO_MIRAICPSTRING_H
#define MIRAICP_PRO_MIRAICPSTRING_H
#include "Exception.h"
#include "assert.h"
#include <iostream>
#include <string>

namespace MiraiCP {
    // note: do not use this directly;
    // always convert to const char* or std::string before using.
    class MiraiCPString {
        using string = std::string;

    private:
        static constexpr decltype(&::std::free) std_free_ptr = &::std::free;

    private:
        // to keep integration and safe for empty construction/deconstruction, always initialize here
        char *str = nullptr;
        size_t _size = 0;
        decltype(&::std::free) free_this = std_free_ptr; // specify which free() to use

    public:
        bool isEmpty() const {
            return _size == 0;
        }

        MiraiCPString() = default;
        // call if _size is set to non-zero
        // allocate memory for str
        void construction();

        ~MiraiCPString();

        MiraiCPString(const MiraiCPString &other);

        MiraiCPString(MiraiCPString &&temp) noexcept;

        explicit MiraiCPString(const char *char_str);

        explicit MiraiCPString(const std::string &string_str);

        friend void swap(MiraiCPString &, MiraiCPString &) noexcept;

        std::string toString() const {
            if (str == nullptr) return {};
            return {str};
        }

        operator std::string() const {
            return toString();
        }

        bool operator==(const MiraiCPString &another) const {
            return another._size == _size && (_size == 0 || strcmp(another.str, str) == 0);
        }

        MiraiCPString &operator=(const MiraiCPString &another);

        MiraiCPString &operator=(MiraiCPString &&another) noexcept;

        // for safe destruction, DO NOT provide move convert to char*
        const char *copyToCharPtr() const;
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MIRAICPSTRING_H
