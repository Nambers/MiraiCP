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

#include "MiraiCPNewThread.h"
#include "Event.h"
#include "Events/MiraiCPExceptionEvent.h"
#include <sstream>


namespace MiraiCP {
    /// 如果在 MiraiCPNewThread 中捕获到了非 MiraiCP 之外的异常抛出
    /// @see MiraiCPNewThread
    class MiraiCPThreadException : public MiraiCPExceptionCRTP<MiraiCPThreadException> {
    public:
        /// 抛出异常的线程 ID
        std::thread::id threadId;

    public:
        explicit MiraiCPThreadException(const std::string &exception_content, std::thread::id threadId, string _filename, int _lineNum)
            : MiraiCPExceptionCRTP(exception_content + " at threadId: " + getThreadIdStr(threadId), std::move(_filename), _lineNum),
              threadId(threadId) {}

    public:
        std::string getThreadIdStr() const { return getThreadIdStr(threadId); }

    public:
        static string exceptionType() { return "MiraiCPThreadException"; }

    private:
        MIRAICP_EXPORT static std::string getThreadIdStr(const std::thread::id &id) {
            static std::stringstream ss;
            ss << id;
            auto result = ss.str();
            ss.str("");
            return result;
        }
    };

    void MiraiCPNewThread::threadThrows(const std::string &content) {
        MiraiCPThreadException exNew(content, std::this_thread::get_id(), MIRAICP_EXCEPTION_WHERE);
        exNew.raise();
        Event::broadcast(MiraiCPExceptionEvent(&exNew));
    }

    void MiraiCPNewThread::threadExceptionBroadcast(MiraiCPExceptionBase &e) {
        e.raise();
        Event::broadcast(MiraiCPExceptionEvent(&e));
    }
} // namespace MiraiCP