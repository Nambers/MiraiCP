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

#include "SingleMessage.h"
#include "Config.h"
#include <json.hpp>

namespace MiraiCP {
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
            {7, "face"}};

    // 结束静态成员

    //远程文件(群文件)
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
    std::string Image::getInfo0(int type, JNIEnv *env) {
        json j;
        j["type"] = type;
        j["id"] = this->id;
        std::string re = Config::koperation(Config::QueryImgInfo, j, env);
        if (re == "E1")
            MiraiCPThrow(RemoteAssetException("图片id格式错误"));
        return re;
    }

    bool Image::isUploaded(const std::string &md5, size_t size, Bot bot, JNIEnv *env) {
        json j;
        j["md5"] = md5;
        j["size"] = size;
        j["botid"] = bot.id;
        std::string re = Config::koperation(Config::ImageUploaded, j, env);
        return re == "True";
    }
} // namespace MiraiCP