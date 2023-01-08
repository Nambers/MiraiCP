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

#include "MiraiCPMacros.h"
// -----------------------
#include "Exception.h"
#include "KtOperation.h"
#include "loaderApiInternal.h"


namespace MiraiCP::KtOperation {
    std::string ktOperation(operation_set type, const nlohmann::json& data, bool catchErr, const std::string &errorInfo) {
        nlohmann::json j{{"type", type}, {"data", data.dump()}};
        std::string re = LibLoader::LoaderApi::pluginOperation(j.dump());
        if (catchErr) {
            MIRAICP_ERROR_HANDLE(re, errorInfo);
        }
        return re;
    }
} // namespace MiraiCP::KtOperation
