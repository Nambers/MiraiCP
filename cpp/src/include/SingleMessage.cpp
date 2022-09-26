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

#include "SingleMessage.h"
#include "Exception.h"
#include "KtOperation.h"
#include "Logger.h"
#include "Tools.h"
#include "commonTools.h"
#include <json.hpp>


namespace MiraiCP {
    using json = nlohmann::json;
    nlohmann::json SingleMessage::toJson() const {
        nlohmann::json re;
        re["key"] = "miraicode";
        re["content"] = this->toMiraiCode();
        return re;
    }
    // 静态成员
    std::unordered_map<int, std::string> SingleMessage::messageType{
            {-5, "MarketFace"},
            {-4, "OnlineForwardedMessage"},
            {-3, "OnlineAudio"},
            {-2, "QuoteReply"},
            {-1, "unSupportMessage"},
            {0, "plainText"},
            {1, "at"},
            {2, "atAll"},
            {3, "image"},
            {4, "app"},
            {5, "service"},
            {6, "file"},
            {7, "face"},
            {8, "FlashImage"},
            {9, "MusicShare"}};

    QuoteReply::QuoteReply(const SingleMessage &m) : SingleMessage(m) {
        if (m.type != -2) throw IllegalArgumentException("cannot convert type(" + std::to_string(m.type) + "to QuoteReply", MIRAICP_EXCEPTION_WHERE);
        source = MessageSource::deserializeFromString(m.content);
    }
    // 结束静态成员
    nlohmann::json PlainText::toJson() const {
        nlohmann::json j;
        j["key"] = "plaintext";
        j["content"] = content;
        return j;
    }
    int SingleMessage::getKey(const std::string &value) {
        for (const auto &a: messageType) {
            if (Tools::iequal(a.second, value)) return a.first;
        }
        return -1;
    }
    std::string SingleMessage::toMiraiCode() const {
        // Logger::logger.info("base");
        if (type > 0)
            if (type == 1)
                return "[mirai:at:" + content + "] ";
            else if (type == 2)
                return "[mirai:atall] ";
            else
                return "[mirai:" + messageType[type] + this->prefix + Tools::escapeToMiraiCode(content) + "]";
        else
            return content;
    }
    PlainText::PlainText(const SingleMessage &sg) : SingleMessage(sg) {
        if (sg.type != 0)
            throw IllegalArgumentException(
                    "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to PlainText", MIRAICP_EXCEPTION_WHERE);
        this->content = sg.content;
    }
    nlohmann::json At::toJson() const {
        nlohmann::json j;
        j["key"] = "at";
        j["content"] = std::to_string(this->target);
        return j;
    }
    At::At(const SingleMessage &sg) : SingleMessage(sg) {
        if (sg.type != 1)
            throw IllegalArgumentException(
                    "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to At", MIRAICP_EXCEPTION_WHERE);
        this->target = std::stol(sg.content);
    }
    nlohmann::json AtAll::toJson() const {
        nlohmann::json j;
        j["key"] = "atall";
        return j;
    }
    nlohmann::json Image::toJson() const {
        nlohmann::json j;
        j["key"] = "image";
        j["imageid"] = this->id;
        j["size"] = this->size;
        j["width"] = this->width;
        j["height"] = this->height;
        j["type"] = this->imageType;
        return j;
    }
    Image::Image(const SingleMessage &sg) : SingleMessage(sg) {
        if (sg.type != 3 && sg.type != 8) throw IllegalArgumentException("传入的SingleMessage应该是Image类型", MIRAICP_EXCEPTION_WHERE);
        this->id = sg.content;
        this->size = this->width = this->height = 0;
        this->imageType = 5;
    }
    bool Image::isUploaded(QQID botid) {
        if (!this->md5.has_value()) this->refreshInfo();
        if (this->size == 0) throw IllegalArgumentException("size不能为0", MIRAICP_EXCEPTION_WHERE);
        nlohmann::json tmp = this->toJson();
        tmp["botid"] = botid;
        std::string re = KtOperation::ktOperation(KtOperation::ImageUploaded, std::move(tmp));
        return re == "true";
    }
    nlohmann::json FlashImage::toJson() const {
        nlohmann::json j;
        j["key"] = "Flashimage";
        j["imageid"] = this->id;
        j["size"] = this->size;
        j["width"] = this->width;
        j["height"] = this->height;
        j["type"] = this->imageType;
        return j;
    }
    nlohmann::json LightApp::toJson() const {
        nlohmann::json j;
        j["key"] = "lightapp";
        j["content"] = this->content;
        return j;
    }
    LightApp::LightApp(const SingleMessage &sg) : SingleMessage(sg) {
        if (sg.type != 3)
            throw IllegalArgumentException(
                    "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to LighApp", MIRAICP_EXCEPTION_WHERE);
    }
    std::string LightApp::toMiraiCode() const {
        return "[mirai:app:" + Tools::escapeToMiraiCode(content) + "]";
    }
    nlohmann::json ServiceMessage::toJson() const {
        nlohmann::json j;
        j["key"] = "servicemessage";
        j["content"] = this->content;
        j["id"] = this->id;
        return j;
    }
    std::string ServiceMessage::toMiraiCode() const {
        return "[mirai:service" + this->prefix + Tools::escapeToMiraiCode(content) + "]";
    }
    ServiceMessage::ServiceMessage(const SingleMessage &sg) : SingleMessage(sg) {
        if (sg.type != 4)
            throw IllegalArgumentException(
                    "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to ServiceMessage", MIRAICP_EXCEPTION_WHERE);
    }
    nlohmann::json Face::toJson() const {
        nlohmann::json j;
        j["key"] = "face";
        j["id"] = this->id;
        return j;
    }
    nlohmann::json UnSupportMessage::toJson() const {
        nlohmann::json j;
        j["key"] = "unsupportmessage";
        j["content"] = this->content;
        return j;
    }

