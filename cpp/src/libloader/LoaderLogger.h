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

#ifndef MIRAICP_PRO_LOADERLOGGER_H
#define MIRAICP_PRO_LOADERLOGGER_H


#include <jni.h>
#include <string>
#include <vector>

namespace LibLoader {
    class LoaderLogger {
        using string = std::string;
    public:
        jmethodID logMethod = nullptr;
        void init();
        void info(const string &);
        void warning(const string &);
        void error(const string &);
        std::string vector2string(const std::vector<std::string> &);

    private:
        void call_logger(const string &content, string name, int id, int level);
    };
    extern LoaderLogger logger;
} // namespace LibLoader


#endif //MIRAICP_PRO_LOADERLOGGER_H
