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

#ifndef MIRAICP_PRO_SINGLEMESSAGE_H
#define MIRAICP_PRO_SINGLEMESSAGE_H

#include "MiraiCPMacros.h"
#include "MessageSource.h"
#include "MiraiCode.h"
#include <array>
#include <json_fwd.hpp>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <utility>


namespace MiraiCP {
    // todo(Antares): delete these after the whole refactor is finished
#define SINGLEMESSAGE_REFACTOR_ASSERTION(x, y) static_assert((x) == (y), "static assertion failed when refactoring code")
    namespace SingleMessageType {
        // id 小于 0 的是不能直接发送的消息(仅在接收的 MessageChain 里出现)
        MIRAICP_ITERABLE_ENUM(            // NOLINT(cert-dcl21-cpp)
                -6,                       // begin at
                MessageSource_t,          // -6
                MarketFace_t,             // -5
                OnlineForwardedMessage_t, // -4
                OnlineAudio_t,            // -3
                QuoteReply_t,             // -2
                UnsupportedMessage_t,     // -1
                PlainText_t,              // 0
                At_t,                     // 1
                AtAll_t,                  // 2
                Image_t,                  // 3
                LightApp_t,               // 4
                ServiceMessage_t,         // 5
                RemoteFile_t,             // 6
                Face_t,                   // 7
                FlashImage_t,             // 8
                MusicShare_t              // 9, End = 10
        )

        constexpr const char *miraiCodeNameInternal[] = {
                "NoExists",
                "NoExists",
                "NoExists",
                "NoExists",
                "NoExists",
                "NoExists",
                "NoExists",
                "at",
                "atall",
                "image",
                "app",
                "service",
                "file",
                "face",
                "flash",
                "musicshare"
        };

        constexpr const char *messageTypeInternal[] = {
                "MessageSource",        // -6
                "MarketFace",             // -5
                "ForwardMessage", // -4
                "OnlineAudio",            // -3
                "QuoteReply",             // -2
                "UnsupportedMessage",       // -1
                "PlainText",              // 0      <-- messageType is here
                "At",                     // 1
                "AtAll",                  // 2
                "Image",                  // 3
                "LightApp",               // 4
                "SimpleServiceMessage",   // 5
                "FileMessage",            // 6
                "Face",                   // 7
                "FlashImage",             // 8
                "MusicShare",             // 9
        };

        static_assert(sizeof(messageTypeInternal) / sizeof(*messageTypeInternal) == SingleMessageType::End - SingleMessageType::Begin);
    } // namespace SingleMessageType

    /// 用serviceMessage的分享信息
    struct URLSharer {
        /// 简介 没点进来看见的样子
        std::string brief = "简介 没点进来看见的样子";
        /// 目标url
        std::string url = "目标url";
        /// 图标地址
        std::string cover = "图标地址";
        /// 标题
        std::string title = "标题";
        /// 描述文字
        std::string summary = "描述文字";
    };

    /// MessageChain的组成部分
    class MIRAICP_EXPORT SingleMessage : public MiraiCodeable {
    public:
    public:
        using Types = SingleMessageType::Type;

        std::string content;
        std::string prefix;
        /// MiraiCode类别
        /// @see SingleMessage::messageType
        int internalType = SingleMessageType::UnsupportedMessage_t;

    protected:
        /// @brief 仅限于让子类允许默认构造，以保证子类可以通过动态判断类型构造
        /// @note dev: 请勿删除
        SingleMessage() = default; // NOLINT(cppcoreguidelines-pro-type-member-init)

    public:
        static const char *const *const messageType;
        static const char *const *const miraiCodeName;

        static std::string getTypeString(int type) {
            return messageType[type];
        }

    public:
        /// @brief 构建单条
        /// @param type 消息类型 @see messageType
        /// @param content 内容
        /// @param prefix 前缀, 默认为`:`, 第二个冒号部分的内容, 目前在serviceMesage有使用
        SingleMessage(int inType, std::string content, std::string prefix = ":") noexcept
            : content(std::move(content)),
              prefix(std::move(prefix)),
              internalType(inType) {}

        virtual ~SingleMessage() noexcept = default;

    public:
        /// @brief 找对应类型的index key
        /// @param value 类型名
        /// @return 如果没找到返回-1
        static int getKey(const std::string &value);

