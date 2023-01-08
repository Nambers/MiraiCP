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
#include "Contact.h"
#include "KtOperation.h"


namespace MiraiCP {
    using json = nlohmann::json;

    ForwardedNode::ForwardedNode(QQID id, std::string name, ForwardedMessage _message, int t)
        : id(id), name(std::move(name)),
          message(MessageChain(std::move(_message))),
          time(t), isForwardedMessage(true) {}

    bool ForwardedMessageDisplayStrategy::operator==(const ForwardedMessageDisplayStrategy &other) const {
        return this->toJson() == other.toJson();
    }
    bool ForwardedMessage::operator==(const ForwardedMessage &m) const {
        if (this->nodes.size() != m.nodes.size()) return false;
        int i = 0;
        if(!(this->display == m.display)) return false;
        return std::all_of(this->nodes.begin(), this->nodes.end(), [&i, &m](const auto &n) {
            return n.message == m[i++].message;
        });
    }

    json ForwardedMessage::nodesToJson() const { // NOLINT(misc-no-recursion)
        auto value = json::array();
        for (const ForwardedNode &node: nodes) {
            json temp{{"senderId", node.id},
                      {"time", node.time},
                      {"senderName", node.name},
                      {"messageChain", node.message.toJson()}};
            value.emplace_back(std::move(temp));
        }
        return value;
    }

    //发送这个聊天记录
    MessageSource ForwardedMessage::sendTo(Contact *c) const{
        json req{{"contact", c->toJson()},
                 {"display", display.toJson()},
                 {"nodes", nodesToJson()}};
        std::string re = KtOperation::ktOperation(KtOperation::Buildforward, req);
        MIRAICP_ERROR_HANDLE(re, "");
        return MessageSource::deserializeFromString(re);
    }

    ForwardedMessage ForwardedMessage::deserializationFromMessageJson(const json &msg) {
        if(!(msg.contains("type") && msg["type"]=="ForwardMessage") && msg.contains("nodeList"))
            throw std::invalid_argument("json is not a ForwardMessage");
        ForwardedMessageDisplayStrategy display;
        if(msg.contains("title") && msg.contains("brief") && msg.contains("source") && msg.contains("summary") && msg.contains("preview"))
            display = ForwardedMessageDisplayStrategy::fromJson(msg);
        else
            display = ForwardedMessageDisplayStrategy::defaultStrategy();
        std::vector<ForwardedNode> nodes;
        json j = msg["nodeList"];
        if(!j.is_array())
            throw IllegalArgumentException("OnlineForwardedMessage参数异常, 应为 array", MIRAICP_EXCEPTION_WHERE);
        try {
            for (auto &&a: j) {
                if (a["messageChain"][0].contains("kind") && a["messageChain"][0]["kind"] == "FORWARD") {
                    nodes.emplace_back(a["senderId"], a["senderName"], ForwardedMessage::deserializationFromMessageJson(a["messageChain"][1]["nodeList"]), a["time"]);
                } else {
                    nodes.emplace_back(a["senderId"], a["senderName"], MessageChain::deserializationFromMessageJson(a["messageChain"]), a["time"]);
                }
            }

        } catch (const json::parse_error &) {
            throw APIException("OnlineForwardedMessage格式化异常", MIRAICP_EXCEPTION_WHERE);
        }
        return ForwardedMessage(std::move(nodes), display);
    }

    nlohmann::json ForwardedMessage::toJson() const {
        return {{"type", "ForwardMessage"},
                          {"preview", display.preview},
                          {"source", display.source},
                          {"title", display.title},
                          {"brief", display.brief},
                          {"summary", display.summary},
                          {"nodeList", nodesToJson()}};
    }
} // namespace MiraiCP
