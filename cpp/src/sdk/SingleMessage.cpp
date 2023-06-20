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
#include "Exceptions/IllegalArgument.h"
#include "Exceptions/RemoteAsset.h"
#include "JsonTools.h"
#include "KtOperation.h"
#include "Logger.h"
#include "Tools.h"
#include "commonTools.h"


namespace MiraiCP {
    using json = nlohmann::json;

    // 静态成员
    const char *const *const SingleMessage::messageType = SingleMessageType::messageTypeInternal + 6;
    const char *const *const SingleMessage::miraiCodeName = SingleMessageType::miraiCodeNameInternal + 6;

    SingleMessage::SingleMessage(int inType, std::string content, std::string prefix) noexcept
        : content(std::move(content)),
          prefix(std::move(prefix)),
          internalType(inType) {}

    nlohmann::json SingleMessage::toJson() const {
        nlohmann::json re;
        re["type"] = "miraicode";
        re["content"] = this->toMiraiCode();
        return re;
    }

    int SingleMessage::getKey(const std::string &value) {
        for (auto index = Types::Begin; index != Types::End; ++index) {
            if (Tools::iequal(SingleMessage::messageType[index], value)) return index;
        }
        return Types::UnsupportedMessage_t; // default to unSupportMessage
    }

    int SingleMessage::getMiraiCodeKey(const std::string &value) {
        for (auto index = Types::Begin; index != Types::End; ++index) {
            if (Tools::iequal(SingleMessage::miraiCodeName[index], value)) return index;
        }
        return Types::UnsupportedMessage_t; // default to unSupportMessage
    }

    std::string SingleMessage::toMiraiCode() const {
        if (internalType > 0)
            if (internalType == Types::At_t)
                return "[mirai:at:" + content + "] ";
            else if (internalType == Types::AtAll_t)
                return "[mirai:atall] ";
            else
                return "[mirai:" + getTypeString(internalType) + prefix + Tools::escapeToMiraiCode(content) + "]";
        else
            return content;
    }

    bool SingleMessage::operator==(const SingleMessage &m) const {
        return this->internalType == m.internalType && this->toMiraiCode() == m.toMiraiCode();
    }

    PlainText::PlainText(std::string inStr) noexcept : SingleMessage(PlainText::type(), std::move(inStr)) {}

    PlainText::PlainText(const SingleMessage &sg) : SingleMessage(sg) {
        if (sg.internalType != type())
            throw IllegalArgumentException(
                    "Cannot convert(" + getTypeString(sg.internalType) + ") to PlainText", MIRAICP_EXCEPTION_WHERE);
        this->content = sg.content;
    }

    nlohmann::json PlainText::toJson() const {
        return {{"type", SingleMessage::messageType[this->internalType]},
                {"content", content}};
    }

    bool PlainText::operator==(const PlainText &p) const {
        return this->content == p.content;
    }

    nlohmann::json At::toJson() const {
        return {{"type", SingleMessage::messageType[this->internalType]},
                {"target", target}};
    }

    At::At(QQID a) : SingleMessage(At::type(), std::to_string(a)), target(a) {}

    At::At(const SingleMessage &sg) : SingleMessage(sg) {
        if (sg.internalType != type())
            throw IllegalArgumentException(
                    "Cannot convert(" + getTypeString(sg.internalType) + ") to At", MIRAICP_EXCEPTION_WHERE);
        this->target = std::stol(sg.content);
    }

    std::string At::toMiraiCode() const {
        return "[mirai:at:" + std::to_string(this->target) + "] "; // 后面有个空格
    }

    bool At::operator==(const At &a) const {
        return this->target == a.target;
    }

    AtAll::AtAll() : SingleMessage(AtAll::type(), "", "") {}

    nlohmann::json AtAll::toJson() const {
        return {{"type", SingleMessage::messageType[this->internalType]}};
    }

    std::string AtAll::toMiraiCode() const {
        return "[mirai:atall] ";
    }

    /*图片类实现*/
    void Image::refreshInfo() {
        std::string re = KtOperation::ktOperation(KtOperation::QueryImgInfo, toJson());
        if (re == "E1")
            throw RemoteAssetException("图片id格式错误", MIRAICP_EXCEPTION_WHERE);
        json j = json::parse(re);
        this->url = json_stringmover(j, "url");
        this->md5 = json_stringmover(j, "md5");
        this->size = j["size"];
        this->width = j["width"];
        this->height = j["height"];
        this->imageType = j["imageType"];
        this->isEmoji = j["isEmoji"];
    }

    Image Image::deserialize(const std::string &str) {
        json j = json::parse(str);
        return Image(
                j["imageId"],
                j["size"],
                j["width"],
                j["height"],
                j["imageType"],
                j["isEmoji"]);
    }

    Image::Image(const std::string &imageId, size_t size, int width, int height, std::string type, bool isEmoji)
        : SingleMessage(Image::type(), imageId), id(imageId), imageType(std::move(type)), size(size),
          width(width), height(height), isEmoji(isEmoji) {
        // todo(Antares): 实际上重复的属性 id 和 content
    }

