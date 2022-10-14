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

#include "LoaderLogger.h"
#include "ktInterface.h"
#include "loaderTools.h"
#include <json.hpp>


namespace LibLoader {
    static const std::string LoaderName = "MiraiCP/LibLoader";
    LibLoader::LoaderLogger logger;

    void LoaderLogger::init() {
    }

    void LoaderLogger::info(const string &msg) const {
        call_logger(msg, LoaderName, -1, 0);
    }

    void LoaderLogger::warning(const string &msg) const {
        call_logger(msg, LoaderName, -1, 1);
    }

    void LoaderLogger::error(const string &msg) const {
        call_logger(msg, LoaderName, -1, 2);
    }

    void LoaderLogger::call_logger(const string &content, string name, long long id, int level) const {
        nlohmann::json j = {
                {"id",  id},
                {"log", content}};
        if (!name.empty()) j["name"] = std::move(name);
        LoaderAPIs::log(j.dump().c_str(), level);
    }
} // namespace LibLoader