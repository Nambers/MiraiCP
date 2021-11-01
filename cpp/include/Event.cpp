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
            throw TimeOutException("取下一条信息超时");
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
            throw TimeOutException("取下一条信息超时");
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
            throw TimeOutException("取下一条信息超时");
        json re = json::parse(r);
        return MessageChain::deserializationFromMiraiCode(re["message"]).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }
} // namespace MiraiCP
