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

#ifndef MIRAICP_PRO_SINGLEMESSAGE_H
#define MIRAICP_PRO_SINGLEMESSAGE_H

#include "Exception.h"
#include "MessageSource.h"
#include <array>

namespace MiraiCP {
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
    class SingleMessage : public MiraiCodeable {
    public:
        virtual ~SingleMessage() = default;
        static std::map<int, std::string> messageType;

        /// @brief 找对应类型的index key
        /// @param value 类型名
        /// @return 如果没找到返回-1
        static int getKey(const std::string &value) {
            for (const auto &a: messageType) {
                if (Tools::iequal(a.second, value)) return a.first;
            }
            return -1;
        }

        /// MiraiCode类别
        /// @see SingleMessage::messageType
        int type;
        std::string content;
        std::string prefix;

        std::string toMiraiCode() const override {
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

        bool operator==(const SingleMessage &m) const {
            return this->type == m.type && this->toMiraiCode() == m.toMiraiCode();
        }

        bool operator==(SingleMessage *m) const {
            return this->type == m->type && this->toMiraiCode() == m->toMiraiCode();
        }

        /// @brief 构建单条
        /// @param type 消息类型 @see messageType
        /// @param content 内容
        /// @param prefix 前缀, 默认为`:`, 第二个冒号部分的内容, 目前在serviceMesage有使用
        SingleMessage(int type, std::string content, std::string prefix = ":") : type(type),
                                                                                 content(std::move(content)),
                                                                                 prefix(std::move(prefix)) {}
    };

    /// 纯文本信息
    class PlainText : public SingleMessage {
    public:
        std::string toMiraiCode() const override {
            return content;
        }

        explicit PlainText(const SingleMessage &sg) : SingleMessage(sg) {
            if (sg.type != 0)
                MiraiCPThrow(IllegalArgumentException(
                        "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to PlainText"));
            this->content = sg.content;
        }

        template<typename T>
        explicit PlainText(const T &a) : SingleMessage(0, ([&a]() -> std::string {
                                                           std::stringstream sst;
                                                           sst << a;
                                                           return sst.str();
                                                       })()) {}
    };

    /// @
    class At : public SingleMessage {
    public:
        QQID target;

        explicit At(const SingleMessage &sg) : SingleMessage(sg) {
            if (sg.type != 1)
                MiraiCPThrow(IllegalArgumentException(
                        "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to At"));
            this->target = std::stol(sg.content);
        }

        explicit At(QQID a) : SingleMessage(1, std::to_string(a)), target(a){};

        std::string toMiraiCode() const override {
            return "[mirai:at:" + std::to_string(this->target) + "] "; // 后面有个空格
        }
    };

    /// @brief \@全体
    class AtAll : public SingleMessage {
    public:
        std::string toMiraiCode() const override {
            return "[mirai:atall] ";
        }

        AtAll() : SingleMessage(2, "", "") {}
    };

    /// 图像类声明
    class Image : public SingleMessage {
    public:
        //图片id，样式:` {xxx}.xx `
        std::string id;

        /*!
        * @brief 从图片id构造，适用于服务器上已经有的图片，即接收到的
        * @example 图片miraiCode格式例子
         * @code [mirai:image:{图片id}.jpg] @endcode
        * @note 可以用这个正则表达式找出id ` \\[mirai:image:(.*?)\\] `
        */
        explicit Image(const std::string &imageId) : SingleMessage(3, imageId) {
            this->id = imageId;
        }

        explicit Image(const SingleMessage &sg) : SingleMessage(sg) {
            if (sg.type != 2) MiraiCPThrow(IllegalArgumentException("传入的SingleMessage应该是Image类型"));
            this->id = sg.content;
        }

        /*
        * 获取图片下载url
        */
        std::string queryURL(JNIEnv * = ThreadManager::getEnv());

        /// 取图片Mirai码
        std::string toMiraiCode() const override {
            return "[mirai:image:" + this->id + "]";
        }
    };