    std::string Image::toMiraiCode() const {
        return "[mirai:image:" + this->id + "]";
    }

    bool Image::operator==(const Image &i) const {
        return this->id == i.id;
    }

    nlohmann::json Image::toJson() const {
        return {{"type", SingleMessage::messageType[this->internalType]},
                {"imageId", id},
                {"size", size},
                {"width", width},
                {"height", height},
                {"imageType", imageType},
                {"isEmoji", isEmoji}};
    }

    bool Image::isUploaded(QQID botid) {
        if (!this->md5.has_value()) this->refreshInfo();
        if (this->size == 0) throw IllegalArgumentException("size不能为0", MIRAICP_EXCEPTION_WHERE);
        std::string re = KtOperation::ktOperation(KtOperation::ImageUploaded, {{"botId", botid}, {"image", toJson()}});
        return re == "true";
    }

    FlashImage FlashImage::deserialize(const std::string &str) {
        json j = json::parse(str);
        return FlashImage(
                j["imageId"],
                j["size"],
                j["width"],
                j["height"],
                j["imageType"]);
    }

    FlashImage::FlashImage(const Image &img) {
        if (img.internalType == FlashImage::type()) {
            *this = static_cast<const FlashImage &>(img); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        } else if (img.internalType == Image::type()) {
            static_cast<Image &>(*this) = img;
        } else {
            MIRAICP_THROW(IllegalArgumentException, "拷贝构造FlashImage时传入参数无法正确转换");
        }
    }

    FlashImage::FlashImage(const std::string &imageId, size_t size, int width, int height, std::string type)
        : Image(imageId, size, width, height, std::move(type)) {
        this->SingleMessage::internalType = Types::FlashImage_t;
    }

    bool FlashImage::operator==(const FlashImage &i) const {
        return this->id == i.id;
    }

    Image FlashImage::toImage() { return Image(*this); }

    nlohmann::json FlashImage::toJson() const {
        return {{"type", SingleMessage::messageType[this->internalType]},
                {"imageId", id},
                {"size", size},
                {"width", width},
                {"height", height},
                {"imageType", imageType}};
    }

    LightApp::LightApp(std::string content) : SingleMessage(LightApp::type(), std::move(content)) {}

    nlohmann::json LightApp::toJson() const {
        return {{"type", SingleMessage::messageType[this->internalType]},
                {"content", content}};
    }

    LightApp::LightApp(const SingleMessage &sg) : SingleMessage(sg) {
        // todo(Antares): this was originally 3; why?
        if (sg.internalType != type())
            throw IllegalArgumentException(
                    "Cannot convert(" + getTypeString(sg.internalType) + ") to LighApp", MIRAICP_EXCEPTION_WHERE);
    }

    std::string LightApp::toMiraiCode() const {
        return "[mirai:app:" + Tools::escapeToMiraiCode(content) + "]";
    }

    bool LightApp::operator==(const LightApp &other) const {
        return this->content == other.content;
    }

    ServiceMessage::ServiceMessage(int id, std::string a)
        : SingleMessage(ServiceMessage::type(), std::move(a),
                        ":" + std::to_string(id) + ','),
          id(id) {}

    nlohmann::json ServiceMessage::toJson() const {
        return {{"type", SingleMessage::messageType[this->internalType]},
                {"content", content},
                {"id", id}};
    }

    std::string ServiceMessage::toMiraiCode() const {
        return "[mirai:service" + this->prefix + Tools::escapeToMiraiCode(content) + "]";
    }

    ServiceMessage::ServiceMessage(const SingleMessage &sg) : SingleMessage(sg) {
        if (sg.internalType != type())
            throw IllegalArgumentException(
                    "Cannot convert(" + getTypeString(sg.internalType) + ") to ServiceMessage", MIRAICP_EXCEPTION_WHERE);
        id = static_cast<const ServiceMessage &>(sg).id; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
    }

