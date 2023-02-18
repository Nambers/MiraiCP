// Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_MIRAICPEXCEPTIONEVENT_H
#define MIRAICP_PRO_MIRAICPEXCEPTIONEVENT_H


#include "EventBase.h"


namespace MiraiCP {
    class MiraiCPExceptionBase; // forward declaration

    /// @brief 异常抛出事件
    class MiraiCPExceptionEvent : public MiraiCPEvent {
    private:
        MiraiCPExceptionBase *exceptionPtr;

    public:
        explicit MiraiCPExceptionEvent(MiraiCPExceptionBase *err) {
            exceptionPtr = err;
        }

    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::MiraiCPExceptionEvent;
        }

        eventTypes::Types getEventType() const override { return get_event_type(); }

        const MiraiCPExceptionBase *getException() const {
            return exceptionPtr;
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MIRAICPEXCEPTIONEVENT_H
