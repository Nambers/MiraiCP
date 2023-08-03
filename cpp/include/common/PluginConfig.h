// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_PLUGINCONFIG_H
#define MIRAICP_PRO_PLUGINCONFIG_H


#include <string>


namespace MiraiCP {
    constexpr const char *m_MiraiCPVersion = "v2.15.0";
    inline const std::string MiraiCPVersion = m_MiraiCPVersion;

    struct PluginConfig {
        /// @brief 插件id, 要与别人不一样否则报错无法加载(建议用类包格式，如: io.github.nambers)
        const char *id = nullptr;
        /// @brief 插件名称
        const char *name = nullptr;
        /// @brief 插件版本
        const char *version = nullptr;
        /// @brief 插件作者(及联系方式)
        const char *author = nullptr;
        /// @brief [optional]插件描述
        const char *description = "";
        /// @brief [optional]构建时间, 默认为__DATE__宏
        const char *time = __DATE__;
        const char *mversion = m_MiraiCPVersion;

        [[nodiscard]] std::string getId() const {
            return {id};
        }
        [[nodiscard]] std::string getName() const {
            return {name};
        }
        [[nodiscard]] std::string getVersion() const {
            return {version};
        }
        [[nodiscard]] std::string getAuthor() const {
            return {author};
        }
        [[nodiscard]] std::string getDescription() const {
            return {description};
        }
        [[nodiscard]] std::string getTime() const {
            return {time};
        }
        [[nodiscard]] std::string getMVersion() const {
            return {mversion};
        }
        [[nodiscard]] bool isValid() const {
            return id && name && version && author;
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_PLUGINCONFIG_H
