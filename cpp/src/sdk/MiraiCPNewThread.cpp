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