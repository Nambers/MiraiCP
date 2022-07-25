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

#include <iostream>
#include <sstream>

#ifdef MIRAICP_LIB_LOADER
#include "LoaderLogger.h"
#define log(x) LibLoader::logger.info(x)
#define err(x) LibLoader::logger.error(x)
#else
#include "Logger.h"
#define log(x) MiraiCP::Logger::logger.info(x)
#define err(x) MiraiCP::Logger::logger.error(x)
#endif

#ifndef MIRAICP_PRO_REDIRECTCOUT_H
#define MIRAICP_PRO_REDIRECTCOUT_H

namespace MiraiCP {

    struct CoutRedirector {
        explicit CoutRedirector(std::ostream &obj, std::streambuf *new_buffer)
            : old(obj.rdbuf(new_buffer)), obj(obj) {}

        ~CoutRedirector() {
            obj.rdbuf(old);
        }

    private:
        std::ostream &obj;
        std::streambuf *old;
    };

    class OString : private std::streambuf, public std::istream {
    public:
        explicit OString(bool info) : std::istream(this), info(info) {}

    private:
        bool info;
        std::ostringstream result{};

        int overflow(std::streambuf::int_type c) override {
            if (c == EOF)
                out();
            else
                result.put((std::streambuf::char_type) c);
            return c;
        }
        int sync() override {
            out();
            return 0;
        }
        void out() {
            if (info)
                log(result.str());
            else
                err(result.str());
            result.str("");
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_REDIRECTCOUT_H
