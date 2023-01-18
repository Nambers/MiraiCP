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

    LowLevelAPI::info LowLevelAPI::info0(const std::string &source) {
        MIRAICP_ERROR_HANDLE(source, "");
        auto j = nlohmann::json::parse(source);
        info re{Tools::json_stringmover(j, "nickOrNameCard"), Tools::json_stringmover(j, "avatarUrl")};
        return re;
    }

    std::string LowLevelAPI::getInfoSource(nlohmann::json c) {
        nlohmann::json j{{"contact", std::move(c)}};
        return KtOperation::ktOperation(KtOperation::RefreshInfo, j);
    }

    std::string LowLevelAPI::uploadImg0(std::string path, nlohmann::json c) {
        nlohmann::json j{{"filePath", std::move(path)}, {"contact", c}};
        return KtOperation::ktOperation(KtOperation::UploadImg, j);
    }

    bool checkSafeCall() {
        return LibLoader::LoaderApi::get_loader_apis() != nullptr;
    }
} // namespace MiraiCP