    //远程文件(群文件)
    RemoteFile::RemoteFile(const std::string &i, unsigned int ii, std::string n, long long s, std::string p,
                           struct Dinfo d, struct Finfo f) : SingleMessage(RemoteFile::type(), i + "," + std::to_string(ii) + "," +
                                                                                                       Tools::escapeToMiraiCode(std::move(n)) +
                                                                                                       "," +
                                                                                                       std::to_string(s)),
                                                             id(i),
                                                             internalid(ii),
                                                             name(std::move(n)),
                                                             size(s),
                                                             path(std::move(p)),
                                                             dinfo(std::move(d)),
                                                             finfo(f) {}
    RemoteFile::RemoteFile(const std::string &i, unsigned int ii, std::string n, long long s) : SingleMessage(6, i + "," + std::to_string(ii) + "," +
                                                                                                                         Tools::escapeToMiraiCode(std::move(n)) +
                                                                                                                         "," +
                                                                                                                         std::to_string(s)),
                                                                                                id(i),
                                                                                                internalid(ii),
                                                                                                name(std::move(n)),
                                                                                                size(s) {}
    RemoteFile RemoteFile::deserializeFromString(const std::string &source) {
        json j;
        try {
            j = json::parse(source);
        } catch (json::parse_error &e) {
            Logger::logger.error("格式化json失败，RemoteFile::deserializeFromString");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
            throw e;
        }
        try {
            auto re = RemoteFile(j["id"], j["internalid"], j["name"], j["finfo"]["size"]);
            if (j.contains("dinfo")) {
                struct Dinfo d {
                    j["dinfo"]["url"],
                            j["dinfo"]["md5"],
                            j["dinfo"]["sha1"]
                };
                re.dinfo = d;
            }
            if (j["finfo"].contains("uploaderid")) {
                struct Finfo f {
                    j["finfo"]["size"],
                            j["finfo"]["uploaderid"],
                            j["finfo"]["expirytime"],
                            j["finfo"]["uploadtime"],
                            j["finfo"]["lastmodifytime"]
                };
                re.finfo = f;
            }
            if (j.contains("path"))
                re.path = j["path"];
            return re;
        } catch (json::type_error &e) {
            Logger::logger.error("json格式化失败，位置:RemoteFile");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
            throw e;
        }
    }
    RemoteFile RemoteFile::plus(unsigned int ii) {
        RemoteFile tmp(*this);
        tmp.internalid = ii;
        tmp.content = id + "," + std::to_string(ii) + "," + Tools::escapeToMiraiCode(std::move(name)) + "," +
                      std::to_string(size);
        return tmp;
    }

    std::string RemoteFile::serializeToString() {
        json j;
        if (this->dinfo.has_value()) {
            j["dinfo"]["url"] = this->dinfo->url;
            j["dinfo"]["md5"] = this->dinfo->md5;
            j["dinfo"]["shar1"] = this->dinfo->sha1;
        }
        if (this->finfo.has_value()) {
            j["finfo"]["size"] = this->finfo->size;
            j["finfo"]["uploaderid"] = this->finfo->uploaderid;
            j["finfo"]["expirytime"] = this->finfo->expirytime;
            j["finfo"]["uploadtime"] = this->finfo->uploadtime;
            j["finfo"]["lastmodifytime"] = this->finfo->lastmodifytime;
        }
        j["id"] = this->id;
        j["internalid"] = this->internalid;
        j["name"] = this->name;
        j["size"] = this->size;
        if (this->path.has_value())
            j["path"] = this->path.value();
        return j.dump();
    }

    /*图片类实现*/
    void Image::refreshInfo() {
        std::string re = KtOperation::ktOperation(KtOperation::QueryImgInfo, toJson());
        if (re == "E1")
            throw RemoteAssetException("图片id格式错误", MIRAICP_EXCEPTION_WHERE);
        json j = json::parse(re);
        this->url = Tools::json_stringmover(j, "url");
        this->md5 = Tools::json_stringmover(j, "md5");
        this->size = j["size"];
        this->width = j["width"];
        this->height = j["height"];
        this->imageType = j["type"];
    }

    Image Image::deserialize(const std::string &str) {
        json j = json::parse(str);
        return Image(
                j["imageid"],
                j["size"],
                j["width"],
                j["height"],
                j["type"]);
    }
    FlashImage FlashImage::deserialize(const std::string &str) {
        json j = json::parse(str);
        return FlashImage(
                j["imageid"],
                j["size"],
                j["width"],
                j["height"],
                j["type"]);
    }
} // namespace MiraiCP
