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
#include "Group.h"

namespace MiraiCP {
    using json = nlohmann::json;
    MessageSource MessageChain::quoteAndSend0(const std::string &msg, QQID groupid,
                                              JNIEnv *env) {
        json obj;
        json sign;
        obj["messageSource"] = this->source->serializeToString();
        obj["msg"] = msg;
        sign["MiraiCode"] = true;
        sign["groupid"] = groupid;
        obj["sign"] = sign.dump();
        std::string re = Config::koperation(Config::SendWithQuote, obj, env);
        return MessageSource::deserializeFromString(re);
    }

    //message chain
    MessageChain MessageChain::deserializationFromMiraiCode(const std::string &m) {
        size_t pos = 0;
        size_t lastPos = -1;
        MessageChain mc;
        if (m.length() <= 7) {
            return MessageChain(PlainText(m));
        }
        do {
            if (m.length() - 7 - pos > 0 && m.substr(pos, 7) == "[mirai:") {
                if (pos - lastPos > 1)
                    mc.add(PlainText(m.substr(lastPos + 1, pos - lastPos - 1))); // plain text
                size_t back = MessageChain::findEnd(m, pos);
                if (back == -1) MiraiCPThrow(IllegalStateException(""));
                std::string tmp = m.substr(pos, back - pos);
                tmp = Tools::replace(tmp, "[mirai:", "");
                size_t i = tmp.find(':'); // first :
                int t = SingleMessage::getKey(tmp.substr(0, i));
                switch (t) {
                    case 0:
                        // no miraiCode key is PlainText
                        Logger::logger.error("无法预料的错误, 信息: " + m);
                        break;
                    case 1:
                        mc.add(At(std::stoll(tmp.substr(i + 1, tmp.length() - i - 1))));
                        break;
                    case 2:
                        mc.add(AtAll());
                        break;
                    case 3:
                        mc.add(Image(tmp.substr(i + 1, tmp.length() - i - 1)));
                        break;
                    case 4:
                        mc.add(LightApp(tmp.substr(i + 1, tmp.length() - i - 1)));
                        break;
                    case 5: {
                        size_t comma = tmp.find(',');
                        mc.add(ServiceMessage(std::stoi(tmp.substr(i + 1, comma - i - 1)),
                                              tmp.substr(comma + 1, tmp.length() - comma - 1)));
                        break;
                    }
                    case 6: {
                        //[mirai:file:/b53231e8-46dd-11ec-8ba5-5452007bd6c0,102,run.bat,55]
                        size_t comma1 = tmp.find(',');
                        size_t comma2 = tmp.find(',', comma1 + 1);
                        size_t comma3 = tmp.find(',', comma2 + 1);
                        mc.add(RemoteFile(tmp.substr(i + 1, comma1 - i - 1), std::stoi(tmp.substr(comma1 + 1, comma2 - comma1 - 1)), tmp.substr(comma2 + 1, comma3 - comma2 - 1), std::stoll(tmp.substr(comma3 + 1, tmp.length() - comma3 - 1))));
                        break;
                    }
                    case 7:
                        mc.add(Face(std::stoi(tmp.substr(i + 1, tmp.length() - i - 1))));
                        break;
                    default:
                        Logger::logger.error(
                                "MiraiCP碰到了意料之中的错误(原因:部分SimpleMessage在MiraiCode解析支持之外)\n请到MiraiCP(github.com/Nambers/MiraiCP)发送issue并复制本段信息使MiraiCP可以支持这种消息: MiraiCode:" +
                                m);
                        mc.add(UnSupportMessage("[mirai:" + tmp));
                        break;
                }
                pos = back;
                lastPos = pos;
                if (t == 1)
                    lastPos++;
            }
            pos++;
        } while (pos < m.length());
        if (lastPos + 1 < m.length())
            mc.add(PlainText(m.substr(lastPos + 1, m.length() - lastPos - 1))); // plain text
        return mc;
    }

    MessageChain MessageChain::deserializationFromMessageSourceJson(const json &tmp, bool origin) {
        json j = tmp;
        if (origin)
            j = tmp["originalMessage"];
        MessageChain mc;
        if (j[0]["type"] == "MessageOrigin") {
            mc.add(OnlineForwardedMessage::deserializationFromMessageSourceJson(j));
            return mc;
        }
        for (auto node: j) {
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
                mc.add(Group(tmp["targetId"].get<QQID>(), tmp["botId"].get<QQID>()).getFileById(node["id"]).plus(node["internalId"]));
                continue;
            }
            if (node["type"] == "MarketFace") {
                mc.add(MarketFace(node["delegate"]["faceId"]));
                break;
            }
            switch (SingleMessage::getKey(node["type"])) {
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
                default:
                    Logger::logger.error(
                            "MiraiCP碰到了意料之中的错误(原因:接受到的SimpleMessage在MessageSource解析支持之外)\n请到MiraiCP(github.com/Nambers/MiraiCP)发送issue并复制本段信息使MiraiCP可以支持这种消息: MessageSource:" +
                            j.dump());
                    mc.add(UnSupportMessage(node["content"]));
            }
        }
        return mc;
    }
} // namespace MiraiCP
