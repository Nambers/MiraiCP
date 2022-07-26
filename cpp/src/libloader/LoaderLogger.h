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
#ifdef GOOGLE_TEST
#include <functional>
#endif

namespace LibLoader {
    class LoaderLogger {
        using string = std::string;

    public:
        jmethodID logMethod = nullptr;

        void init();

    public:
        /// 注意，这三个函数不是给MiraiCP插件使用的，而是给loader部分使用的
        /// 实际暴露的接口只有call_logger一个，
        /// MiraiCP插件通过call_logger具体实现多种不同的logger
        void info(const string &) const;
        void warning(const string &) const;
        void error(const string &) const;

    public:
#ifdef GOOGLE_TEST
        std::function<void(const string &, const string &, long long, int)> action = {};
#endif
        /// 实际暴露的接口
        void call_logger(const string &content, const string &name, long long id, int level) const;
    };

    extern LoaderLogger logger;
} // namespace LibLoader


#endif //MIRAICP_PRO_LOADERLOGGER_H
