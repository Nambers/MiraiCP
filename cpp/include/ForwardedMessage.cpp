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

#include "ForwardedMessage.h"

namespace MiraiCP {
    using json = nlohmann::json;
    //发送这个聊天记录
    MessageSource ForwardedMessage::sendTo(Contact *c, JNIEnv *env) {
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

    ForwardedMessage::ForwardedMessage(Contact *c, std::initializer_list<ForwardedNode> nodes) {
        json root;
        json value;
        root["type"] = c->type();
        root["id"] = c->id();
        root["id2"] = c->groupid();
        for (const ForwardedNode &node: nodes) {
            json temp;
            temp["id"] = node.id;
            temp["time"] = node.time;
            temp["message"] = node.message.toMiraiCode();
            temp["name"] = node.name;
            value.push_back(temp);
        }
        root["value"] = value;
        sendmsg = root;
    }
    OnlineForwardedMessage OnlineForwardedMessage::deserializationFromMessageSourceJson(json j) {
        std::vector<ForwardedNode> nodes;
        for (json a: j[1]["nodeList"])
            nodes.emplace_back(a["senderId"], a["senderName"], MessageChain::deserializationFromMessageSourceJson(a["messageChain"], false), a["time"]);
        return OnlineForwardedMessage(j[0]["origin"], j[0]["resourceId"], nodes);
    }
} // namespace MiraiCP
