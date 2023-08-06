// Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_JSONTOOLS_H
#define MIRAICP_PRO_JSONTOOLS_H
#include <nlohmann/json.hpp>

namespace MiraiCP{
    /// @brief 从json中移动数据，被移动的数据使用后就不再存在，主要用于std::string和json
    template<typename T>
    inline T json_mover(nlohmann::json &j, const std::string &key) {
        return std::move(j[key].get_ref<T &>());
    }

    /// @brief 从json中移动数据，被移动的数据使用后就不再存在，json特化
    template<>
    inline nlohmann::json json_mover(nlohmann::json &j, const std::string &key) {
        return std::move(j[key]);
    }

    inline std::string json_stringmover(nlohmann::json &j, const std::string &key) {
        return json_mover<std::string>(j, key);
    }

    inline nlohmann::json json_jsonmover(nlohmann::json &j, const std::string &key) {
        return json_mover<nlohmann::json>(j, key);
    }
}
#endif //MIRAICP_PRO_JSONTOOLS_H
