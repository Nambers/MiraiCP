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

#ifndef MIRAICP_PRO_MESSAGECHAIN_H
#define MIRAICP_PRO_MESSAGECHAIN_H

#include "Exception.h"
#include "SingleMessage.h"
#include <type_traits>

namespace MiraiCP {
    class MessageSource; // forward declaration

    /// 消息链, 一般由SingleMessage组成
    class MessageChain : public MiraiCodeable {
    public: // typedefs
        class Message {
        private:
            std::shared_ptr<SingleMessage> content;

        public: // constructor
            template<class T>
            explicit Message(T &&a) {
                using U = std::remove_const_t<std::remove_reference_t<T>>;
                static_assert(std::is_base_of_v<SingleMessage, U>, "只支持SingleMessage的子类");
                content.reset(new U(std::forward<T>(a)));
            }

            explicit Message(std::shared_ptr<SingleMessage> a) {
                content = std::move(a);
            }

        public:
            /// 代表的子类
            /// @see MessageChain::messageType
            int type() const {
                return this->content->type;
            };

            /// 取指定类型
            /// @throw IllegalArgumentException
            template<class T>
            T get() const {
                static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
                if (T::type() != this->type())
                    throw IllegalArgumentException("cannot convert from " + SingleMessage::messageType[this->type()] + " to " + SingleMessage::messageType[T::type()], MIRAICP_EXCEPTION_WHERE);
                T *re = static_cast<T *>(this->content.get());
                if (re == nullptr)
                    throw IllegalArgumentException("cannot convert from " + SingleMessage::messageType[this->type()] + " to " + SingleMessage::messageType[T::type()], MIRAICP_EXCEPTION_WHERE);
                return *re;
            }

            std::string toMiraiCode() const {
                return this->content->toMiraiCode();
            }

            bool operator==(const Message &m) const {
                return this->content->type == m.content->type && this->content->toMiraiCode() == m.toMiraiCode();
            }

            bool operator!=(const Message &m) const {
                return this->content->type != m.content->type || this->content->toMiraiCode() != m.toMiraiCode();
            }
        };

    private:
        std::vector<Message> _messages;
        /// 如果由MiraiCP构造(incoming)就会存在，否则则不存在
        std::optional<MessageSource> source = std::nullopt;

    public:
        /// incoming构造器
        template<class... T>
        explicit MessageChain(MessageSource ms, T... args) : source(std::move(ms)) {
            this->p(&this->_messages, args...);
        };

        /*!
         * @brief 从多个参数构建MessageChain
         * @tparam T 多个传入参数的类型
         * 支持以下类型:
         * - std::string / const char* 相当于传入PlainText
         * - SingleMessage的派生类
         * @param args 参数本身
         */
        template<class... T>
        explicit MessageChain(T... args) {
            p(&this->_messages, args...);
        };

        /// outcoming 构造器
        template<class T>
        explicit MessageChain(const T &msg) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage子类");
            this->_messages.emplace_back(msg);
        };

    public:
        size_t size() {
            return this->_messages.size();
        }

        const std::vector<Message> &vector() {
            return this->_messages;
        }

        std::string toMiraiCode() const override;

        std::vector<std::string> toMiraiCodeVector() const {
            std::vector<std::string> tmp;
            for (const Message &a: this->_messages)
                tmp.emplace_back(a.toMiraiCode());
            return tmp;
        }

