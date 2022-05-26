//
// Created by antares on 5/25/22.
//

#ifndef MIRAICP_PRO_LOADERTOOLS_H
#define MIRAICP_PRO_LOADERTOOLS_H

#include <jni.h>
#include <json.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace LibLoader {
    std::vector<std::string> collect_plugins(const std::string &cfgPath, nlohmann::json j);

    std::string jstring2str(jstring jStr);

    jstring str2jstring(const char *cstr);


} // namespace LibLoader

#endif //MIRAICP_PRO_LOADERTOOLS_H
