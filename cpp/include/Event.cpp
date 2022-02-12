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

#include "Event.h"
#include "Config.h"
#include "Exception.h"

namespace MiraiCP {
    using json = nlohmann::json;
    Event Event::processor = Event();

    void GroupInviteEvent::operation0(const std::string &source, QQID botid, bool accept, JNIEnv *env) {
        nlohmann::json j;
        j["text"] = source;
        j["operate"] = accept;
        j["botid"] = botid;
        std::string re = Config::koperation(Config::Gioperation, j, env);
        if (re == "E") Logger::logger.error("群聊邀请事件同意失败(可能因为重复处理),id:" + source);
    }

    MessageChain PrivateMessageEvent::nextMessage(long time, bool halt, JNIEnv *env) const {
        json j;
        j["contactSource"] = this->sender.toString();
        j["time"] = time;
        j["halt"] = halt;
        std::string r = Config::koperation(Config::NextMsg, j, env);
        if (r == "-1")
            MiraiCPThrow(TimeOutException("取下一条信息超时"));
        json re = json::parse(r);
        return MessageChain::deserializationFromMiraiCode(re["message"]).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    MessageChain GroupMessageEvent::nextMessage(long time, bool halt, JNIEnv *env) const {
        json j;
        j["contactSource"] = this->group.toString();
        j["time"] = time;
        j["halt"] = halt;
        std::string r = Config::koperation(Config::NextMsg, j, env);
        if (r == "-1")
            MiraiCPThrow(TimeOutException("取下一条信息超时"));
        json re = json::parse(r);
        return MessageChain::deserializationFromMiraiCode(re["message"]).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    MessageChain GroupMessageEvent::senderNextMessage(long time, bool halt, JNIEnv *env) const {
        json j;
        j["contactSource"] = this->sender.toString();
        j["time"] = time;
        j["halt"] = halt;
        std::string r = Config::koperation(Config::NextMsg, j, env);
        if (r == "-1")
            MiraiCPThrow(TimeOutException("取下一条信息超时"));
        json re = json::parse(r);
        return MessageChain::deserializationFromMessageSourceJson(json::parse(re["messageSource"].get<std::string>())).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    void NewFriendRequestEvent::operation0(const std::string &source, QQID botid, bool accept, JNIEnv *env, bool ban) {
        nlohmann::json j;
        j["text"] = source;
        j["operate"] = accept;
        j["botid"] = botid;
        j["ban"] = ban;
        std::string re = Config::koperation(Config::Nfroperation, j, env);
        if (re == "E") Logger::logger.error("好友申请事件同意失败(可能因为重复处理),id:" + source);
    }

    void MemberJoinRequestEvent::operate(const std::string &s, QQID botid, bool sign, const std::string &msg, JNIEnv *env) const {
        nlohmann::json j;
        j["source"] = s;
        j["botid"] = botid;
        j["sign"] = sign;
        j["msg"] = msg;
        Config::koperation(Config::MemberJoinRequest, j, env);
    }
} // namespace MiraiCP