        /**
         * 寻找对应的 MiraiCode 名称
         * @param value 名称
         * @return 如果没找到返回-1
         */
        static int getMiraiCodeKey(const std::string &value);

    public:
        [[nodiscard]] virtual nlohmann::json toJson() const;

        [[nodiscard]] std::string toMiraiCode() const override;

    public:
        bool operator==(const SingleMessage &m) const {
            return this->internalType == m.internalType && this->toMiraiCode() == m.toMiraiCode();
        }

        bool operator==(SingleMessage *m) const {
            return this->internalType == m->internalType && this->toMiraiCode() == m->toMiraiCode();
        }
    };

    /// 纯文本信息
    class MIRAICP_EXPORT PlainText : public SingleMessage {
    public:
        explicit PlainText(const SingleMessage &sg);

        PlainText(PlainText &&_o) noexcept = default;

        PlainText(const PlainText &_o) = default;

        explicit PlainText(std::string inStr) noexcept : SingleMessage(PlainText::type(), std::move(inStr)) {}

        template<typename Stringable, typename = decltype(std::to_string(std::declval<Stringable>()))>
        explicit PlainText(Stringable val) : SingleMessage(PlainText::type(), std::to_string(val)) {}

    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(0, Types::PlainText_t);
            return Types::PlainText_t;
        }

    public:
        [[nodiscard]] std::string toMiraiCode() const override {
            return content;
        }

        [[nodiscard]] nlohmann::json toJson() const override;

