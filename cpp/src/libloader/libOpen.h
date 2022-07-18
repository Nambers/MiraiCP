// Copyright (c) 2022 - 2022. Antares, Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_LIBOPEN_H
#define MIRAICP_PRO_LIBOPEN_H


#if _WIN32 || _WIN64
#include <cstdio>
#include <windows.h>
#else
#include <dlfcn.h>
#endif


#if _WIN32 || _WIN64
// https://docs.microsoft.com/zh-cn/windows/win32/dlls/using-run-time-dynamic-linking
// 以及可能需要GetModuleHandle
#define OPEN_LIBRARY(path) LoadLibrary(TEXT(path.c_str()))
#define CLOSE_LIBRARY(handle) FreeLibrary((HMODULE) handle)
#define GET_SYMBOL(handle, symbol) GetProcAddress((HINSTANCE) handle, symbol)
#else
#define OPEN_LIBRARY(path) dlopen(path.c_str(), RTLD_LAZY)
#define CLOSE_LIBRARY(handle) dlclose(handle)
#define GET_SYMBOL(handle, symbol) dlsym(handle, symbol)
#endif


namespace LibLoader::LoaderApi {
    // dlopen or sth like dlopen on Windows
    inline plugin_handle libOpen(const std::string &path) {
        return OPEN_LIBRARY(path);
    }

    using FuncAddress = void *(*) ();
    inline FuncAddress libSymbolLookup(void *handle, const char *symbol) {
        return (FuncAddress) GET_SYMBOL(handle, symbol);
    }

    inline int libClose(void *handle) {
        return CLOSE_LIBRARY(handle);
    }
} // namespace LibLoader
#endif //MIRAICP_PRO_LIBOPEN_H
