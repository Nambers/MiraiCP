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


#include "eventHandle.h"
#include "JNIEnvs.h"
#include "LoaderLogger.h"
#include "PluginList.h"
#include <json.hpp>


std::string get_or_empty(nlohmann::json j, const std::string &key) {
    if (j.contains(key)) {
        return j[key];
    }
    return "";
}

const static std::unordered_map<std::string, std::function<void(const std::string &)>> actions = {
        {"EnablePlugin", [](const std::string &name) {
             LibLoader::loader_enablePluginByName(name);
         }},
        {"DisablePlugin", [](const std::string &name) {
             LibLoader::loader_disablePluginByName(name);
         }},
        {"DisablePluginList", [](const std::string &name) {
             // todo DisablePluginList
         }},
        {"EnablePluginList", [](const std::string &name) {
             // todo EnablePluginList
         }},
        {"ReloadPlugin", [](const std::string &name) {
             // todo ReloadPlugin
         }},
        {"LoadPlugin", [](const std::string &name) {
             // todo LoadPlugin
         }},
        {"PluginList", [](const std::string &name) {
             // todo PluginList
         }},
};


void builtInCommand(const std::string &cmd) {
    using nlohmann::json;
    json j = json::parse(cmd);
    auto it = actions.find(j["name"]);
    if (it == actions.end()) {
        JNIEnvs::logger.error("builtInCommand: unknown command: " + j["name"].get<std::string>());
        return;
    }
    it->second(get_or_empty(j, "content"));
}