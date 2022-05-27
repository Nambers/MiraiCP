//
// Created by antares on 5/27/22.
//

#ifndef MIRAICP_PRO_LIBOPEN_H
#define MIRAICP_PRO_LIBOPEN_H



#if _WIN32 || WIN32

#else
#include <dlfcn.h>
#endif

namespace LibLoader {
    // dlopen or sth like dlopen on Windows
    inline void *libOpen(const std::string &path) {
#if _WIN32 || WIN32
        return nullptr;
#else
        return dlopen(path.c_str(), RTLD_LAZY);
#endif
    }

    inline void *libSymbolLookup(void *handle, const char *symbol) {
#if _WIN32 || WIN32
        return nullptr;
#else
        return dlsym(handle, symbol);
#endif
    }

    inline int libClose(void *handle) {
#if _WIN32 || WIN32
        return 0;
#else
        return dlclose(handle);
#endif
    }
    /////////////////
} // namespace LibLoader
#endif //MIRAICP_PRO_LIBOPEN_H
