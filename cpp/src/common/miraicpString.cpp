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

#include "miraicpString.h"
#include <cstring>


namespace MiraiCP {
    void swap(MiraiCPString &a, MiraiCPString &b) noexcept {
        std::swap(a.str, b.str);
        std::swap(a._size, b._size);
        std::swap(a.free_this, b.free_this);
    }

    // avoid calling this if _size == 0
    void MiraiCPString::construction() {
        str = (char *) ::std::malloc(sizeof(char) * (_size + 1));
        if (str == nullptr) {
            // to keep str, use static storage
            static std::string message; // don't give value at init
            message = "Cannot allocate size of " + std::to_string(_size);
            throw std::bad_alloc();
        }
    }

    MiraiCPString::~MiraiCPString() { // do not inherit MiraiCPString!!
        if (str != nullptr) {
            // ensure deconstruction is paired to construction
            free_this(str);
            str = nullptr;
        }
    }

    MiraiCPString::MiraiCPString(const MiraiCPString &other) : str(nullptr), _size(other._size), free_this(std_free_ptr) {
        if (_size == 0) return;
        construction();
        assert(str != nullptr);
        memcpy(str, other.str, _size * sizeof(char));
        str[_size] = 0;
    }

    MiraiCPString::MiraiCPString(MiraiCPString &&temp) noexcept : MiraiCPString() {
        swap(*this, temp);
    }

    MiraiCPString::MiraiCPString(const char *char_str) : MiraiCPString() {
        if (char_str == nullptr) return;
        _size = strlen(char_str);
        if (0 == _size) return;
        construction();
        assert(str != nullptr);
        memcpy(str, char_str, _size * sizeof(char));
        str[_size] = 0;
    }

    MiraiCPString::MiraiCPString(const std::string &string_str) {
        _size = string_str.size();
        if (!_size) return;
        construction();
        assert(str != nullptr);
        memcpy(str, string_str.c_str(), _size * sizeof(char));
        str[_size] = 0;
    }

    const char *MiraiCPString::copyToCharPtr() const {
        if (str == nullptr || _size == 0) return new char[1]{0};
        char *t = new char[_size + 1];
        memcpy(t, str, (_size + 1) * sizeof(char));
        return t;
    }

    bool MiraiCPString::operator==(const MiraiCPString &another) const {
        return another._size == _size && (_size == 0 || strcmp(another.str, str) == 0);
    }

    MiraiCPString &MiraiCPString::operator=(const MiraiCPString &another) {
        MiraiCPString temp(another);
        std::swap(*this, temp);
        return *this;
    }

    MiraiCPString &MiraiCPString::operator=(MiraiCPString &&another) noexcept {
        std::swap(*this, another);
        return *this;
    }
} // namespace MiraiCP
