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

#ifndef MIRAICP_PRO_MIRAICPSTRINGINTERNAL_H
#define MIRAICP_PRO_MIRAICPSTRINGINTERNAL_H


#include <cassert>
#include <string>


namespace MiraiCP {
    // this class is used to ensure data consistency between dynamic libs
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
        decltype(&::std::free) free_this = std_free_ptr; // specify which free() to use; ensure deconstruction is paired to construction

    public:
        bool isEmpty() const {
            return _size == 0;
        }

        MiraiCPString() : str(nullptr), _size(0), free_this(std_free_ptr) {}
        // call if _size is set to non-zero
        // allocate memory for str
        void construction();

        ~MiraiCPString();

        MiraiCPString(const MiraiCPString &other);

        MiraiCPString(MiraiCPString &&temp) noexcept;

        MiraiCPString(const char *char_str);

        MiraiCPString(const std::string &string_str);

        std::string toString() const {
            if (str == nullptr || _size == 0) return {};
            return {str};
        }

        operator std::string() const {
            return toString();
        }

        // for safe destruction, DO NOT provide move convert to char*
        // the return value of this method can always be deleted by delete[] and is never nullptr
        const char *copyToCharPtr() const;

        bool operator==(const MiraiCPString &another) const;

        MiraiCPString &operator=(const MiraiCPString &another);

        MiraiCPString &operator=(MiraiCPString &&another) noexcept;

    private:
        void swap(MiraiCPString &other) noexcept;
    };

    static_assert(sizeof(char) == 1, "Please make sure the size of char type is 1");
    static_assert(sizeof(MiraiCPString) == 3 * 8, "Please make sure MiraiCP is compiled under 64-bit mode.");
} // namespace MiraiCP

#endif //MIRAICP_PRO_MIRAICPSTRINGINTERNAL_H
