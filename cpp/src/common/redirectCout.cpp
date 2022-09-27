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
#include "MiraiCPMacros.h"
#include <iostream>
#include <memory>
#include <sstream>

#ifdef MIRAICP_LIB_LOADER
#include "LoaderLogger.h"
#else

#include "Logger.h"

#endif

class OStreamRedirector {
    // 旧的缓冲区目标
    std::streambuf *old;
    // 被重定向的流
    std::ostream *obj;
    std::ostream oldStream;

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
    explicit OStreamRedirector(std::ostream *in_obj, std::streambuf *newBuffer) : old(in_obj->rdbuf(newBuffer)),
                                                                                  obj(in_obj), oldStream(old) {}

private:
    // print directly
    friend void print(const std::string &);
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

        // std::streambuf interface
        int sync() override {
            record(out());
            return 0;
        }

        // std::streambuf interface
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
    explicit OString(bool info) : std::ostream(&sbuf), sbuf(info) {}

    ~OString() override = default;

    void setRecorder(Recorder recorder) {
        sbuf.recorder = recorder;
    }
};

// --- impl ---

OString outTarget(true);
OString errTarget(false);
std::unique_ptr<OStreamRedirector> outRedirector;
std::unique_ptr<OStreamRedirector> errRedirector;

struct RedirectedOstreamPair {
public:
    std::ostream *out = &std::cout;
    std::ostream *err = &std::cerr;
} RedirectedOstreamPointers;

std::string OString::OStringBuf::out() {
    std::string resultstr = result.str();
    result.str("");

#ifdef MIRAICP_LIB_LOADER
    if (isInfoLevel)
        LibLoader::logger.info(resultstr);
    else
        LibLoader::logger.error(resultstr);
#else
    if (isInfoLevel)
        MiraiCP::Logger::logger.info(resultstr);
    else
        MiraiCP::Logger::logger.error(resultstr);
#endif
    return resultstr;
}

void MiraiCP::Redirector::reset() {
    outRedirector.reset();
    errRedirector.reset();
}

void print(const std::string &str) {
    if (outRedirector == nullptr) {
        std::cout << str << std::endl;
    } else {
        outRedirector->oldStream << str;
        outRedirector->oldStream.flush();
    }
}

void MiraiCP::Redirector::setRedirectedObjs(std::ostream *outStream, std::ostream *errStream) {
    RedirectedOstreamPointers = {outStream, errStream};
}

void MiraiCP::Redirector::start() {
    outRedirector = std::make_unique<OStreamRedirector>(RedirectedOstreamPointers.out, outTarget.rdbuf());
    errRedirector = std::make_unique<OStreamRedirector>(RedirectedOstreamPointers.err, errTarget.rdbuf());
}

namespace MiraiCP::Redirector {
    /// @note dev: this function is only used for tests;
    ///  should never be declared in source headers
    MIRAICP_EXPORT void SetCoutRecorder(void (*recorder)(std::string)) {
        outTarget.setRecorder(recorder);
    }

    /// @note dev: this function is only used for tests;
    ///  should never be declared in source headers
    MIRAICP_EXPORT void SetCerrRecorder(void (*recorder)(std::string)) {
        errTarget.setRecorder(recorder);
    }
} // namespace MiraiCP::Redirector