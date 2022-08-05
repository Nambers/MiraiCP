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

// restore clang diagnostic settings
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// clean up
#undef JSON_ASSERT
#undef JSON_INTERNAL_CATCH
#undef JSON_THROW
#undef JSON_PRIVATE_UNLESS_TESTED
#undef NLOHMANN_BASIC_JSON_TPL_DECLARATION
#undef NLOHMANN_BASIC_JSON_TPL
#undef JSON_EXPLICIT
#undef NLOHMANN_CAN_CALL_STD_FUNC_IMPL
#undef JSON_INLINE_VARIABLE
#undef JSON_NO_UNIQUE_ADDRESS
#undef JSON_DISABLE_ENUM_SERIALIZATION
#undef JSON_USE_GLOBAL_UDLS

#ifndef JSON_TEST_KEEP_MACROS
#undef JSON_CATCH
#undef JSON_TRY
#undef JSON_HAS_CPP_11
#undef JSON_HAS_CPP_14
#undef JSON_HAS_CPP_17
#undef JSON_HAS_CPP_20
#undef JSON_HAS_FILESYSTEM
#undef JSON_HAS_EXPERIMENTAL_FILESYSTEM
#undef JSON_HAS_THREE_WAY_COMPARISON
#undef JSON_HAS_RANGES
#undef JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON
#endif

#include <nlohmann/thirdparty/hedley/hedley_undef.hpp>
