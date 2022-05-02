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

#include "Exception.h"
#include "Event.h"

namespace MiraiCP {
    void MiraiCPExceptionBase::basicRaise() const {
        Logger::logger.error(this->what());
    }

    void MiraiCPExceptionBase::raise() const {
        this->basicRaise();
        if (!filename.empty() && lineNum != 0)
            Logger::logger.error("文件名:" + filename + "\n行号:" + std::to_string(lineNum));
    }

    void MiraiCPExceptionBase::exception_broadcast() {
        Event::broadcast<MiraiCPExceptionEvent>(MiraiCPExceptionEvent(this));
    }
} // namespace MiraiCP
