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

#include <type_traits>

#include <nlohmann/detail/meta/void_t.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN
namespace detail {

    // https://en.cppreference.com/w/cpp/experimental/is_detected
    struct nonesuch {
        nonesuch() = delete;
        ~nonesuch() = delete;
        nonesuch(nonesuch const &) = delete;
        nonesuch(nonesuch const &&) = delete;
        void operator=(nonesuch const &) = delete;
        void operator=(nonesuch &&) = delete;
    };

    template<class Default,
             class AlwaysVoid,
             template<class...> class Op,
             class... Args>
    struct detector {
        using value_t = std::false_type;
        using type = Default;
    };

    template<class Default, template<class...> class Op, class... Args>
    struct detector<Default, void_t<Op<Args...>>, Op, Args...> {
        using value_t = std::true_type;
        using type = Op<Args...>;
    };

    template<template<class...> class Op, class... Args>
    using is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;

    template<template<class...> class Op, class... Args>
    struct is_detected_lazy : is_detected<Op, Args...> {};

    template<template<class...> class Op, class... Args>
    using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

    template<class Default, template<class...> class Op, class... Args>
    using detected_or = detector<Default, void, Op, Args...>;

    template<class Default, template<class...> class Op, class... Args>
    using detected_or_t = typename detected_or<Default, Op, Args...>::type;

    template<class Expected, template<class...> class Op, class... Args>
    using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

    template<class To, template<class...> class Op, class... Args>
    using is_detected_convertible =
            std::is_convertible<detected_t<Op, Args...>, To>;

} // namespace detail
NLOHMANN_JSON_NAMESPACE_END
