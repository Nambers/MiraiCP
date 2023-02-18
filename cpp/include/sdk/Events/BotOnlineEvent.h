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

#ifndef MIRAICP_PRO_BOTONLINEEVENT_H
#define MIRAICP_PRO_BOTONLINEEVENT_H


#include "BotEvent.h"


namespace MiraiCP {
    /// 机器人上线事件
    class BotOnlineEvent : public BotEvent<BotOnlineEvent> {
    public:
        static eventTypes::Types get_event_type() {
            return eventTypes::Types::BotOnlineEvent;
        }

    public:
        explicit BotOnlineEvent(QQID botid) : BotEvent(botid) {}
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_BOTONLINEEVENT_H