    ServiceMessage::ServiceMessage(const URLSharer &a)
        : SingleMessage(5,
                        R"(<?xml version="1.0" encoding="utf-8"?><msg templateID="12345" action="web" brief=")" +
                                a.brief + R"(" serviceID="1" url=")" + a.url +
                                R"("><item layout="2"><picture cover=")" +
                                a.cover + "\"/><title>" + a.title +
                                "</title><summary>" + a.summary +
                                "</summary></item><source/></msg>",
                        ":1,"),
          id(1) {}

    bool ServiceMessage::operator==(const ServiceMessage &s) const {
        return this->content == s.content;
    }

    QuoteReply::QuoteReply(MessageSource source)
        : SingleMessage(QuoteReply::type(), source.serializeToString()), source(std::move(source)) {}

    QuoteReply::QuoteReply(const SingleMessage &m) : SingleMessage(m) {
        if (m.internalType != type()) throw IllegalArgumentException("cannot convert type(" + std::to_string(m.internalType) + "to QuoteReply", MIRAICP_EXCEPTION_WHERE);
        source = MessageSource::deserializeFromString(m.content);
    }

    nlohmann::json QuoteReply::toJson() const {
        nlohmann::json re;
        re["type"] = "QuoteReply";
        re["source"] = nlohmann::json::parse(source.source);
        return re;
    }

    bool QuoteReply::operator==(const QuoteReply &qr) const {
        return this->source == qr.source;
    }

    OnlineAudio::OnlineAudio(std::string f, std::array<uint8_t, 16> md5, int size, int codec, int length, std::string url)
        : SingleMessage(OnlineAudio::type(), ""),
          filename(std::move(f)), url(std::move(url)), size(size), codec(codec),
          length(length), md5(md5) {}

    bool OnlineAudio::operator==(const OnlineAudio &oa) const {
        return this->md5 == oa.md5;
    }

    //远程文件(群文件)
    RemoteFile::RemoteFile(
            const std::string &i,
            unsigned int ii,
            std::string n,
            long long s,
            std::string p,
            struct Dinfo d,
            struct Finfo f)
        : SingleMessage(
                  RemoteFile::type(),
                  i + "," + std::to_string(ii) + "," + Tools::escapeToMiraiCode(n) + "," + std::to_string(s)),
          id(i),
          internalid(ii),
          name(std::move(n)),
          size(s),
          path(std::move(p)),
          dinfo(std::move(d)),
          finfo(f) {}

    RemoteFile::RemoteFile(
            const std::string &i,
            unsigned int ii,
            std::string n,
            long long s)
        : SingleMessage(
                  6,
                  i + "," + std::to_string(ii) + "," + Tools::escapeToMiraiCode(n) + "," + std::to_string(s)),
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
            auto re = RemoteFile(j["id"], j["internalId"], j["name"], j["detailInfo"]["size"]);
            if (j.contains("downloadInfo")) {
                auto tmp = json_jsonmover(j, "downloadInfo");
                struct Dinfo d {
                    json_jsonmover(tmp, "url"),
                            json_jsonmover(tmp, "md5"),
                            json_jsonmover(tmp, "sha1")
                };
                re.dinfo = d;
            }
            if (j["detailInfo"].contains("uploaderId")) {
                auto tmp = json_jsonmover(j, "detailInfo");
                struct Finfo f {
                    json_jsonmover(tmp, "size"),
                            json_jsonmover(tmp, "uploaderId"),
                            json_jsonmover(tmp, "expiryTime"),
                            json_jsonmover(tmp, "uploadTime"),
                            json_jsonmover(tmp, "lastModifyTime")
                };
                re.finfo = f;
            }
            if (j.contains("path"))
                re.path = json_jsonmover(j, "path");
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
        tmp.content = id + "," + std::to_string(ii) + "," + Tools::escapeToMiraiCode(name) + "," +
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

    bool RemoteFile::operator==(const RemoteFile &rf) const {
        return this->id == rf.id;
    }

    nlohmann::json Face::toJson() const {
        return {{"type", SingleMessage::messageType[this->internalType]},
                {"id", id}};
    }

    std::string Face::toMiraiCode() const {
        return "[mirai:face:" + std::to_string(id) + "]";
    }

    bool Face::operator==(const Face &f) const {
        return this->id == f.id;
    }

    MusicShare::MusicShare(std::string appName, std::string title, std::string summary, std::string jumpUrl, std::string picUrl, std::string musicUrl, std::string brief)
        : SingleMessage(MusicShare::type(), ""),
          appName(std::move(appName)),
          title(std::move(title)),
          summary(std::move(summary)),
          jumpUrl(std::move(jumpUrl)),
          picUrl(std::move(picUrl)),
          musicUrl(std::move(musicUrl)),
          brief(std::move(brief)) {}

    std::string MusicShare::toMiraiCode() const {
        return "[mirai:musicshare:" + appName + "," + title + "," + summary + "," + jumpUrl + "," + picUrl + "," + musicUrl + "," + brief + "]";
    }

    // impl: {"type":"MusicShare","kind":"NeteaseCloudMusic","title":"title","summary":"summary","jumpUrl":"jumpUrl","pictureUrl":"pictureUrl","musicUrl":"musicUrl","brief":"brief"}
    json MusicShare::toJson() const {
        return {{"type", SingleMessage::messageType[this->internalType]},
                {"kind", appName},
                {"title", title},
                {"summary", summary},
                {"jumpUrl", jumpUrl},
                {"pictureUrl", picUrl},
                {"musicUrl", musicUrl},
                {"brief", brief}};
    }
    std::string MusicShare::toString() const {
        return this->toJson().dump();
    }

    MarketFace::MarketFace(std::array<uint8_t, 16> id) : SingleMessage(MarketFace::type(), ""), faceId(id) {}

    bool MarketFace::operator==(const MarketFace &mf) const {
        return this->faceId == mf.faceId;
    }

    UnSupportMessage::UnSupportMessage(const SingleMessage &s) : SingleMessage(s) {}

    nlohmann::json UnSupportMessage::toJson() const {
        return {{"type", SingleMessage::messageType[this->internalType]},
                {"struct", content}};
    }

    bool UnSupportMessage::operator==(const UnSupportMessage &m) const {
        return this->content == m.content;
    }
} // namespace MiraiCP
