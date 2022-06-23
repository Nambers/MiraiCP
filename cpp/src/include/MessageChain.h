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

namespace MiraiCP {
    class MessageSource; // forward declaration

    namespace internal {
        class Message : public std::shared_ptr<SingleMessage> {
        private:
            // std::shared_ptr<SingleMessage> content;

        public: // constructor
            template<class T>
            explicit Message(const T &_singleMessage) {
                static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
                reset(new T(_singleMessage));
            }

            explicit Message(std::shared_ptr<SingleMessage> msgptr) : std::shared_ptr<SingleMessage>(std::move(msgptr)) {}

        public:
            /// 代表的子类
            /// @see MessageChain::messageType
            int type() const {
                return (*this)->type;
            };

            /// 取指定类型
            /// @throw IllegalArgumentException
            template<class T>
            T get() const {
                static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
                if (T::type() != this->type())
                    throw IllegalArgumentException("cannot convert from " + SingleMessage::messageType[this->type()] + " to " + SingleMessage::messageType[T::type()], MIRAICP_EXCEPTION_WHERE);
                T *re = static_cast<T *>(std::shared_ptr<SingleMessage>::get());
                if (re == nullptr)
                    throw IllegalArgumentException("cannot convert from " + SingleMessage::messageType[this->type()] + " to " + SingleMessage::messageType[T::type()], MIRAICP_EXCEPTION_WHERE);
                return *re;
            }

            std::string toMiraiCode() const {
                return (*this)->toMiraiCode();
            }

            bool operator==(const Message &m) const {
                return (*this)->type == m->type && (*this)->toMiraiCode() == m->toMiraiCode();
            }

            bool operator!=(const Message &m) const {
                return (*this)->type != m->type || (*this)->toMiraiCode() != m->toMiraiCode();
            }
        };
    } // namespace internal

    /// 消息链, 一般由SingleMessage组成
    class MessageChain : public std::vector<internal::Message>, public MiraiCodeable {
    public: // typedefs
        using Message = internal::Message;

    public:
        /// 如果由MiraiCP构造(incoming)就会存在，否则则不存在
        std::optional<MessageSource> source = std::nullopt;

    public:
        MessageChain(const MessageChain &_o) = default;
        MessageChain(MessageChain &&_o) = default;

        /// incoming构造器
        template<class... T>
        explicit MessageChain(MessageSource ms, T... args) : source(std::move(ms)) {
            this->constructMessages(args...);
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
            constructMessages(args...);
        };

        /// outcoming 构造器
        template<class T>
        explicit MessageChain(const T &msg) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage子类");
            emplace_back(msg);
        };

    public:
        [[deprecated("MessageChain继承自std::vector<Message>，无需获取内部vector")]] const std::vector<Message> &vector() const {
            return *static_cast<const std::vector<Message> *>(this);
        }

        std::string toMiraiCode() const override;

        std::vector<std::string> toMiraiCodeVector() const {
            std::vector<std::string> tmp;
            for (auto &&a: *this)
                tmp.emplace_back(a->toMiraiCode());
            return tmp;
        }

        /// @brief 添加元素
        /// @tparam T 任意的SingleMessage的子类
        /// @param a 添加的值
        template<class T>
        void add(const T &a) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只接受SingleMessage的子类");
            emplace_back(a);
        }

        void add(const MessageSource &val) {
            source = val;
        }

        /// 筛选出某种类型的消息
        template<class T>
        std::vector<T> filter() {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            std::vector<T> re;
            for (auto &&a: *this) {
                if (a.type() == T::type())
                    re.emplace_back(a.get<T>());
            }
            return re;
        }

        /// 自定义筛选器
        template<class T>
        std::vector<T> filter(const std::function<bool(Message)> &func) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            std::vector<T> re;
            for (auto &&a: *this) {
                if (func(a))
                    re.push_back(a.get<T>());
            }
            return re;
        }

        /// 找出第一个指定的type的消息，消息可能不存在
        template<class T>
        std::optional<T> first() {
            for (auto &&a: *this)
                if (a.type() == T::type())
                    return a.get<T>();
            return std::nullopt;
        }

        template<class T>
        [[nodiscard]] MessageChain plus(const T &a) const {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            MessageChain tmp(*this);
            tmp.emplace_back(a);
            return tmp;
        }

        [[nodiscard]] MessageChain plus(const MessageChain &mc) const {
            MessageChain tmp(*this);
            tmp.insert(tmp.end(), mc.begin(), mc.end());
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

        bool operator==(const MessageChain &mc) const {
            if (size() != mc.size())
                return false;
            for (size_t i = 0; i < size(); i++) {
                if ((*this)[i] != mc[i])
                    return false;
            }
            return true;
        }

        bool operator!=(const MessageChain &mc) const {
            return !(*this == mc);
        }

        bool empty() const {
            return std::vector<Message>::empty() || toMiraiCode().empty();
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
                quoteAndSendMessage(T s, QQID groupid = -1, void *env = nullptr) = delete;

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
        void constructMessages() {}

        template<class T1, class... T2>
        void constructMessages(T1 h, T2... args) {
            static_assert(std::is_base_of_v<SingleMessage, T1>, "只支持SingleMessage子类");
            emplace_back(h);
            constructMessages(args...);
        }

        template<class... T2>
        void constructMessages(const std::string &h, T2... args) {
            emplace_back(PlainText(h));
            constructMessages(args...);
        }

        template<class... T2>
        void constructMessages(const char *h, T2... args) {
            emplace_back(PlainText(h));
            constructMessages(args...);
        }

        template<class... T>
        void constructMessages(const MessageChain &mc, T... args) {
            insert(end(), mc.begin(), mc.end());
            constructMessages(args...);
        }

        MessageSource quoteAndSend0(std::string msg, QQID groupid = -1);

        template<class T>
        MessageSource quoteAndSend1(T s, QQID groupid = -1) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
            return this->quoteAndSend0(s.toMiraiCode(), groupid);
        }

        MessageSource quoteAndSend1(std::string s, QQID groupid) {
            return this->quoteAndSend0(std::move(s), groupid);
        }

        MessageSource quoteAndSend1(const MessageChain &mc, QQID groupid) {
            return this->quoteAndSend0(mc.toMiraiCode(), groupid);
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MESSAGECHAIN_H
