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

#include <cstddef> // size_t

#include <nlohmann/detail/abi_macros.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN
namespace detail {

    /// struct to capture the start position of the current token
    struct position_t {
        /// the total number of characters read
        std::size_t chars_read_total = 0;
        /// the number of characters read in the current line
        std::size_t chars_read_current_line = 0;
        /// the number of lines read
        std::size_t lines_read = 0;

        /// conversion to size_t to preserve SAX interface
        constexpr operator size_t() const {
            return chars_read_total;
        }
    };

} // namespace detail
NLOHMANN_JSON_NAMESPACE_END
