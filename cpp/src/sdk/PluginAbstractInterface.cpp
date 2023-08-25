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

#include "MsgDefine.h"
#include "MsgSender.h"
#include <deque>
#include <memory>
#include <polym/Msg.hpp>
#include "commonTypes.h"

namespace MiraiCP::PluginInterface {
    static auto &deleteQueue() {
        static std::deque<std::unique_ptr<PolyM::Msg>> queue;
        return queue;
    }

    PayLoadInfo _try_get_payload() {
        PayLoadInfo ret{nullptr, -1};
        auto queue_ptr = MiraiCP::getMsgQueue();
        auto msg = queue_ptr->tryGet();
        if (!msg) return ret;
        ret.payload = msg->getRawPayLoad();
        ret.payload_id = msg->getMsgId();
        deleteQueue().emplace_back(std::move(msg));
        return ret;
    }

    void _delete_one_msg() {
        deleteQueue().pop_front();
    }
} // namespace MiraiCP
