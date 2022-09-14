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
#include "Exception.h"
#include "KtOperation.h"
#include <utility>
namespace LibLoader::LoaderApi {
    const interface_funcs *get_loader_apis();
}

namespace MiraiCP {
    using json = nlohmann::json;

    std::string LowLevelAPI::send0( std::string content, json c, int retryTime, bool miraicode,
                                   const std::string &errorInfo) {
        nlohmann::json j;
        nlohmann::json tmp;
        tmp["content"] = std::move(content);
        tmp["contact"] = std::move(c);
        j["source"] = tmp.dump();
        j["miraiCode"] = miraicode;
        j["retryTime"] = retryTime;
        return KtOperation::ktOperation(KtOperation::Send, j, true, errorInfo);
    }

    LowLevelAPI::info LowLevelAPI::info0(const std::string &source) {
        info re;
        MIRAICP_ERROR_HANDLE(source, "");
        nlohmann::json j = nlohmann::json::parse(source);
        re.avatarUrl = j["avatarUrl"];
        re.nickornamecard = j["nickornamecard"];
        return re;
    }

    std::string LowLevelAPI::getInfoSource(const std::string &c) {
        nlohmann::json j;
        j["source"] = c;
        return KtOperation::ktOperation(KtOperation::RefreshInfo, j);
    }

    std::string LowLevelAPI::uploadImg0(const std::string &path, const std::string &c) {
        nlohmann::json j;
        j["fileName"] = path;
        j["source"] = c;
        return KtOperation::ktOperation(KtOperation::UploadImg, j);
    }

    bool checkSafeCall() {
        return LibLoader::LoaderApi::get_loader_apis() != nullptr;
    }
} // namespace MiraiCP
