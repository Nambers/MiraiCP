//
// Created by antares on 6/2/22.
//


#include "eventHandle.h"
#include <json.hpp>


void builtInCommand(const std::string &cmd) {
    using nlohmann::json;
    json j = json::parse(cmd);
    // todo 解析内建指令
}