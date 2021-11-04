#ifndef MIRAICP_PRO_MESSAGECHAIN_H
#define MIRAICP_PRO_MESSAGECHAIN_H

#include "Config.h"
#include "MessageSource.h"
#include "MiraiCode.h"
#include "SingleMessage.h"
#include "json.hpp"
#include <optional>
#include <variant>

namespace MiraiCP {
    using json = nlohmann::json;
    /// 消息链, 一般由SingleMessage组成
    class MessageChain : public MiraiCodeable {
    public:
        class Message {
        private:
            std::shared_ptr<SingleMessage> content;

        public:
            /// 代表的子类
            /// @see MessageChain::messageType
            int type() const {
                return this->content->type;
            };

            template<class T>
            explicit Message(T a) {
                static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
                content = std::make_shared<SingleMessage>(a);
            }

            explicit Message(std::shared_ptr<SingleMessage> a) {
                content = std::move(a);
            }

            std::shared_ptr<SingleMessage> getPtr() {
                return this->content;
            }

            std::shared_ptr<SingleMessage> operator->() const {
                return this->content;
            }

            template<class T>
            T get() {
                auto tmp = this->content;
                switch (tmp->type) {
                    case -1:
                        if (std::is_same_v<T, UnSupportMessage>)
                            return (UnSupportMessage) (*tmp);
                        else
                            MiraiCPThrow(IllegalArgumentException("转换错误"));
                    case 0:
                        if (std::is_same_v<T, PlainText>)
                            return (PlainText) (*tmp);
                        else
                            MiraiCPThrow(IllegalArgumentException("转换错误"));
                    case 1:
                        if (std::is_same_v<T, At>)
                            return (At) (*tmp);
                        else
                            MiraiCPThrow(IllegalArgumentException("转换错误"));
                    case 2:
                        if (std::is_same_v<T, Image>)
                            return (Image) (*tmp);
                        else
                            MiraiCPThrow(IllegalArgumentException("转换错误"));
                    case 3:
                        if (std::is_same_v<T, LightApp>)
                            return (LightApp) (*tmp);
                        else
                            MiraiCPThrow(IllegalArgumentException("转换错误"));
                    case 4:
                        if (std::is_same_v<T, ServiceMessage>)
                            return (ServiceMessage) (*tmp);
                        else
                            MiraiCPThrow(IllegalArgumentException("转换错误"));
                    default: // cannot reach
                        MiraiCPThrow(APIException(""));
                }
            }

            std::variant<PlainText, At, Image, LightApp, ServiceMessage, UnSupportMessage> get() const {
                auto tmp = this->content;
                switch (tmp->type) {
                    case -1:
                        return (UnSupportMessage) (*tmp);
                    case 0:
                        return (PlainText) (*tmp);
                    case 1:
                        return (At) (*tmp);
                    case 2:
                        return (Image) (*tmp);
                    case 3:
                        return (LightApp) (*tmp);
                    case 4:
                        return (ServiceMessage) (*tmp);
                    default:
                        MiraiCPThrow(APIException("位置MessageChain::get"));
                }
            }
        };

    private:
        void p(std::vector<Message> *v) {}

        template<class T1, class... T2>
        void p(std::vector<Message> *v, T1 h, T2... args) {
            static_assert(std::is_base_of_v<SingleMessage, T1>, "只支持SingleMessage子类");
            v->push_back(Message(h));
            p(v, args...);
        }

        template<class... T2>
        void p(std::vector<Message> *v, std::string h, T2... args) {
            v->push_back(Message(PlainText(h)));
            p(v, args...);
        }

        template<class... T2>
        void p(std::vector<Message> *v, const char *h, T2... args) {
            v->push_back(Message(PlainText(h)));
            p(v, args...);
        }

        std::vector<Message> content;

        MessageSource quoteAndSend0(const std::string &msg, QQID groupid = -1,
                                    JNIEnv *env = ThreadManager::getEnv()) {
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

        template<class T>
        MessageSource quoteAndSend1(T s, QQID groupid = -1, JNIEnv *env = ThreadManager::getEnv()) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
            return this->quoteAndSend0(s.toMiraiCode(), groupid, env);
        }

