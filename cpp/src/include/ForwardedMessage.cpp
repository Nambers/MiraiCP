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

#include "ForwardedMessage.h"

#include "Config.h"
#include "Exception.h"
#include "Contact.h"
#include <utility>
namespace MiraiCP {
    using json = nlohmann::json;
    json ForwardedMessage::nodesToJson() {
        json value;
        for (const ForwardedNode &node: nodes) {
            json temp;
            temp["id"] = node.id;
            temp["time"] = node.time;
            if (node.forwardedMsg != nullptr) {
                temp["isForwardedMessage"] = true;
                temp["message"] = node.forwardedMsg->nodesToJson().dump();
            } else
                temp["message"] = node.message.toMiraiCode();
            temp["name"] = node.name;
            value.push_back(temp);
        }
        json tmp = this->sendmsg;
        tmp["value"] = value;
        return tmp;
    }
    //发送这个聊天记录
    MessageSource ForwardedMessage::sendTo(Contact *c, JNIEnv *env) {
        json temp;
        json text;
        text["id"] = c->id();
        text["groupid"] = c->groupid();
        text["type"] = c->type();
        text["content"] = this->nodesToJson();
        temp["text"] = text.dump();
        temp["botid"] = c->botid();
        std::string re = Config::koperation(Config::Buildforward, temp, env);
        ErrorHandle(re, "");
        return MessageSource::deserializeFromString(re);
    }
    ForwardedMessage::ForwardedMessage(Contact *c, std::initializer_list<ForwardedNode> nodes) : ForwardedMessage(c, std::vector(nodes)) {}
    ForwardedMessage::ForwardedMessage(Contact *c, std::vector<ForwardedNode> nodes) : nodes(std::move(nodes)) {
        json root;
        root["type"] = c->type();
        root["id"] = c->id();
        root["groupid"] = c->groupid();
        sendmsg = root;
    }
    OnlineForwardedMessage OnlineForwardedMessage::deserializationFromMessageSourceJson(json j) {
        std::vector<ForwardedNode> nodes;
        for (json a: j[1]["nodeList"])
            nodes.emplace_back(a["senderId"], a["senderName"], MessageChain::deserializationFromMessageSourceJson(a["messageChain"], false), a["time"]);
        return OnlineForwardedMessage(j[0]["origin"], j[0]["resourceId"], nodes);
    }
    ForwardedNode::ForwardedNode(Contact *c, MessageChain message, int t) : id(c->id()), name(c->nickOrNameCard()),
                                                                            message(std::move(message)),
                                                                            time(t) {}
    ForwardedNode::ForwardedNode(Contact *c, ForwardedMessage message, int t) : id(c->id()), name(c->nickOrNameCard()),
                                                                                forwardedMsg(std::make_shared<ForwardedMessage>(message)),
                                                                                time(t) {}
    ForwardedNode::ForwardedNode(QQID id, const std::string &name, ForwardedMessage message, int t) : id(id), name(name), forwardedMsg(std::make_shared<ForwardedMessage>(message)), time(t) {}
    bool OnlineForwardedMessage::operator==(const OnlineForwardedMessage &m) const {
        if (this->nodelist.size() != m.nodelist.size())
            return false;
        for (int i = 0; i < this->nodelist.size(); i++)
            if (this->nodelist[i].message != m[i].message)
                return false;
        return true;
    }
    ForwardedMessage OnlineForwardedMessage::toForwardedMessage(Contact *c) {
        return {c, this->nodelist};
    }
} // namespace MiraiCP
