#ifndef MIRAICP_PRO_PLUGINCONFIG_H
#define MIRAICP_PRO_PLUGINCONFIG_H

#include <json.hpp>

namespace MiraiCP {
    using json = nlohmann::json;
    class PluginConfig {
    public:
        /// @brief 插件id, 要与别人不一样否则报错无法加载(建议用类包格式，如: io.github.nambers)
        std::string id;
        /// @brief 插件名称
        std::string name;
        /// @brief 插件版本
        std::string version;
        /// @brief 插件作者(及联系方式)
        std::string author;
        /// @brief [optional]插件描述
        std::string description;
        /// @brief [optional]构建时间, 默认为__DATE__宏
        std::string time;

        PluginConfig(std::string id,
                     std::string name,
                     std::string version,
                     std::string author,
                     std::string description = "",
                     std::string time = __DATE__)
            : id(std::move(id)),
              name(std::move(name)),
              version(std::move(version)),
              author(std::move(author)),
              description(std::move(description)),
              time(std::move(time)) {}

        json serialize();

        std::string serialize2string() {
            return serialize().dump();
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_PLUGINCONFIG_H
