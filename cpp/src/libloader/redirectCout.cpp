// Copyright (c) 2022. Eritque arcus and contributors.
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

#include "redirectCout.h"
#include <iostream>
#ifdef MIRAICP_LIB_LOADER
#include "LoaderLogger.h"
#else
#include "Logger.h"
#endif

std::string LibLoader::OString::out() {
#ifdef MIRAICP_LIB_LOADER
    if (info)
        LibLoader::logger.info(result.str());
    else
        LibLoader::logger.error(result.str());
#else
    if (info)
        MiraiCP::Logger::logger.info(result.str());
    else
        MiraiCP::Logger::logger.error(result.str());
#endif
    auto temp = result.str();
    result.str("");
    return temp;
}
int LibLoader::OString::overflow(std::streambuf::int_type c) {
    if (c == EOF)
        out();
    else
        result.put((std::streambuf::char_type) c);
    return c;
}

int LibLoader::OString::sync() {
    out();
    return 0;
}

LibLoader::OString LibLoader::OString::outTarget(true);
LibLoader::OString LibLoader::OString::errTarget(false);
std::unique_ptr<LibLoader::OStreamRedirector> LibLoader::outRedirector;
std::unique_ptr<LibLoader::OStreamRedirector> LibLoader::errRedirector;