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

    class OStreamRedirector {
    public:
        ~OStreamRedirector() {
            obj.rdbuf(old);
        }

        /**
         * @brief 重定向 obj 的流
         * @param obj 需要重定向的流
         * @param new_buffer 重定向到的新缓冲区
         */
        explicit OStreamRedirector(std::ostream &obj, std::streambuf *newBuffer)
            : old(obj.rdbuf(newBuffer)), obj(obj) {}

    private:
        // 被重定向的流
        std::ostream &obj;
        // 旧的缓冲区目标
        std::streambuf *old;
    };

    class OString : private std::streambuf, public std::ostream {
    public:
        // 输出是否为 info 级别
        explicit OString(bool info) : std::ostream(this), info(info) {}

    private:
        bool info;
        // 缓冲区
        std::ostringstream result{};
        // 加入缓冲区
        int overflow(std::streambuf::int_type c) override {
            if (c == EOF)
                out();
            else
                result.put((std::streambuf::char_type) c);
            return c;
        }
        // std::streambuff 的 sync
        int sync() override {
            out();
            return 0;
        }
        // 输出缓冲区内容, 相当于 flush
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
