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

#include <json.hpp>

#include "Config.h"
#include "Exception.h"
#include "Logger.h"
#include "SingleMessage.h"
#include "Tools.h"

namespace MiraiCP {
    using json = nlohmann::json;
    // 静态成员
    std::map<int, std::string> SingleMessage::messageType = {
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
            {8, "FlashImage"},};

    QuoteReply::QuoteReply(const SingleMessage &m) : SingleMessage(m) {
        if (m.type != -2) MiraiCPThrow(IllegalArgumentException("cannot convert type(" + std::to_string(m.type) + "to QuoteReply"));
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
        Logger::logger.info("base");
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
            MiraiCPThrow(IllegalArgumentException(
                    "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to PlainText"));
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
            MiraiCPThrow(IllegalArgumentException(
                    "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to At"));
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
        if (sg.type != 3) MiraiCPThrow(IllegalArgumentException("传入的SingleMessage应该是Image类型"));
        this->id = sg.content;
        this->size = this->width = this->height = 0;
        this->imageType = 5;
    }
    bool Image::isUploaded(QQID botid, JNIEnv *env) {
        if (!this->md5.has_value()) this->refreshInfo();
        if (this->size == 0) MiraiCPThrow(IllegalArgumentException("size不能为0"));
        nlohmann::json tmp = this->toJson();
        tmp["botid"] = botid;
        std::string re = Config::koperation(Config::ImageUploaded, tmp, env);
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
    FlashImage::FlashImage(const SingleMessage& sg) : Image(sg) {
        if (sg.type != 8) MiraiCPThrow(IllegalArgumentException("传入的SingleMessage应该是FlashImage类型"));
        this->id = sg.content;
        this->size = this->width = this->height = 0;
        this->imageType = 5;
    }
    nlohmann::json LightApp::toJson() const {
        nlohmann::json j;
        j["key"] = "lightapp";
        j["content"] = this->content;
        return j;
    }
    LightApp::LightApp(const SingleMessage &sg) : SingleMessage(sg) {
        if (sg.type != 3)
            MiraiCPThrow(IllegalArgumentException(
                    "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to LighApp"));
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
        return "[mirai:service:" + this->prefix + Tools::escapeToMiraiCode(content) + "]";
    }
    ServiceMessage::ServiceMessage(const SingleMessage &sg) : SingleMessage(sg) {
        if (sg.type != 4)
            MiraiCPThrow(IllegalArgumentException(
                    "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to ServiceMessage"));
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
    void Image::refreshInfo(JNIEnv *env) {
        std::string re = Config::koperation(Config::QueryImgInfo, this->toJson(), env);
        if (re == "E1")
            MiraiCPThrow(RemoteAssetException("图片id格式错误"));
        json j = json::parse(re);
        this->url = j["url"];
        this->md5 = j["md5"];
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
    FlashImage FlashImage::deserialize(const std::string& str) {
        json j = json::parse(str);
        return FlashImage(
            j["imageid"],
            j["size"],
            j["width"],
            j["height"],
            j["type"]);
    }
} // namespace MiraiCP