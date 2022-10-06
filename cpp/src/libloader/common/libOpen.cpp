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

#include "libOpen.h"


#if MIRAICP_WINDOWS
#include <cstdio>
#include <windows.h>
#define OPEN_LIBRARY(path) LoadLibrary(TEXT(path.c_str()))
#define CLOSE_LIBRARY(handle) FreeLibrary((HMODULE) handle)
#define GET_SYMBOL(handle, symbol) GetProcAddress((HINSTANCE) handle, symbol)
#else
#include <dlfcn.h>
#define OPEN_LIBRARY(path) dlopen(path.c_str(), RTLD_LAZY)
#define CLOSE_LIBRARY(handle) dlclose(handle)
#define GET_SYMBOL(handle, symbol) dlsym(handle, symbol)
#endif

namespace LibLoader::LoaderApi {
    plugin_handle libOpen(const std::string &path) {
        return OPEN_LIBRARY(path);
    }

    plugin_func_ptr libSymbolLookup(void *handle, const char *symbol) {
        return (plugin_func_ptr) GET_SYMBOL(handle, symbol);
    }

    int libClose(void *handle) {
        return CLOSE_LIBRARY(handle);
    }
} // namespace LibLoader::LoaderApi