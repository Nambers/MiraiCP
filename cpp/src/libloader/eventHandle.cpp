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
#include "PluginListManager.h"
#include <json.hpp>


namespace LibLoader {
    std::string get_or_empty(nlohmann::json j, const std::string &key) {
        if (j.contains(key)) {
            return j[key];
        }
        return "";
    }

    const static std::unordered_map<std::string, std::function<void(const std::string &)>> actions = {
            // NOLINT(cert-err58-cpp)
            {"EnablePlugin", [](const std::string &name) {
                 LoaderApi::enablePluginById(name);
             }},
            {"DisablePlugin", [](const std::string &name) {
                 LoaderApi::disablePluginById(name);
             }},
            {"DisablePluginList", [](const std::string &name) {
                 LibLoader::logger.info(LibLoader::PluginListManager::pluginListInfo([](LibLoader::LoaderPluginConfig *c) { return !c->enabled; }));
             }},
            {"EnablePluginList", [](const std::string &name) {
                 LibLoader::logger.info(LibLoader::PluginListManager::pluginListInfo([](LibLoader::LoaderPluginConfig *c) { return c->enabled; }));
             }},
            {"ReloadPlugin", [](const std::string &id) {
                 LoaderApi::reloadPluginById(id);
             }},
            {"LoadPlugin", [](const std::string &name) {
                 LoaderApi::loadNewPlugin(name, true);
             }},
            {"UnloadPlugin", [](const std::string &id) {
                 LoaderApi::unloadPluginById(id);
             }},
            {"PluginList", [](const std::string &name) {
                 logger.info(LibLoader::PluginListManager::pluginListInfo([](LibLoader::LoaderPluginConfig *) { return true; }));
             }},
    };

    void builtInCommand(const std::string &cmd) {
        using nlohmann::json;
        json j = json::parse(cmd);
        auto it = actions.find(j["name"]);
        if (it == actions.end()) {
            logger.error("builtInCommand: unknown command: " + j["name"].get<std::string>());
            return;
        }
        it->second(get_or_empty(j, "content"));
    }
} // namespace LibLoader