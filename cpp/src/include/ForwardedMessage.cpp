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
#include <utility>

namespace MiraiCP {
    using json = nlohmann::json;

    json ForwardedMessage::nodesToJson() {
        json value;
        for (const ForwardedNode &node: nodes) {
            json temp;
            temp["id"] = node.id;
            temp["time"] = node.time;
            if (node.isForwarded()) {
                temp["isForwardedMessage"] = true;
                temp["message"] = std::get<std::shared_ptr<ForwardedMessage>>(node.message)->nodesToJson().dump();
                if (display.has_value())
                    temp["display"] = display->toJson();
            } else
                temp["message"] = std::get<MessageChain>(node.message).toMiraiCode();
            temp["name"] = node.name;
            value.emplace_back(std::move(temp));
        }
        json tmp = this->sendmsg;
        tmp["value"] = std::move(value);
        return tmp;
    }

    //发送这个聊天记录
    MessageSource ForwardedMessage::sendTo(Contact *c) {
        json text = c->toJson();
        //        text["id"] = c->id();
        //        text["groupid"] = c->groupid();
        //        text["type"] = c->type();
        text["content"] = this->nodesToJson();
        json temp{{"text", text.dump()}};
        // temp["botid"] = c->botid();
        if (display.has_value())
            temp["display"] = display->toJson();
        std::string re = KtOperation::ktOperation(KtOperation::Buildforward, std::move(temp));
        MIRAICP_ERROR_HANDLE(re, "");
        return MessageSource::deserializeFromString(re);
    }

    ForwardedMessage ForwardedMessage::deserializationFromMessageSourceJson(const json &j) {
        std::vector<ForwardedNode> nodes;
        try {
            for (auto &&a: j) {
                if (a["messageChain"][0].contains("kind") && a["messageChain"][0]["kind"] == "FORWARD") {
                    nodes.emplace_back(a["senderId"], a["senderName"], ForwardedMessage::deserializationFromMessageSourceJson(a["messageChain"][1]["nodeList"]), a["time"], ForwardedMessageDisplayStrategy::defaultStrategy());
                } else {
                    nodes.emplace_back(a["senderId"], a["senderName"], MessageChain::deserializationFromMessageSourceJson(a["messageChain"], false), a["time"]);
                }
            }
        } catch (const json::exception &) {
            throw APIException("ForwardedMessage格式化异常", MIRAICP_EXCEPTION_WHERE);
        }

        return {std::move(nodes), ForwardedMessageDisplayStrategy::defaultStrategy()};
    }


    OnlineForwardedMessage OnlineForwardedMessage::deserializationFromMessageSourceJson(const json &j) {
        std::vector<ForwardedNode> nodes;

        try {
            for (auto &&a: j[1]["nodeList"]) {
                if (a["messageChain"][0].contains("kind") && a["messageChain"][0]["kind"] == "FORWARD") {
                    nodes.emplace_back(a["senderId"], a["senderName"], ForwardedMessage::deserializationFromMessageSourceJson(a["messageChain"][1]["nodeList"]), a["time"], ForwardedMessageDisplayStrategy::defaultStrategy());
                } else {
                    nodes.emplace_back(a["senderId"], a["senderName"], MessageChain::deserializationFromMessageSourceJson(a["messageChain"], false), a["time"]);
                }
            }

        } catch (json::parse_error &e) {
            throw APIException("OnlineForwardedMessage格式化异常", MIRAICP_EXCEPTION_WHERE);
        }

        //if (j[0].contains("resourceId") && !j[0]["resourceId"].is_null())
        return OnlineForwardedMessage(j[0]["origin"], /*j[0]["resourceId"],*/ std::move(nodes));
        //else
        // return OnlineForwardedMessage(j[0]["origin"], std::nullopt, std::move(nodes));
    }

    ForwardedNode::ForwardedNode(QQID id, std::string name, ForwardedMessage _message, int t, std::optional<ForwardedMessageDisplayStrategy> display)
        : id(id), name(std::move(name)),
          message(std::make_shared<ForwardedMessage>(std::move(_message))),
          time(t), isForwardedMessage(true), display(std::move(display)) {}

    /*
    ForwardedNode::ForwardedNode(Contact *c, MessageChain message, int t) : id(c->id()), name(c->nickOrNameCard()),
                                                                            message(std::move(message)),
                                                                            time(t) {}

    ForwardedNode::ForwardedNode(QQID id, std::string name, ForwardedMessage &message, int t) : id(id), name(std::move(name)), forwardedMsg(&message), time(t) {}
    */

    bool OnlineForwardedMessage::operator==(const OnlineForwardedMessage &m) const {
        if (this->nodelist.size() != m.nodelist.size()) return false;

        int i = 0;
        return std::all_of(this->nodelist.begin(), this->nodelist.end(), [&i, &m](const auto &n) {
            return n.message == m[i++].message;
        });
    }

    ForwardedMessage OnlineForwardedMessage::toForwardedMessage(std::optional<ForwardedMessageDisplayStrategy> display) const {
        return {this->nodelist, std::move(display)};
    }
} // namespace MiraiCP