    /*!
    * @brief 小程序卡片
    * @see LightAppStyle1, LightAppStyle2, LightAppStyle3
    * @attention 自带的模板不稳定，可能发出现没有效果
    * @example 通过常量构建并发送小程序卡片
    * @code
 * Event::processor.registerEvent([](GroupMessageEvent e) {
		//修改里面的属性从而自定义
		LightAppStyle1 a = LightAppStyle1();
		LightAppStyle2 b = LightAppStyle2();
		LightAppStyle3 c = LightAppStyle3();
		e.group.SendMiraiCode(LightApp(a).toMiraiCode());
		e.group.SendMiraiCode(LightApp(b).toMiraiCode());
		e.group.SendMiraiCode(LightApp(c).toMiraiCode());
	});
    * @endcode
    * @example 通过文本构建并发送小程序卡片
    * @code
 * Event::processor.registerEvent([](GroupMessageEvent e) {
		//风格1，适合文字展示，不能交互,无大图
		//图标地址，应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
		//小标题
		string titles = "{\"title\":\"hi\", \"value\":\"test3\"}";
		//下面的按钮，但是不能按
		string buttons = "{\"name\":\"Test4\",\"action\":\"plugin\",\"actionData\":\"https://baidu.com\"}";
		string x = "{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[应用]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":"
			"{\"appName\":\"Test1\",\"appType\":4,\"appid\":1109659848,"
			"\"iconUrl\":\""+icon+"\"},"
			"\"data\":["+titles+"],"
			"\"title\":\"Test2\",\"button\":"
			"["+buttons+"],"
			"\"emphasis_keyword\":\"\"}},\"text\":\"\",\"sourceAd\":\"\"}";
		e.group.SendMiraiCode(LightApp(x).toString());
		//风格2，不能交互，有预览图
		//icon图标,应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string icon1 = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
		//预览图(大图),应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string preview1 = icon1;
		string a = "{\"config\":"
			"{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"1.0.0.103\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1110081493\",\"preview\":\""+preview1+"\",\"shareTemplateData\":{},"
			"\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"Test5\",\"title\":\"Test6\","
			"\"host\":{\"uin\":1000000,\"nick\":\"应用消息\"},"
			"\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\""+icon1+"\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
		e.group.SendMiraiCode(LightApp(a).toString());
		//风格3，可以跳转到网址，有预览图
		//跳转链接
		string url = "https://www.baidu.com";
		//icon图标,应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string icon2 = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
		//预览图(大图),应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string preview = icon2;
		string b = "{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\""+preview+"\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"Test1\",\"title\":\"Test3\",\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\""+icon+"\",\"qqdocurl\":\""+url+"\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
		e.group.SendMiraiCode(LightApp(b).toString());
		});
    * @endcode
    */
    class LightApp : public SingleMessage {
    public:
        /// @brief 使用纯文本构造，推荐使用其他结构体方法构造
        /// @param content 构造文本
        explicit LightApp(std::string content) : SingleMessage(4, std::move(content)) {}

        explicit LightApp(const SingleMessage &sg) : SingleMessage(sg) {
            if (sg.type != 3)
                MiraiCPThrow(IllegalArgumentException(
                        "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to LighApp"));
        }

        /// 返回miraicode
        std::string toMiraiCode() const override {
            return "[mirai:app:" + Tools::escapeToMiraiCode(content) + "]";
        }
    };

    /// xml格式的超文本信息
    /// @attention 自带的模板不稳定，可能发出现没有效果
    class ServiceMessage : public SingleMessage {
    public:
        std::string toMiraiCode() const override {
            return "[mirai:service:" + this->prefix + Tools::escapeToMiraiCode(content) + "]";
        }

        /// @brief ServiceMessage
        /// @param id 在xml内容前面的id (不包括逗号)
        /// @param a xml内容 (不需要事先转码到miraiCode)
        explicit ServiceMessage(int id, std::string a) : SingleMessage(5, std::move(a),
                                                                       ":" + std::to_string(id) + ',') {}

        explicit ServiceMessage(const SingleMessage &sg) : SingleMessage(sg) {
            if (sg.type != 4)
                MiraiCPThrow(IllegalArgumentException(
                        "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to ServiceMessage"));
        }

        explicit ServiceMessage(const URLSharer &a) : SingleMessage(5,
                                                                    "<?xml version=\"1.0\" encoding=\"utf-8\"?><msg templateID=\"12345\" action=\"web\" brief=\"" +
                                                                            a.brief + "\" serviceID=\"1\" url=\"" + a.url +
                                                                            "\"><item layout=\"2\"><picture cover=\"" +
                                                                            a.cover + "\"/><title>" + a.title +
                                                                            "</title><summary>" + a.summary +
                                                                            "</summary></item><source/></msg>",
                                                                    ":1,") {}
    };

