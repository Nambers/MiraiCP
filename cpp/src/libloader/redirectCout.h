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

#include <sstream>

#ifndef MIRAICP_PRO_REDIRECTCOUT_H
#define MIRAICP_PRO_REDIRECTCOUT_H

namespace LibLoader {
    class OString : private std::streambuf, public std::ostream {
    private:
        // 缓冲区
        std::ostringstream result{};

    public:
        static OString outTarget;
        static OString errTarget;

    private:
        // 输出是否为 info 级别
        explicit OString(bool info) : std::ostream(this), info(info) {}

    public:
        ~OString() override = default;

        // 加入缓冲区
        int overflow(std::streambuf::int_type c) override;
        // std::streambuff 的 sync
        int sync() override;
        // 输出缓冲区内容, 相当于 flush
        std::string out();

    private:
        bool info;
    };
} // namespace MiraiCP
#endif //MIRAICP_PRO_REDIRECTCOUT_H
