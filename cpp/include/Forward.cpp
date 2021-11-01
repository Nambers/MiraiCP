#include "Forward.h"

namespace MiraiCP {
    //发送这个聊天记录
    MessageSource ForwardMessage::sendTo(Contact *c, JNIEnv *env) {
        json temp;
        json text;
        text["id"] = c->id();
        text["groupid"] = c->groupid();
        text["type"] = c->type();
        text["content"] = sendmsg;
        temp["text"] = text.dump();
        temp["botid"] = c->botid();
        std::string re = Config::koperation(Config::Buildforward, temp, env);
        //TODO:https://github.com/mamoe/mirai/issues/1371
        return MessageSource::deserializeFromString(re);
    }

    ForwardMessage::ForwardMessage(Contact *c, std::initializer_list<ForwardNode> nodes) {
        json root;
        json value;
        root["type"] = c->type();
        root["id"] = c->id();
        root["id2"] = c->groupid();
        for (const ForwardNode &node: nodes) {
            json temp;
            temp["id"] = node.id;
            temp["time"] = node.time;
            temp["message"] = node.message;
            temp["name"] = node.name;
            value.push_back(temp);
        }
        root["value"] = value;
        sendmsg = root;
    }
} // namespace MiraiCP
