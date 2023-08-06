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

#include "MessageChain.h"
#include "Exception.h"
#include "Exceptions/IllegalArgument.h"
#include "ForwardedMessage.h"
#include "Group.h"
#include "KtOperation.h"
#include "Logger.h"
#include "Tools.h"
#include <nlohmann/json.hpp>


namespace MiraiCP {
    using json = nlohmann::json;

    std::string MessageChain::toMiraiCode() const {
        return Tools::VectorToString(toMiraiCodeVector(), "");
    }

    nlohmann::json MessageChain::toJson() const {
        nlohmann::json j = nlohmann::json::array();
        for (auto &&a: *this)
            j.emplace_back(a->toJson());
        return j;
    }

    std::string MessageChain::toString() const {
        return toJson().dump();
    }

    //message chain
    MessageChain MessageChain::deserializationFromMiraiCode(const std::string &m) {
        std::string re = KtOperation::ktOperationStr(KtOperation::DeserializeMiraiCode, m);
        return MessageChain::deserializationFromMessageJson(json::parse(re));
    }

    MessageChain MessageChain::deserializationFromMessageSourceJson(json j, bool origin) {
        if (origin)
            j = j["originalMessage"];
        MessageChain mc;
        if (j.empty()) return mc;

        if (!j.is_array()) throw IllegalArgumentException(std::string(__func__) + "输入的json应当是数组类型", MIRAICP_EXCEPTION_WHERE);
        json::array_t jArray = std::move(j);
        if (jArray[0]["type"] == "MessageOrigin") {
            if (jArray[0]["kind"] == "MUSIC_SHARE") {
                if (jArray.size() < 2) throw IllegalArgumentException(std::string(__func__) + "数组长度应至少为2", MIRAICP_EXCEPTION_WHERE);

                mc.add(MusicShare(jArray[1]["kind"], jArray[1]["title"], jArray[1]["summary"], jArray[1]["jumpUrl"], jArray[1]["pictureUrl"],
                                  jArray[1]["musicUrl"], jArray[1]["brief"]));
                return mc;
            }
            // todo del MessageSource deserialization methods
            //            mc.add(ForwardedMessage::deserializationFromMessageSourceJson(jArray));
            return mc;
        }

        for (auto &node: jArray) {
            if (node["type"] == "SimpleServiceMessage") {
                mc.add(ServiceMessage(node["serviceId"], node["content"]));
                continue;
            }
            if (node["type"] == "LightApp") {
                mc.add(LightApp(node["content"]));
                continue;
            }
            if (node["type"] == "OnlineAudio") {
                mc.add(OnlineAudio(node["filename"], node["fileMd5"], node["fileSize"], node["codec"], node["length"],
                                   node["urlForDownload"]));
                continue;
            }
            if (node["type"] == "FileMessage") {
                // note: RemoteFile::deserializeFromString() uses j["finfo"]["size"], which is different from here.
                mc.add(RemoteFile(node["id"], node["internalId"], node["name"], node["size"]));
                continue;
            }
            if (node["type"] == "MarketFace") {
                mc.add(MarketFace(node["delegate"]["faceId"]));
                continue;
            }
            switch (SingleMessage::getKey(node["type"])) {
                    // todo(Antares): change to enum in each case
                case -2:
                    mc.add(QuoteReply(MessageSource::deserializeFromString(node["source"].dump())));
                    break;
                case 0:
                    mc.add(PlainText(node["content"].get<std::string>()));
                    break;
                case 1:
                    mc.add(At(node["target"]));
                    break;
                case 2:
                    mc.add(AtAll());
                    break;
                case 3:
                    mc.add(Image(node["imageId"]));
                    break;
                case 7:
                    mc.add(Face(node["id"]));
                    break;
                case 8:
                    mc.add(FlashImage(node["imageId"]));
                    break;
                default:
                    Logger::logger.warning(
                            "MiraiCP碰到了意料之中的错误(原因:接受到的SimpleMessage在MessageSource解析支持之外)\n请到MiraiCP(github.com/Nambers/MiraiCP)发送issue并复制本段信息使MiraiCP可以支持这种消息: node:" +
                            node.dump());
                    mc.add(UnSupportMessage(node.dump()));
            }
        }
        return mc;
    }