    /// 引用信息, 不可直接发送, 发送引用信息用MessageChain.quoteAndSendMessage
    class QuoteReply : public SingleMessage {
    public:
        [[deprecated("cannot use, use MessageChain.quote")]] std::string toMiraiCode() const override {
            return "";
        }

        /// 引用信息的MessageSource
        MessageSource source;

        explicit QuoteReply(const SingleMessage &m) : SingleMessage(m) {
            if (m.type != -2) MiraiCPThrow(IllegalArgumentException("cannot convert type(" + std::to_string(m.type) + "to QuoteReply"));
            source = MessageSource::deserializeFromString(m.content);
        }

        explicit QuoteReply(MessageSource source) : SingleMessage(-2, source.serializeToString()), source(std::move(source)){};
    };

    /// 接收到的音频文件, 发送用`Contact.sendAudio`
    class OnlineAudio : public SingleMessage {
    public:
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

        [[deprecated("cannot use, use Contact.sendAudio")]] std::string toMiraiCode() const override {
            return "";
        }

        explicit OnlineAudio(std::string f, std::array<uint8_t, 16> md5, int size, int codec, int length,
                             std::string url) : SingleMessage(-3, ""),
                                                filename(std::move(f)), md5(md5), size(size), codec(codec),
                                                length(length), url(std::move(url)){};
    };

    /// @brief 远程(群)文件类型
    class RemoteFile : public SingleMessage {
    public:
        /// @brief 下载信息
        /// @see RemoteFile
        struct Dinfo {
            /// 下载地址
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
            /// 下载次数
            unsigned int downloadtime;
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
        std::string path;
        /// 文件下载信息
        /// @see dinfo
        Dinfo dinfo;
        /// 文件信息
        /// @see finfo
        Finfo finfo;

        std::string serializeToString();

        RemoteFile plus(unsigned int ii) {
            RemoteFile tmp(*this);
            tmp.internalid = ii;
            tmp.content = id + "," + std::to_string(ii) + "," + Tools::escapeToMiraiCode(std::move(name)) + "," +
                          std::to_string(size);
            return tmp;
        }

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
        explicit RemoteFile(const std::string &i, unsigned int ii, std::string n, long long s, std::string p,
                            struct Dinfo d, struct Finfo f) : SingleMessage(6, i + "," + std::to_string(ii) + "," +
                                                                                       Tools::escapeToMiraiCode(std::move(n)) +
                                                                                       "," +
                                                                                       std::to_string(s)),
                                                              id(i),
                                                              internalid(ii),
                                                              name(std::move(n)),
                                                              size(s),
                                                              path(std::move(p)),
                                                              dinfo(std::move(d)),
                                                              finfo(f){};

        /// 仅在上传后构建的有效, 即获取到internalid时(internalid != 0) 否则重新上传并重新获取internalid再转换
        std::string toMiraiCode() const override {
            if (internalid == 0) {
                // 重新上传
                MiraiCPThrow(RemoteAssetException("toMiraiCode error: internalid, 错误，重新上传"));
            }
            return "[mirai:file:" + id + "," + std::to_string(internalid) + "," + Tools::escapeToMiraiCode(name) + "," +
                   std::to_string(size) + "]";
        }
    };
    /// 自带表情
    /// @attention 有些表情会变成PlainText类型和\\xxx 的格式
    class Face : public SingleMessage {
    public:
        int id;

        std::string toMiraiCode() const override {
            return "[mirai:face:" + std::to_string(id) + "]";
        }

        explicit Face(int id) : SingleMessage(7, std::to_string(id)), id(id) {}
    };
    class MarketFace : public SingleMessage {
    public:
        /// 目前无法直接发送MarketFace, 可以转发
        [[deprecated("暂不支持直接发送")]] std::string toMiraiCode() {
            return "";
        }
        std::array<uint8_t, 16> faceId;

        explicit MarketFace(std::array<uint8_t, 16> id) : SingleMessage(-5, ""), faceId(id) {}
    };
    /// @brief 目前不支持的消息类型
    class UnSupportMessage : public SingleMessage {
    public:
        std::string toMiraiCode() const override {
            return content;
        }

        explicit UnSupportMessage(const SingleMessage &s) : SingleMessage(s){};

        explicit UnSupportMessage(const std::string &content) : SingleMessage(-1, content) {}
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_SINGLEMESSAGE_H
