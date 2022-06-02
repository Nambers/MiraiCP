//
// Created by antares on 5/27/22.
//


#ifndef MIRAICP_PRO_LIBOPEN_H
#define MIRAICP_PRO_LIBOPEN_H


#if _WIN32 || WIN32

#else
#include <dlfcn.h>
#endif


#if _WIN32 || WIN32
// todo(Antares): implement windows dlopen
#define OPEN_LIBRARY(path)
#define CLOSE_LIBRARY(handle)
#define GET_SYMBOL(handle, symbol)
#else
#define OPEN_LIBRARY(path) dlopen(path.c_str(), RTLD_LAZY)
#define CLOSE_LIBRARY(handle) dlclose(handle)
#define GET_SYMBOL(handle, symbol) dlsym(handle, symbol)
#endif


namespace LibLoader {
    // dlopen or sth like dlopen on Windows
    inline void *libOpen(const std::string &path) {
        return OPEN_LIBRARY(path);
    }

    inline void *libSymbolLookup(void *handle, const char *symbol) {
        return GET_SYMBOL(handle, symbol);
    }

    inline int libClose(void *handle) {
        return CLOSE_LIBRARY(handle);
    }
} // namespace LibLoader
#endif //MIRAICP_PRO_LIBOPEN_H
