// Copyright (c) 2021-2021. Eritque arcus and contributors.
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

#include "Event.h"

namespace MiraiCP {
    Event Event::processor = Event();

    [[deprecated("Use Event::processor instead")]] Event *const procession = &Event::processor;

    MessageChain PrivateMessageEvent::nextMessage(long time, bool halt, JNIEnv *env) {
        json j;
        j["contactSource"] = this->sender.serializationToString();
        j["time"] = time;
        j["halt"] = halt;
        std::string r = Config::koperation(Config::NextMsg, j, env);
        if (r == "-1")
            MiraiCPThrow(TimeOutException("取下一条信息超时"));
        json re = json::parse(r);
        return MessageChain::deserializationFromMiraiCode(re["message"]).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    MessageChain GroupMessageEvent::nextMessage(long time, bool halt, JNIEnv *env) {
        json j;
        j["contactSource"] = this->group.serializationToString();
        j["time"] = time;
        j["halt"] = halt;
        std::string r = Config::koperation(Config::NextMsg, j, env);
        if (r == "-1")
            MiraiCPThrow(TimeOutException("取下一条信息超时"));
        json re = json::parse(r);
        return MessageChain::deserializationFromMiraiCode(re["message"]).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    MessageChain GroupMessageEvent::senderNextMessage(long time, bool halt, JNIEnv *env) {
        json j;
        j["contactSource"] = this->sender.serializationToString();
        j["time"] = time;
        j["halt"] = halt;
        std::string r = Config::koperation(Config::NextMsg, j, env);
        if (r == "-1")
            MiraiCPThrow(TimeOutException("取下一条信息超时"));
        json re = json::parse(r);
        return MessageChain::deserializationFromMessageSourceJson(json::parse(re["messageSource"].get<std::string>())).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }
} // namespace MiraiCP
