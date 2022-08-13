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
#include <memory>
#include <sstream>
#ifdef MIRAICP_LIB_LOADER
#include "LoaderLogger.h"
#else
#include "Logger.h"
#endif

class OStreamRedirector {
public:
    ~OStreamRedirector() {
        obj->rdbuf(old);
        obj = nullptr;
        old = nullptr;
    }

    /**
         * @brief 重定向 obj 的流
         * @param obj 需要重定向的流
         * @param new_buffer 重定向到的新缓冲区
         */
    explicit OStreamRedirector(std::ostream *obj, std::streambuf *newBuffer)
        : old(obj->rdbuf(newBuffer)), obj(obj) {}

private:
    // 被重定向的流
    std::ostream *obj;
    // 旧的缓冲区目标
    std::streambuf *old;
};

class OString : public std::ostream {
    class OStringBuf : public std::streambuf {
        friend class OString;
        typedef void (*Recorder)(std::string);
        //
        explicit OStringBuf(bool inIsInfoLevel) : isInfoLevel(inIsInfoLevel) {}
        ~OStringBuf() override = default;
        //
        std::ostringstream result{};
        Recorder recorder = nullptr;
        bool isInfoLevel;
        //
        // 输出缓冲区内容, 相当于 flush
        std::string out();
        // std::streambuff interface
        int sync() override {
            record(out());
            return 0;
        }

        // std::streambuff interface
        // 加入缓冲区
        int overflow(std::streambuf::int_type c) override {
            if (c == EOF)
                record(out());
            else
                result.put((std::streambuf::char_type) c);
            return c;
        }

        void record(std::string message) {
            if (recorder) recorder(std::move(message));
        }
    };

public:
    using Recorder = OStringBuf::Recorder;

private:
    // 缓冲区
    OStringBuf sbuf;

public:
    // 输出是否为 info 级别
    explicit OString(bool info) : sbuf(info), std::ostream(&sbuf) {}

    ~OString() override = default;

    void setRecorder(Recorder recorder) {
        sbuf.recorder = recorder;
    }
};

// --- impl ---

std::string OString::OStringBuf::out() {
#ifdef MIRAICP_LIB_LOADER
    if (isInfoLevel)
        LibLoader::logger.info(result.str());
    else
        LibLoader::logger.error(result.str());
#else
    if (isInfoLevel)
        MiraiCP::Logger::logger.info(result.str());
    else
        MiraiCP::Logger::logger.error(result.str());
#endif
    auto temp = result.str();
    result.str("");
    return temp;
}

OString outTarget(true);
OString errTarget(false);
std::unique_ptr<OStreamRedirector> outRedirector;
std::unique_ptr<OStreamRedirector> errRedirector;

void MiraiCP::Redirector::reset() {
    outRedirector.reset();
    errRedirector.reset();
}

void MiraiCP::Redirector::start() {
    outRedirector = std::make_unique<OStreamRedirector>(&std::cout, outTarget.rdbuf());
    errRedirector = std::make_unique<OStreamRedirector>(&std::cerr, errTarget.rdbuf());
}

namespace MiraiCP::Redirector {
    /// @note dev: this function is only used for tests;
    ///  should never be declared in source headers
    void SetCoutRecorder(void (*recorder)(std::string)) {
        outTarget.setRecorder(recorder);
    }

    /// @note dev: this function is only used for tests;
    ///  should never be declared in source headers
    void SetCerrRecorder(void (*recorder)(std::string)) {
        errTarget.setRecorder(recorder);
    }
} // namespace MiraiCP::Redirector