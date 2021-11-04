#include "ForwardedMessage.h"
#include "Group.h"

namespace MiraiCP {
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
                        break;
                    case 1:
                        mc.add(At(std::stoll(tmp.substr(i + 1, tmp.length() - i - 1))));
                        break;
                    case 2:
                        mc.add(Image(tmp.substr(i + 1, tmp.length() - i - 1)));
                        break;
                    case 3:
                        mc.add(AtAll());
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
                    default:
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
        MessageChain mc;
        if (j.is_array() && j[0]["type"] == "MessageOrigin") {
            mc.add(OnlineForwardedMessage::deserializationFromMessageSourceJson(j));
            return mc;
        }
        Logger::logger.info(tmp);
        if (origin)
            j = tmp["originalMessage"];
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
                mc.add(Group(tmp["targetId"], tmp["botId"]).getFileById(node["id"]).plus(node["internalId"]));
                continue;
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
                case 4:
                    Logger::logger.error(
                            "MiraiCP碰到了预料之外的错误(原因:匹配到了LightApp)\n请到MiraiCP(github.com/Nambers/MiraiCP)发送issue并复制本段话使MiraiCP可以修复: MessageSource:" +
                            j.dump());
                    break;
                case 5:
                    Logger::logger.error(
                            "MiraiCP碰到了预料之外的错误(原因:匹配到了ServiceMessage)\n请到MiraiCP(github.com/Nambers/MiraiCP)发送issue并复制本段话使MiraiCP可以修复: MessageSource:" +
                            j.dump());
                    break;
                default:
                    mc.add(UnSupportMessage(node["content"]));
                    Logger::logger.error(
                            "MiraiCP碰到了意料之中的错误(原因:接受到的SimpleMessage在支持之外)\n请到MiraiCP(github.com/Nambers/MiraiCP)发送issue并复制本段话使MiraiCP可以支持这种消息: MessageSource:" +
                            j.dump());
            }
        }
        return mc;
    }
} // namespace MiraiCP
