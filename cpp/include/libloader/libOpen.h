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

#ifndef MIRAICP_PRO_LIBOPEN_H
#define MIRAICP_PRO_LIBOPEN_H


#include "commonTypes.h"

namespace LibLoader::LoaderApi {
    // dlopen or sth like dlopen on Windows
    plugin_handle libOpen(const std::string &path);

    plugin_func_ptr libSymbolLookup(void *handle, const char *symbol);

    int libClose(void *handle);

    std::string libError();
} // namespace LibLoader::LoaderApi
#endif //MIRAICP_PRO_LIBOPEN_H
