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

#ifndef MIRAICP_PRO_BOTEVENT_H
#define MIRAICP_PRO_BOTEVENT_H


#include "../Bot.h"
#include "../Logger.h"
#include "EventBase.h"


namespace MiraiCP {
    /// 所有事件处理timeoutevent都是机器人事件，指都有机器人实例
    template<class T>
    class BotEvent : public MiraiCPEvent {

    public:
        [[nodiscard]] eventTypes::Types getEventType() const override { return T::get_event_type(); }

        /// 该事件接受的机器人
        Bot bot;

        /// 以该机器人的名义发送日志
        /// @see BotLogger
        IdLogger botlogger;

        explicit BotEvent(QQID botid) : bot(botid), botlogger(botid, &Logger::logger) {}
        ~BotEvent() override = default;
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_BOTEVENT_H