        /// @brief 添加元素
        /// @tparam T 任意的SingleMessage的子类
        /// @param a 添加的值
        template<class T>
        void add(const T &a) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只接受SingleMessage的子类");
            this->_messages.emplace_back(a);
        }

        void add(const MessageSource &val) {
            this->source = val;
        }

        /// 筛选出某种类型的消息
        template<class T>
        std::vector<T> filter() {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            std::vector<T> re;
            for (auto &&a: this->_messages) {
                if (a.type() == T::type())
                    re.push_back(a.get<T>());
            }
            return re;
        }

        /// 自定义筛选器
        template<class T>
        std::vector<T> filter(const std::function<bool(Message)> &func) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            std::vector<T> re;
            for (auto &&a: this->_messages) {
                if (func(a))
                    re.push_back(a.get<T>());
            }
            return re;
        }

        /// 找出第一个指定的type的消息，消息可能不存在
        template<class T>
        std::optional<T> first() {
            for (auto &&a: this->_messages)
                if (a.type() == T::type())
                    return a.get<T>();
            return std::nullopt;
        }


        template<class T>
        [[nodiscard]] MessageChain plus(const T &a) const {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            MessageChain tmp(*this);
            tmp._messages.push_back(a);
            return tmp;
        }

        [[nodiscard]] MessageChain plus(const MessageChain &mc) const {
            MessageChain tmp(*this);
            tmp._messages.insert(tmp._messages.end(), mc._messages.begin(), mc._messages.end());
            return tmp;
        }

        [[nodiscard]] MessageChain plus(const MessageSource &ms) const {
            MessageChain tmp(*this);
            tmp.source = ms;
            return tmp;
        }

        template<class T>
        MessageChain operator+(const T &msg) const {
            return this->plus(msg);
        }

        Message operator[](size_t i) const {
            return this->_messages[i];
        }

        bool operator==(const MessageChain &mc) const {
            if (this->_messages.size() != mc._messages.size())
                return false;
            for (size_t i = 0; i < this->_messages.size(); i++) {
                if (this->_messages[i] != mc[i])
                    return false;
            }
            return true;
        }

        bool operator!=(const MessageChain &mc) const {
            return !(*this == mc);
        }

        bool empty() const {
            if (this->_messages.empty() || toMiraiCode().empty())
                return true;
            return false;
        }

        /// @brief 回复并发送
        /// @param s 内容
        /// @param groupid 如果是来源于TempGroupMessage就要提供(因为要找到那个Member)
        /// @note 可以改MessageSource里的内容, 客户端在发送的时候并不会校验MessageSource的内容正确性(比如改originalMessage来改引用的文本的内容, 或者改id来定位到其他信息)
        /// @detail 支持以下类型传入
        /// - std::string / const char* 相当于传入PlainText(str)
        /// - SingleMessage的各种派生类
        /// - MessageChain
        /// @deprecated use Contact.quoteAndSend or `this->quoteAndSend1(s, groupid, env)`, since v2.8.1
        template<class T>
        ShouldNotUse("use Contact.quoteAndSend") MessageSource
                quoteAndSendMessage(T s, QQID groupid = -1, JNIEnv *env = nullptr) = delete;

    public: // static functions
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

        /// 从miraicode string构建MessageChain
        static MessageChain deserializationFromMiraiCode(const std::string &m);

        static MessageChain deserializationFromMessageSourceJson(const std::string &msg, bool origin = true) {
            return deserializationFromMessageSourceJson(nlohmann::json::parse(msg), origin);
        }
        /// 从MessageSource json中构建MessageChain, 常用于Incoming message
        /// @attention 本方法并不会自动附加MessageSource到MessageChain, 需要用.plus方法自行附加
        static MessageChain deserializationFromMessageSourceJson(const nlohmann::json &j, bool origin = true);

    private: // private methods
        void p(std::vector<Message> *) {}

        template<class T1, class... T2>
        void p(std::vector<Message> *v, T1 h, T2... args) {
            static_assert(std::is_base_of_v<SingleMessage, T1>, "只支持SingleMessage子类");
            v->emplace_back(h);
            p(v, args...);
        }

        template<class... T2>
        void p(std::vector<Message> *v, std::string h, T2... args) {
            v->emplace_back(PlainText(h));
            p(v, args...);
        }

        template<class... T2>
        void p(std::vector<Message> *v, const char *h, T2... args) {
            v->emplace_back(PlainText(h));
            p(v, args...);
        }

        template<class... T>
        void p(std::vector<Message> *v, MessageChain mc, T... args) {
            v->insert(v->end(), mc._messages.begin(), mc._messages.end());
            p(v, args...);
        }

        MessageSource quoteAndSend0(const std::string &msg, QQID groupid = -1,
                                    JNIEnv *env = nullptr);

        template<class T>
        MessageSource quoteAndSend1(T s, QQID groupid = -1, JNIEnv *env = nullptr) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
            return this->quoteAndSend0(s.toMiraiCode(), groupid, env);
        }

        MessageSource quoteAndSend1(std::string s, QQID groupid, JNIEnv *env) {
            return this->quoteAndSend0(s, groupid, env);
        }

        MessageSource quoteAndSend1(MessageChain mc, QQID groupid, JNIEnv *env) {
            return this->quoteAndSend0(mc.toMiraiCode(), groupid, env);
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MESSAGECHAIN_H