        bool operator==(const PlainText &p) const {
            return this->content == p.content;
        }
    };

    // todo(Antares): 除特殊情况外（子类与基类没有差异）应禁止子类使用父类对象进行构造；或者构造函数直接调用downcast（不安全）
    //  例如 At(const SingleMessage &) 这样的构造函数不应该出现

    /// @
    class MIRAICP_EXPORT At : public SingleMessage {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(1, Types::At_t);
            return Types::At_t;
        }

        QQID target;

        [[nodiscard]] nlohmann::json toJson() const override;

        explicit At(const SingleMessage &sg);

        explicit At(QQID a) : SingleMessage(At::type(), std::to_string(a)), target(a) {};

        [[nodiscard]] std::string toMiraiCode() const override {
            return "[mirai:at:" + std::to_string(this->target) + "] "; // 后面有个空格
        }

        bool operator==(const At &a) const {
            return this->target == a.target;
        }
    };

    /// @brief \@全体
    class MIRAICP_EXPORT AtAll : public SingleMessage {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(2, Types::AtAll_t);
            return Types::AtAll_t;
        }

        [[nodiscard]] std::string toMiraiCode() const override {
            return "[mirai:atall] ";
        }

        [[nodiscard]] nlohmann::json toJson() const override;

        AtAll() : SingleMessage(AtAll::type(), "", "") {}
    };

    /// 图像类声明
    class MIRAICP_EXPORT Image : public SingleMessage {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(3, Types::Image_t);
            return Types::Image_t;
        }
        //图片id，样式:` {xxx}.xx `
        std::string id;
        /// 可为空, 用`refreshInfo`获取
        std::optional<std::string> md5;
        /// 可为0, 来源:用`refreshInfo`可能可以获取或者自己填充, 是isUploaded的必须条件, 默认0
        size_t size;
        /// 可为空, 用`refreshInfo`获取
        std::optional<std::string> url;
        /// 宽度, 默认0, 单位px
        int width;
        /// 长度, 默认0, 单位px
        int height;
        bool isEmoji;
        /*!
         * @brief 图片类型
         *  - png
         *  - bmp
         *  - jpg
         *  - gif
         *  - apng
         *  - unknown
         *  默认 png
         */
        std::string imageType;

        /*!
         * @brief 图片是否已经上传(如果已经上传即表明可以直接用ImageId发送, 如果没有需要手动上传)
         * @param md5 在kotlin端会用.toByteArray()转换
         * @param size 图片大小, 不能为0
         * @param botid 所属Botid
         * @return 是否已上传
         */
        bool isUploaded(QQID botid);

    protected:
        /// @brief 仅限于让子类允许默认构造，以保证子类可以通过动态判断类型构造
        /// @note dev: 请勿删除
        Image() = default; // NOLINT(cppcoreguidelines-pro-type-member-init)

    public:
        /*!
        * @brief 从图片builder构造，适用于服务器上已经有的图片，即接收到的
        * @param imageId 图片id, 必须
        * @param size isUploaded的必要条件, 单纯用ImageId可能取不到图片size, 需要自己上传
        * @param width 宽度
        * @param height 长度
        * @param type 图片类型
        * @detail 图片miraiCode格式例子, `[mirai:image:{图片id}.jpg]`
        * 可以用这个正则表达式找出id `\\[mirai:image:(.*?)\\]`
        */
        explicit Image(const std::string &imageId, size_t size = 0, int width = 0, int height = 0,
                       std::string type = "PNG",
                       bool isEmoji = false)
                : SingleMessage(Image::type(), imageId), id(imageId), size(size), width(width),
                  height(height), isEmoji(isEmoji), imageType(std::move(type)) {
            // todo(Antares): 实际上重复的属性 id 和 content
        }

        /// 刷新信息(获取图片下载Url,md5, size)
        void refreshInfo();

        /// 取图片Mirai码
        [[nodiscard]] std::string toMiraiCode() const override {
            return "[mirai:image:" + this->id + "]";
        }
        [[nodiscard]] nlohmann::json toJson() const override;

        static Image deserialize(const std::string &);

        bool operator==(const Image &i) const {
            return this->id == i.id;
        }
    };

    /// 闪照, 和Image属性类似
    class MIRAICP_EXPORT FlashImage : public Image {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(8, Types::FlashImage_t);
            return Types::FlashImage_t;
        }

        [[nodiscard]] std::string toMiraiCode() const override {
            return "[mirai:flash:" + this->id + "]";
        }

        explicit FlashImage(const std::string &imageId, size_t size = 0, int width = 0, int height = 0,
                            std::string type = "PNG") : Image(imageId, size, width, height, type) {
            this->SingleMessage::internalType = 8;
        }

        explicit FlashImage(const Image &img);

        [[nodiscard]] nlohmann::json toJson() const override;

        static FlashImage deserialize(const std::string &);

        bool operator==(const FlashImage &i) const {
            return this->id == i.id;
        }

        /// 转换到普通图片
        Image toImage() { return Image(*this); }
    };

    /*!
    * @brief 小程序卡片
    * @attention 自带的模板不稳定，可能发出现没有效果
    * @doxygenEg{1015, lightApp.cpp, 从文本构建LightApp}
    */
    class MIRAICP_EXPORT LightApp : public SingleMessage {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(4, Types::LightApp_t);
            return Types::LightApp_t;
        }
        /// @brief 使用纯文本构造，推荐使用其他结构体方法构造
        /// @param content 构造文本
        explicit LightApp(std::string content) : SingleMessage(LightApp::type(), std::move(content)) {}

        explicit LightApp(const SingleMessage &sg);
        [[nodiscard]] nlohmann::json toJson() const override;
        /// 返回miraicode
        [[nodiscard]] std::string toMiraiCode() const override;

        bool operator==(const LightApp &la) const {
            return this->content == la.content;
        }
    };

    /// xml格式的超文本信息
    /// @attention 自带的模板不稳定，可能发出现没有效果
    class MIRAICP_EXPORT ServiceMessage : public SingleMessage {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(5, Types::ServiceMessage_t);
            return Types::ServiceMessage_t;
        }

        [[nodiscard]] nlohmann::json toJson() const override;

        [[nodiscard]] std::string toMiraiCode() const override;

        int id;

        /// @brief ServiceMessage
        /// @param id 在xml内容前面的id (不包括逗号)
        /// @param a xml内容 (不需要事先转码到miraiCode)
        explicit ServiceMessage(int id, std::string a) : SingleMessage(ServiceMessage::type(), std::move(a),
                                                                       ":" + std::to_string(id) + ','),
                                                         id(id) {}

        explicit ServiceMessage(const SingleMessage &sg);

        explicit ServiceMessage(const URLSharer &a) : SingleMessage(5,
                                                                    R"(<?xml version="1.0" encoding="utf-8"?><msg templateID="12345" action="web" brief=")" +
                                                                            a.brief + R"(" serviceID="1" url=")" + a.url +
                                                                            R"("><item layout="2"><picture cover=")" +
                                                                            a.cover + "\"/><title>" + a.title +
                                                                            "</title><summary>" + a.summary +
                                                                            "</summary></item><source/></msg>",
                                                                    ":1,"),
                                                      id(1) {}

        bool operator==(const ServiceMessage &s) const {
            return this->content == s.content;
        }
    };

    /// 引用信息
    class MIRAICP_EXPORT QuoteReply : public SingleMessage {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(-2, Types::QuoteReply_t);
            return Types::QuoteReply_t;
        }
        // 不可直接发送, 发送引用信息用MessageChain.quoteAndSendMessage
        ShouldNotUse("don't have MiraiCode, use MessageChain.quote instead") std::string toMiraiCode() const override {
            return "";
        }

        /// 引用信息的MessageSource
        MessageSource source;

        explicit QuoteReply(const SingleMessage &m);

        explicit QuoteReply(MessageSource source) : SingleMessage(QuoteReply::type(), source.serializeToString()), source(std::move(source)){};

        bool operator==(const QuoteReply &qr) const {
            return this->source == qr.source;
        }

        nlohmann::json toJson() const override;
    };

    /// 接收到的音频文件, 发送用`Contact.sendAudio`
    class MIRAICP_EXPORT OnlineAudio : public SingleMessage {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(-3, Types::OnlineAudio_t);
            return Types::OnlineAudio_t;
        }

        /// 文件名
        std::string filename;
        /// 下载地址
        std::string url;
        /// 文件大小
        int size;
        /// 编码方式
        int codec;
        /// 时长(单位s)
        int length;
        /// 16位md5
        std::array<uint8_t, 16> md5;

        /// 不支持直接发送, 用Contact.sendAudio
        ShouldNotUse("cannot use, use Contact.sendAudio") std::string toMiraiCode() const override {
            return "";
        }

        explicit OnlineAudio(std::string f, std::array<uint8_t, 16> md5, int size, int codec, int length,
                             std::string url) : SingleMessage(OnlineAudio::type(), ""),
                                                filename(std::move(f)), url(std::move(url)), size(size), codec(codec),
                                                length(length), md5(md5){};

        bool operator==(const OnlineAudio &oa) const {
            return this->md5 == oa.md5;
        }
    };

    /// @brief 远程(群)文件类型
    class MIRAICP_EXPORT RemoteFile : public SingleMessage {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(6, Types::RemoteFile_t);
            return Types::RemoteFile_t;
        }

        /// @brief 下载信息
        /// @see RemoteFile
        struct Dinfo {
            /// 下载地址, 可能会是 `null` 当文件不存在
            std::string url;
            /// md5 可用于校验
            std::string md5;
            /// sha1 可用于校验
            std::string sha1;
        };
        /// @brief 文件信息
        /// @see RemoteFile
        struct Finfo {
            /// 文件大小
            QQID size;
            /// 上传者id
            QQID uploaderid;
            /// 过期时间
            long expirytime;
            /// 上传时间, 时间戳格式
            QQID uploadtime;
            /// 上次更改时间, 时间戳格式
            QQID lastmodifytime;
        };
        /// 文件唯一id, 用于识别
        std::string id;
        /// 文件内部id, 用于构造miraiCode发送
        unsigned int internalid;
        /// 文件名
        std::string name;
        /// 文件大小
        long long size;
        /// 文件在群文件的路径
        /// @attention 可能为空(通常出现于MessageChain从MiraiCode反序列化), 需要从Group重新获取文件
        /// @see Group::getFileByFile
        std::optional<std::string> path;
        /// 文件下载信息
        /// @attention 可能为空(常出现于MessageChain从MiraiCode反序列化), 如果为空需要从Group重新获取
        /// @see MiraiCP::Dinfo, Group::getFileByFile
        std::optional<Dinfo> dinfo;
        /// 文件信息
        /// @attention 可能为空(常出现于MessageChain从MiraiCode反序列化), 如果为空需要从Group重新获取
        /// @see MiraiCP::Finfo, Group::getFileByFile
        std::optional<Finfo> finfo;

        std::string serializeToString();

        RemoteFile plus(unsigned int ii);

        static RemoteFile deserializeFromString(const std::string &source);

        /*!
         * @brief 构造远程(群)文件
         * @param i ids
         * @param ii internalids
         * @param n name
         * @param s size
         * @param p path
         * @param d dinfo
         * @param f finfo
         */
        explicit RemoteFile(const std::string &i, unsigned int ii, std::string n, long long s, std::string p, struct Dinfo d, struct Finfo f);

        /*!
         * @brief 构造远程(群)文件
         * @param i ids
         * @param ii internalids
         * @param n name
         * @param s size
         * @param p path
         * @param d dinfo
         * @param f finfo
         */
        explicit RemoteFile(const std::string &i, unsigned int ii, std::string n, long long s);

        /// 上传后会自动发送
        ShouldNotUse("Cannot send manually, use Group.sendFile") std::string toMiraiCode() const override {
            return "";
        }

        bool operator==(const RemoteFile &rf) const {
            return this->id == rf.id;
        }
    };

    /// 自带表情
    /// @attention 有些表情会变成PlainText类型和\\xxx 的格式
    class MIRAICP_EXPORT Face : public SingleMessage {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(7, Types::Face_t);
            return Types::Face_t;
        }

        /**
         * @brief 表情 id
         */
        int id;

        [[nodiscard]] nlohmann::json toJson() const override;

        [[nodiscard]] std::string toMiraiCode() const override {
            return "[mirai:face:" + std::to_string(id) + "]";
        }

        explicit Face(int id) : SingleMessage(Face::type(), std::to_string(id)), id(id) {}

        bool operator==(const Face &f) const {
            return this->id == f.id;
        }
    };

    /// 一些可以被mirai识别的音乐卡片, 如果不能被mirai识别, 那应该被表现成lightApp类型(可能收费/vip歌曲用lightApp, 免费用MusicShare)
    class MIRAICP_EXPORT MusicShare : public SingleMessage {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(9, Types::MusicShare_t);
            return Types::MusicShare_t;
        }

        /// 应用名称, 如NeteaseCloudMusic
        std::string appName;
        /// 歌名
        std::string title;
        /// 卡片第二行的文字内容
        std::string summary;
        /// 点击跳转到的链接
        std::string jumpUrl;
        /// 图片链接
        std::string picUrl;
        /// 音乐文件链接
        std::string musicUrl;
        /// 简介, 点进聊天节目前显示的小文字, 一般是`分享`
        std::string brief;
        [[nodiscard]] std::string toMiraiCode() const override {
            return "[mirai:musicshare:" + appName + "," + title + "," + summary + "," + jumpUrl + "," + picUrl + "," + musicUrl + "," + brief + "]";
        }
        MusicShare(std::string appName,
                   std::string title,
                   std::string summary,
                   std::string jumpUrl,
                   std::string picUrl,
                   std::string musicUrl,
                   std::string brief)
            : SingleMessage(MusicShare::type(), ""),
              appName(std::move(appName)),
              title(std::move(title)),
              summary(std::move(summary)),
              jumpUrl(std::move(jumpUrl)),
              picUrl(std::move(picUrl)),
              musicUrl(std::move(musicUrl)),
              brief(std::move(brief)) {}
    };

    class MIRAICP_EXPORT MarketFace : public SingleMessage {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(-5, Types::MarketFace_t);
            return Types::MarketFace_t;
        }
        /// 目前无法直接发送MarketFace, 可以转发
        ShouldNotUse("暂不支持直接发送") std::string toMiraiCode() const override {
            return "";
        }

        std::array<uint8_t, 16> faceId;

        explicit MarketFace(std::array<uint8_t, 16> id) : SingleMessage(MarketFace::type(), ""), faceId(id) {}

        bool operator==(const MarketFace &mf) const {
            return this->faceId == mf.faceId;
        }
    };

    /// @brief 目前不支持的消息类型, 不支持发送
    class MIRAICP_EXPORT UnSupportMessage : public SingleMessage {
    public:
        static int type() {
            SINGLEMESSAGE_REFACTOR_ASSERTION(-1, Types::UnsupportedMessage_t);
            return Types::UnsupportedMessage_t;
        }

        [[nodiscard]] nlohmann::json toJson() const override;
        /// 不支持发送
        ShouldNotUse("不支持直接发送UnSupportMessage") std::string toMiraiCode() const override {
            return "";
        }

        explicit UnSupportMessage(const SingleMessage &s) : SingleMessage(s){};

        explicit UnSupportMessage(const std::string &content) : SingleMessage(UnSupportMessage::type(), content) {}

        bool operator==(const UnSupportMessage &m) const {
            return this->content == m.content;
        }
    };
#undef SINGLEMESSAGE_REFACTOR_ASSERTION
} // namespace MiraiCP

#endif //MIRAICP_PRO_SINGLEMESSAGE_H
