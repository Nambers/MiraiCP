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

// This file contains all macro definitions affecting or depending on the ABI

#ifndef JSON_SKIP_LIBRARY_VERSION_CHECK
#if defined(NLOHMANN_JSON_VERSION_MAJOR) && defined(NLOHMANN_JSON_VERSION_MINOR) && defined(NLOHMANN_JSON_VERSION_PATCH)
#if NLOHMANN_JSON_VERSION_MAJOR != 3 || NLOHMANN_JSON_VERSION_MINOR != 11 || NLOHMANN_JSON_VERSION_PATCH != 1
#warning "Already included a different version of the library!"
#endif
#endif
#endif

#define NLOHMANN_JSON_VERSION_MAJOR 3  // NOLINT(modernize-macro-to-enum)
#define NLOHMANN_JSON_VERSION_MINOR 11 // NOLINT(modernize-macro-to-enum)
#define NLOHMANN_JSON_VERSION_PATCH 1  // NOLINT(modernize-macro-to-enum)

#ifndef JSON_DIAGNOSTICS
#define JSON_DIAGNOSTICS 0
#endif

#ifndef JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON
#define JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON 0
#endif

#if JSON_DIAGNOSTICS
#define NLOHMANN_JSON_ABI_TAG_DIAGNOSTICS _diag
#else
#define NLOHMANN_JSON_ABI_TAG_DIAGNOSTICS
#endif

#if JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON
#define NLOHMANN_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON _ldvcmp
#else
#define NLOHMANN_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON
#endif

#define NLOHMANN_JSON_ABI_PREFIX_EX(major, minor, patch) \
    json_v##major##_##minor##_##patch
#define NLOHMANN_JSON_ABI_PREFIX(major, minor, patch) \
    NLOHMANN_JSON_ABI_PREFIX_EX(major, minor, patch)

#define NLOHMANN_JSON_ABI_CONCAT_EX(a, b, c) a##b##c
#define NLOHMANN_JSON_ABI_CONCAT(a, b, c) \
    NLOHMANN_JSON_ABI_CONCAT_EX(a, b, c)

#define NLOHMANN_JSON_ABI_STRING                  \
    NLOHMANN_JSON_ABI_CONCAT(                     \
            NLOHMANN_JSON_ABI_PREFIX(             \
                    NLOHMANN_JSON_VERSION_MAJOR,  \
                    NLOHMANN_JSON_VERSION_MINOR,  \
                    NLOHMANN_JSON_VERSION_PATCH), \
            NLOHMANN_JSON_ABI_TAG_DIAGNOSTICS,    \
            NLOHMANN_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON)

#ifndef NLOHMANN_JSON_NAMESPACE
#define NLOHMANN_JSON_NAMESPACE nlohmann::NLOHMANN_JSON_ABI_STRING
#endif

#ifndef NLOHMANN_JSON_NAMESPACE_BEGIN
#define NLOHMANN_JSON_NAMESPACE_BEGIN \
    namespace nlohmann {              \
        inline namespace NLOHMANN_JSON_ABI_STRING {
#endif

#ifndef NLOHMANN_JSON_NAMESPACE_END
#define NLOHMANN_JSON_NAMESPACE_END \
    } /* namespace (abi_string) */  \
    } /* namespace nlohmann */
#endif