    MessageChain MessageChain::deserializationFromMessageJson(const json &j) {
        MessageChain mc;
        if (j.empty()) return mc;
        if (!j.is_array())
            throw IllegalArgumentException(std::string(__func__) + "输入的json应当是数组类型", MIRAICP_EXCEPTION_WHERE);
        for (auto &node: j.get<json::array_t>()) {
            switch (SingleMessage::getKey(node["type"])) {
                case SingleMessageType::MusicShare_t:
                    mc.add(MusicShare(node["kind"], node["title"], node["summary"], node["jumpUrl"], node["pictureUrl"],
                                      node["musicUrl"], node["brief"]));
                    break;
                case SingleMessageType::ServiceMessage_t:
                    mc.add(ServiceMessage(node["serviceId"], node["content"]));
                    break;
                case SingleMessageType::LightApp_t:
                    mc.add(LightApp(node["content"]));
                    break;
                case SingleMessageType::OnlineAudio_t:
                    mc.add(OnlineAudio(node["filename"], node["fileMd5"], node["fileSize"], node["codec"],
                                       node["length"],
                                       node["urlForDownload"]));
                    break;
                case SingleMessageType::MarketFace_t:
                    mc.add(MarketFace(node["delegate"]["faceId"]));
                    break;
                case SingleMessageType::RemoteFile_t:
                    // note: RemoteFile::deserializeFromString() uses j["finfo"]["size"], which is different from here.
                    mc.add(RemoteFile(node["id"], node["internalId"], node["name"], node["size"]));
                    break;
                case SingleMessageType::MessageSource_t:
                    //                    mc.add(MessageSource::deserializeFromString(node.dump()));
                    break;
                case SingleMessageType::QuoteReply_t:
                    mc.add(QuoteReply(MessageSource::deserializeFromString(node["source"].dump())));
                    break;
                case SingleMessageType::UnsupportedMessage_t:
                    mc.add(UnSupportMessage(node["struct"].dump()));
                    break;
                case SingleMessageType::PlainText_t:
                    mc.add(PlainText(node["content"].get<std::string>()));
                    break;
                case SingleMessageType::At_t:
                    mc.add(At(node["target"]));
                    break;
                case SingleMessageType::AtAll_t:
                    mc.add(AtAll());
                    break;
                case SingleMessageType::Image_t:
                    mc.add(Image(node["imageId"], node["size"].get<size_t>(), node["width"].get<int>(),
                                 node["height"].get<int>(), node["imageType"],
                                 node["isEmoji"].get<bool>()));
                    break;
                case SingleMessageType::Face_t:
                    mc.add(Face(node["id"]));
                    break;
                case SingleMessageType::FlashImage_t:
                    mc.add(FlashImage(node["imageId"]));
                    break;
                case SingleMessageType::OnlineForwardedMessage_t:
                    mc.add(ForwardedMessage::deserializationFromMessageJson(node));
                    break;
                default:
                    Logger::logger.warning(
                            "MiraiCP碰到了意料之中的错误(原因:接受到的SimpleMessage在MessageSource解析支持之外)\n请到MiraiCP(github.com/Nambers/MiraiCP)发送issue并复制本段信息使MiraiCP可以支持这种消息: node:" +
                            node.dump());
                    mc.add(UnSupportMessage(node.dump()));
            }
        }
        return mc;
    }

    std::vector<std::string> MessageChain::toMiraiCodeVector() const {
        std::vector<std::string> tmp;
        for (auto &&a: *this)
            tmp.emplace_back(a->toMiraiCode());
        return tmp;
    }

    MessageChain MessageChain::plus(const MessageChain &mc) const {
        MessageChain tmp(*this);
        tmp.insert(tmp.end(), mc.begin(), mc.end());
        return tmp;
    }

    MessageChain MessageChain::plus(const MessageSource &ms) const {
        MessageChain tmp(*this);
        tmp.source = ms;
        return tmp;
    }

    bool MessageChain::operator==(const MessageChain &mc) const {
        if (size() != mc.size())
            return false;
        for (size_t i = 0; i < size(); i++) {
            if ((*this)[i] != mc[i])
                return false;
        }
        return true;
    }

    bool MessageChain::operator!=(const MessageChain &mc) const {
        return !(*this == mc);
    }

    bool MessageChain::empty() const {
        return std::vector<Message>::empty() || toMiraiCode().empty();
    }

    size_t MessageChain::findEnd(const std::string &s, size_t start) {
        size_t pos = start;
        while (pos < s.length()) {
            switch (s[pos]) {
                case '\\':
                    pos += 2;
                    continue;
                case ']':
                    return pos;
            }
            pos++;
        }
        return -1;
    }

    MessageChain MessageChain::deserializationFromMessageSourceJson(const std::string &msg, bool origin) {
        return deserializationFromMessageSourceJson(nlohmann::json::parse(msg), origin);
    }

    internal::Message::Message(internal::Message::Super msgptr) noexcept : Super(std::move(msgptr)) {}

    int internal::Message::getType() const {
        return (*this)->internalType;
    }

    std::string internal::Message::toMiraiCode() const {
        return (*this)->toMiraiCode();
    }

    std::string internal::Message::toJson() const {
        return (*this)->toJson();
    }

    bool internal::Message::operator==(const internal::Message &m) const {
        return (*this)->internalType == m->internalType && (*this)->toMiraiCode() == m->toMiraiCode();
    }

    bool internal::Message::operator!=(const internal::Message &m) const {
        return (*this)->internalType != m->internalType || (*this)->toMiraiCode() != m->toMiraiCode();
    }

    void internal::Message::messageThrow(const std::string& from, const std::string& to, const char* file, int line) {
        throw IllegalArgumentException("cannot convert from " + from + " to " + to, std::string(file), line);
    };
} // namespace MiraiCP
