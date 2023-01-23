//
// Created by 60168 on 2023-1-24.
//

#ifndef MIRAICP_PRO_JSONTOOLS_H
#define MIRAICP_PRO_JSONTOOLS_H
#include <json.hpp>

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
