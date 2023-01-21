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

#include <iterator> // random_access_iterator_tag

#include <nlohmann/detail/abi_macros.hpp>
#include <nlohmann/detail/meta/cpp_future.hpp>
#include <nlohmann/detail/meta/void_t.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN
namespace detail {

    template<typename It, typename = void>
    struct iterator_types {};

    template<typename It>
    struct iterator_types<
            It,
            void_t<typename It::difference_type, typename It::value_type, typename It::pointer,
                   typename It::reference, typename It::iterator_category>> {
        using difference_type = typename It::difference_type;
        using value_type = typename It::value_type;
        using pointer = typename It::pointer;
        using reference = typename It::reference;
        using iterator_category = typename It::iterator_category;
    };

    // This is required as some compilers implement std::iterator_traits in a way that
    // doesn't work with SFINAE. See https://github.com/nlohmann/json/issues/1341.
    template<typename T, typename = void>
    struct iterator_traits {
    };

    template<typename T>
    struct iterator_traits<T, enable_if_t<!std::is_pointer<T>::value>>
        : iterator_types<T> {
    };

    template<typename T>
    struct iterator_traits<T *, enable_if_t<std::is_object<T>::value>> {
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;
    };

} // namespace detail
NLOHMANN_JSON_NAMESPACE_END
