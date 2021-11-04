#include "SingleMessage.h"
#include "Config.h"
#include <json.hpp>

namespace MiraiCP {
    // 静态成员
    std::map<int, std::string> SingleMessage::messageType = {
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
            {6, "file"}};

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
            struct Dinfo d {
                j["dinfo"]["url"],
                        j["dinfo"]["md5"],
                        j["dinfo"]["sha1"]
            };
            struct Finfo f {
                j["finfo"]["size"],
                        j["finfo"]["uploaderid"],
                        j["finfo"]["downloadtime"],
                        j["finfo"]["uploadtime"],
                        j["finfo"]["lastmodifytime"]
            };
            return RemoteFile(j["id"], j["internalid"], j["name"], j["finfo"]["size"], j["path"], d, f);
        } catch (json::type_error &e) {
            Logger::logger.error("json格式化失败，位置:RemoteFile");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
            throw e;
        }
    }

    std::string RemoteFile::serializeToString() {
        json j;
        j["dinfo"]["url"] = this->dinfo.url;
        j["dinfo"]["md5"] = this->dinfo.md5;
        j["dinfo"]["shar1"] = this->dinfo.sha1;
        j["finfo"]["size"] = this->finfo.size;
        j["finfo"]["uploaderid"] = this->finfo.uploaderid;
        j["finfo"]["downloadtime"] = this->finfo.downloadtime;
        j["finfo"]["uploadtime"] = this->finfo.uploadtime;
        j["finfo"]["lastmodifytime"] = this->finfo.lastmodifytime;
        j["id"] = this->id;
        j["internalid"] = this->internalid;
        j["name"] = this->name;
        j["size"] = this->size;
        return j.dump();
    }

    /*图片类实现*/
    std::string Image::queryURL(JNIEnv *env) {
        json j;
        j["id"] = this->id;
        std::string re = Config::koperation(Config::QueryImgUrl, j, env);
        if (re == "E1")
            MiraiCPThrow(RemoteAssetException("图片id格式错误"));
        return re;
    }
} // namespace MiraiCP