        MessageSource quoteAndSend1(std::string s, QQID groupid, JNIEnv *env) {
            return this->quoteAndSend0(s, groupid, env);
        }

        MessageSource quoteAndSend1(MessageChain mc, QQID groupid, JNIEnv *env) {
            return this->quoteAndSend0(mc.toMiraiCode(), groupid, env);
        }

    public:
        size_t size() {
            return this->content.size();
        }

        const std::vector<Message> &vector() {
            return this->content;
        }

        /// 如果由MiraiCP构造(incoming)就会存在，否则则不存在
        std::optional<MessageSource> source = std::nullopt;

        /// @brief 找到miraiCode结尾的`]`
        /// @param s 文本
        /// @param start 开始位置
        /// @return 如果不存在返回-1, 存在则返回index
        static size_t findEnd(const std::string &s, size_t start) {
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

        std::string toMiraiCode() const override {
            return Tools::VectorToString(this->toMiraiCodeVector(), "");
        }

        std::vector<std::string> toMiraiCodeVector() const {
            std::vector<std::string> tmp;
            for (const Message &a: this->content)
                tmp.emplace_back(a->toMiraiCode());
            return tmp;
        }

        /// @brief 添加元素
        /// @tparam T 任意的SingleMessage的子类
        /// @param a 添加的值
        template<class T>
        void add(const T &a) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只接受SingleMessage的子类");
            this->content.push_back(Message(a));
        }

        void add(const MessageSource &val) {
            this->source = val;
        }

        /// 筛选出某种特点type的信息
        template<class T>
        std::vector<T> filter(int type) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            std::vector<T> re;
            for (auto a: this->content) {
                if (a->type == type)
                    re.push_back(std::static_pointer_cast<T>(a));
            }
            return re;
        }

        /// 自定义筛选器
        template<class T>
        std::vector<T> filter(std::function<bool(Message)> func) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            std::vector<T> re;
            for (auto a: this->content) {
                if (func(a))
                    re.push_back(std::static_pointer_cast<T>(a));
            }
            return re;
        }

        /// 找出第一个指定的type的信息
        template<class T>
        T first(int type) {
            for (auto a: this->content)
                if (a->type == type)
                    return std::static_pointer_cast<T>(a);
        }

        /// incoming构造器
        template<class... T>
        explicit MessageChain(MessageSource ms, T... args) : source(std::move(ms)) {
            this->p(&this->content, args...);
        };

        /// outcoming构造器
        template<class... T>
        explicit MessageChain(T... args) {
            p(&this->content, args...);
        };

        /// outcoming 构造器
        template<class T>
        explicit MessageChain(const T &msg) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage子类");
            this->content.push_back(Message(msg));
        };

        template<class T>
        [[nodiscard]] MessageChain plus(const T &a) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            MessageChain tmp(*this);
            tmp.content.push_back(std::make_shared<SingleMessage>(a));
            return tmp;
        }

        MessageChain plus(const MessageSource &ms) {
            MessageChain tmp(*this);
            tmp.source = ms;
            return tmp;
        }

        template<class T>
        MessageChain operator+(const T &msg) {
            return this->plus(msg);
        }

        Message operator[](size_t i) {
            return this->content[i];
        }

        /// @brief 回复并发送
        /// @param s 内容
        /// @param groupid 如果是来源于TempGroupMessage就要提供(因为要找到那个Member)
        /// @note 可以改MessageSource里的内容, 客户端在发送的时候并不会校验MessageSource的内容正确性(比如改originalMessage来改引用的文本的内容, 或者改id来定位到其他信息)
        /// @detail 支持以下类型传入
        /// - std::string / const char* 相当于传入PlainText(str)
        /// - SingleMessage的各种派生类
        /// - MessageChain
        template<class T>
        [[deprecated("use Contact.quoteAndSend")]] MessageSource
        quoteAndSendMessage(T s, QQID groupid = -1, JNIEnv *env = ThreadManager::getEnv()) {
            return this->quoteAndSend1(s, groupid, env);
        }

        /// 从string构建MessageChain, 常用于Incoming message
        static MessageChain deserializationFromMiraiCode(const std::string &m);

        static MessageChain deserializationFromMessageSourceJson(const json &j, bool origin = true);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MESSAGECHAIN_H
