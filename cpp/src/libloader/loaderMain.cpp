//
// Created by antares on 5/20/22.
//

#include "json.hpp"
#include <fstream>
#include <jni.h>

#if _WIN32 || WIN32

#else
#include <dlfcn.h>
#endif
namespace MiraiCP {
    // TODO(antares): jni str implements, include Tools.h later
    std::string jstring2str(jstring jStr);

#if _WIN32 || WIN32

#else
    void enrollPlugin(jstring _cfgPath) {
        typedef void (*void_funct_ptr)();
        void *handle = nullptr;
        std::string cfgPath = jstring2str(std::move(_cfgPath));
        std::ifstream i;
        i.open(cfgPath.c_str(), std::ios::in);
        nlohmann::json j;
        i >> j;
        i.close();
        void_funct_ptr func_ptr = nullptr;
        // TODO(antares): dlopen all plugin in json
    }
#endif
} // namespace MiraiCP
