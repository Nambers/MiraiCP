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
#include <nlohmann/detail/iterators/primitive_iterator.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN
namespace detail {

    /*!
@brief an iterator value

@note This structure could easily be a union, but MSVC currently does not allow
unions members with complex constructors, see https://github.com/nlohmann/json/pull/105.
*/
    template<typename BasicJsonType>
    struct internal_iterator {
        /// iterator for JSON objects
        typename BasicJsonType::object_t::iterator object_iterator{};
        /// iterator for JSON arrays
        typename BasicJsonType::array_t::iterator array_iterator{};
        /// generic iterator for all other types
        primitive_iterator_t primitive_iterator{};
    };

} // namespace detail
NLOHMANN_JSON_NAMESPACE_END
