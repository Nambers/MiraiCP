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

#include <utility>

#include <nlohmann/detail/abi_macros.hpp>
#include <nlohmann/detail/conversions/from_json.hpp>
#include <nlohmann/detail/conversions/to_json.hpp>
#include <nlohmann/detail/meta/identity_tag.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN

/// @sa https://json.nlohmann.me/api/adl_serializer/
template<typename ValueType, typename>
struct adl_serializer {
    /// @brief convert a JSON value to any value type
    /// @sa https://json.nlohmann.me/api/adl_serializer/from_json/
    template<typename BasicJsonType, typename TargetType = ValueType>
    static auto from_json(BasicJsonType &&j, TargetType &val) noexcept(
            noexcept(::nlohmann::from_json(std::forward<BasicJsonType>(j), val)))
            -> decltype(::nlohmann::from_json(std::forward<BasicJsonType>(j), val), void()) {
        ::nlohmann::from_json(std::forward<BasicJsonType>(j), val);
    }

    /// @brief convert a JSON value to any value type
    /// @sa https://json.nlohmann.me/api/adl_serializer/from_json/
    template<typename BasicJsonType, typename TargetType = ValueType>
    static auto from_json(BasicJsonType &&j) noexcept(
            noexcept(::nlohmann::from_json(std::forward<BasicJsonType>(j), detail::identity_tag<TargetType>{})))
            -> decltype(::nlohmann::from_json(std::forward<BasicJsonType>(j), detail::identity_tag<TargetType>{})) {
        return ::nlohmann::from_json(std::forward<BasicJsonType>(j), detail::identity_tag<TargetType>{});
    }

    /// @brief convert any value type to a JSON value
    /// @sa https://json.nlohmann.me/api/adl_serializer/to_json/
    template<typename BasicJsonType, typename TargetType = ValueType>
    static auto to_json(BasicJsonType &j, TargetType &&val) noexcept(
            noexcept(::nlohmann::to_json(j, std::forward<TargetType>(val))))
            -> decltype(::nlohmann::to_json(j, std::forward<TargetType>(val)), void()) {
        ::nlohmann::to_json(j, std::forward<TargetType>(val));
    }
};

NLOHMANN_JSON_NAMESPACE_END
