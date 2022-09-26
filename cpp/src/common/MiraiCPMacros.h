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

#ifndef MIRAICP_PRO_MIRAICPMACROS_H
#define MIRAICP_PRO_MIRAICPMACROS_H


// This file contains static assertions and macros.
// If any macro is used, one should always include this file directly or indirectly.
// Dev: any new macro(s) should be added here since we have to ensure the "single" target
// behaves just the same as "multi". If any macro infected the pre-compilation in "single",
// we will know immediately in "multi".


static_assert(sizeof(void *) == 8, "Only 64-bit platforms are supported");
static_assert(sizeof(char) == 1, "Please make sure the size of char is 1");


// detect platform, pre-define default value
#define MIRAICP_WINDOWS 0
#define MIRAICP_LINUX 0
#define MIRAICP_UNIX 0
#define MIRAICP_IOS 0
#define MIRAICP_MACOS 0
#define MIRAICP_ANDROID 0
#define MIRAICP_TERMUX 0

// detect platform
// ref: https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#undef MIRAICP_WINDOWS
#define MIRAICP_WINDOWS 1
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_MACCATALYST || TARGET_OS_IPHONE
#undef MIRAICP_IOS
#define MIRAICP_IOS 1
#elif TARGET_OS_MAC
#undef MIRAICP_MACOS
#define MIRAICP_MACOS 1
#else
static_assert(false, "Unknown apple platform");
#endif
#elif __ANDROID__
#ifdef __TERMUX__
#undef MIRAICP_TERMUX
#define MIRAICP_TERMUX 1
#else
#undef MIRAICP_ANDROID
#define MIRAICP_ANDROID 1
#endif
#elif __linux__
#undef MIRAICP_LINUX
#define MIRAICP_LINUX 1
#elif __unix__
#undef MIRAICP_UNIX
#define MIRAICP_UNIX 1
#else
static_assert(false, "Unsupported platform");
#endif


// compiler
#define MIRAICP_MSVC 0
#define MIRAICP_GCC 0
#define MIRAICP_COMPILER_OTHER 0

#if defined(_MSC_VER)
#undef MIRAICP_MSVC
#define MIRAICP_MSVC 1
#elif defined(__GNUC__) // GNUC, MinGW
#undef MIRAICP_GCC
#define MIRAICP_GCC 1
#else
#undef MIRAICP_COMPILER_OTHER
#define MIRAICP_COMPILER_OTHER 1
#endif


// ShouldNotUse
#if MIRAICP_MSVC
#pragma warning(error : 4996)
#define ShouldNotUse(msg) [[deprecated(msg)]]
#elif MIRAICP_GCC
#define ShouldNotUse(msg) [[deprecated(msg)]] __attribute__((error(msg)))
#else
#define ShouldNotUse(msg)
#endif


// assert
#include "cassert"
#ifndef assert
#define assert(ignore) ((void) 0)
#endif


// exceptions
#define MIRAICP_EXCEPTION_WHERE __FILE__, __LINE__
#define MIRAICP_THROW(x, ...) throw x(##__VA_ARGS__, MIRAICP_EXCEPTION_WHERE)


// token paste and stringify
#define TOKEN_PASTE_INNER(X, Y) X##Y
#define TOKEN_PASTE(X, Y) TOKEN_PASTE_INNER(X, Y)
#define STRINGIFY(A) __STRINGIFY(A)
#ifndef __STRINGIFY
#define __STRINGIFY(A) #A
#endif


// defer tool, works like the defer key word in Golang.
#include "commonTools.h"
#define MIRAICP_DEFER(code)                                                   \
    auto TOKEN_PASTE(_defered_statement_wrapper_, __LINE__) = [&]() { code }; \
    CommonTools::MiraiCPDefer TOKEN_PASTE(_defered_object_, __LINE__)(std::move(TOKEN_PASTE(_defered_statement_wrapper_, __LINE__)))
#define MIRAICP_DEFER_LAMBDA(lambda)                                  \
    auto TOKEN_PASTE(_defered_statement_wrapper_, __LINE__) = lambda; \
    CommonTools::MiraiCPDefer TOKEN_PASTE(_defered_object_, __LINE__)(std::move(TOKEN_PASTE(_defered_statement_wrapper_, __LINE__)))


// names of plugin entry points
#define FUNC_ENTRANCE FUNC_ENTRANCE
#define FUNC_EVENT FUNC_EVENT
#define FUNC_EXIT FUNC_EXIT
#define PLUGIN_INFO PLUGIN_INFO

// error handling
#ifndef LIBLOADER
#define MIRAICP_ERROR_HANDLE(x, y) ErrorHandle0(__FILE__, __LINE__, (x), (y))
#endif


// export
#define MIRAICP_EXPORT
#if MIRAICP_WINDOWS
#ifdef GOOGLE_TEST
#undef MIRAICP_EXPORT
#define MIRAICP_EXPORT __declspec(dllimport)
#else
#undef MIRAICP_EXPORT
#define MIRAICP_EXPORT __declspec(dllexport)
#endif
#endif


// getter
// need to define macro LOC_CLASS_NAMESPACE to the class first!

#define DECL_GETTER(attr) decltype(DataType::_##attr) attr();
#define IMPL_GETTER(attr)                                                                 \
    decltype(LOC_CLASS_NAMESPACE::DataType::_##attr) LOC_CLASS_NAMESPACE::attr() { \
        InternalData->request_refresh();                                                  \
        std::shared_lock<std::shared_mutex> local_lck(InternalData->get_mutex());         \
        return GetDataInternal()->_##attr;                                                \
    }
#define INLINE_GETTER(attr) \
    auto attr() { return GetDataInternal()->_##attr; }

#endif //MIRAICP_PRO_MIRAICPMACROS_H
