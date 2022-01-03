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
#include "LowLevelAPI.h"
#include "Config.h"
#include "Exception.h"

namespace MiraiCP {
    using json = nlohmann::json;
    std::string LowLevelAPI::send0(const std::string &content, json c, int retryTime, bool miraicode, JNIEnv *env,
                                   const std::string &errorInfo) {
        nlohmann::json j;
        nlohmann::json tmp;
        tmp["content"] = content;
        tmp["contact"] = c;
        j["source"] = tmp.dump();
        j["miraiCode"] = miraicode;
        j["retryTime"] = retryTime;
        return Config::koperation(Config::Send, j, env, true, errorInfo);
    }
    LowLevelAPI::info LowLevelAPI::info0(const std::string &source) {
        info re;
        ErrorHandle(source, "");
        nlohmann::json j = nlohmann::json::parse(source);
        re.avatarUrl = j["avatarUrl"];
        re.nickornamecard = j["nickornamecard"];
        return re;
    }

    std::string LowLevelAPI::getInfoSource(const std::string &c, JNIEnv *env = nullptr) {
        nlohmann::json j;
        j["source"] = c;
        return Config::koperation(Config::RefreshInfo, j, env);
    }
    std::string LowLevelAPI::uploadImg0(const std::string &path, const std::string &c, JNIEnv *env = nullptr) {
        nlohmann::json j;
        j["fileName"] = path;
        j["source"] = c;
        return Config::koperation(Config::UploadImg, j, env);
    }
} // namespace MiraiCP
