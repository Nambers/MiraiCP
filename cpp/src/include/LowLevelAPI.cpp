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
#include "Tools.h"
#include <utility>


namespace LibLoader::LoaderApi {
    const interface_funcs *get_loader_apis();
}

namespace MiraiCP {
    using json = nlohmann::json;

    std::string LowLevelAPI::send0(std::string content, json c, int retryTime, bool miraicode,
                                   const std::string &errorInfo) {
        nlohmann::json tmp{{"content", std::move(content)}, {"contact", std::move(c)}};
        nlohmann::json j{{"source", tmp.dump()}, {"miraiCode", miraicode}, {"retryTime", retryTime}};
        return KtOperation::ktOperation(KtOperation::Send, std::move(j), true, errorInfo);
    }

    LowLevelAPI::info LowLevelAPI::info0(const std::string &source) {
        MIRAICP_ERROR_HANDLE(source, "");
        auto j = nlohmann::json::parse(source);
        info re{Tools::json_stringmover(j, "nickornamecard"), Tools::json_stringmover(j, "avatarUrl")};
        return std::move(re);
    }

    std::string LowLevelAPI::getInfoSource(std::string c) {
        nlohmann::json j{{"source", std::move(c)}};
        return KtOperation::ktOperation(KtOperation::RefreshInfo, std::move(j));
    }

    std::string LowLevelAPI::uploadImg0(std::string path, std::string c) {
        nlohmann::json j{{"fileName", std::move(path)}, {"source", std::move(c)}};
        return KtOperation::ktOperation(KtOperation::UploadImg, std::move(j));
    }

    bool checkSafeCall() {
        return LibLoader::LoaderApi::get_loader_apis() != nullptr;
    }
} // namespace MiraiCP
