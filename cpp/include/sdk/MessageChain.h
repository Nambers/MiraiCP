// Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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

#include "MiraiCPMacros.h"
// -----------------------
#include "Exception.h"
#include "SingleMessage.h"
#include "commonTools.h"


namespace MiraiCP {
    class MessageSource; // forward declaration

    namespace internal {
        /// @brief 为 std::shared_ptr<SingleMessage> 增加功能的封装类
        /// @note dev: 不可加入其他成员变量
        class Message : public std::shared_ptr<SingleMessage> {
            typedef std::shared_ptr<SingleMessage> Super;

        public:
#if MIRAICP_MSVC
            Message() {} // for MSVC compatible, Message should be default constructable. See MessageChain
#endif

            template<typename T, typename = std::enable_if<std::is_base_of_v<SingleMessage, T>>>
            explicit Message(T msg) {
                reset(new T(std::move(msg)));
            }

            explicit Message(Super msgptr) noexcept;

            // dev: DON'T write copy and move constructors here, otherwise add operator= overloads. See MessageChain
        public:
            /// 代表的子类
            /// @see MessageChain::messageType
            [[nodiscard]] int getType() const;

            /// 取指定类型
            /// @throw IllegalArgumentException
            template<class T>
            T getVal() const {
                // for dev: 不用 get 为了不和shared_ptr重叠
                static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
                if (T::type() != getType())
                    throw IllegalArgumentException(
                            "cannot convert from " + SingleMessage::getTypeString(getType()) + " to " +
                                    SingleMessage::getTypeString(T::type()),
                            MIRAICP_EXCEPTION_WHERE);
                T *re = static_cast<T *>(std::shared_ptr<SingleMessage>::get());
                if (re == nullptr)
                    throw IllegalArgumentException(
                            "cannot convert from " + SingleMessage::getTypeString(getType()) + " to " +
                                    SingleMessage::getTypeString(T::type()),
                            MIRAICP_EXCEPTION_WHERE);
                return *re;
            }

            [[nodiscard]] std::string toMiraiCode() const;

            [[nodiscard]] std::string toJson() const;

            bool operator==(const Message &m) const;
            bool operator!=(const Message &m) const;
        };
    } // namespace internal

    /// 消息链, 一般由SingleMessage组成
    class MIRAICP_EXPORT MessageChain : public std::vector<internal::Message>, public MiraiCodeable {
    public: // typedefs
        using Message = internal::Message;

    public:
        /// 如果由MiraiCP构造(incoming)就会存在，否则则不存在
        std::optional<MessageSource> source = std::nullopt;

    public:
        MessageChain(const MessageChain &_o) = default;
        MessageChain(MessageChain &&_o) = default;

        MessageChain() = default;

        ~MessageChain() override = default;

        /*!
         * @brief 从多个参数构建MessageChain
         * @tparam T 多个传入参数的类型
         * 支持以下类型:
         * - std::string / const char* 相当于传入PlainText
         * - SingleMessage的派生类
         * @param args 参数本身
         */
        template<typename... T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>..., MessageChain>>>
        explicit MessageChain(T &&...args) {
            constructMessages(std::forward<T>(args)...);
        };

        /// incoming构造器，对第一个参数为MessageSource的特化
        template<class... T>
        explicit MessageChain(MessageSource ms, T &&...args) : source(std::move(ms)) {
            this->constructMessages(std::forward<T>(args)...);
        };

    public:
        [[deprecated("MessageChain继承自std::vector<Message>，无需获取内部vector")]] const std::vector<Message> &vector() const {
            return static_cast<const std::vector<Message> &>(*this);
        }

        [[nodiscard]] std::string toMiraiCode() const override;

        [[nodiscard]] std::vector<std::string> toMiraiCodeVector() const;

        [[nodiscard]] nlohmann::json toJson() const;
        /**
         * @ensure toJson().dump()
         * @return MessageChain serialize to String
         */
        [[nodiscard]] std::string toString() const;

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
                if (a.getType() == T::type())
                    re.emplace_back(a.getVal<T>());
            }
            return re;
        }

        /// 自定义筛选器
        template<class T>
        std::vector<T> filter(const std::function<bool(const Message &)> &func) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            std::vector<T> re;
            for (auto &&a: *this) {
                if (func(a))
                    re.push_back(a.getVal<T>());
            }
            return re;
        }

        /// 找出第一个指定的type的消息，消息可能不存在
        template<class T>
        std::optional<T> first() {
            for (auto &&a: *this)
                if (a.getType() == T::type())
                    return a.getVal<T>();
            return std::nullopt;
        }

        template<class T>
        [[nodiscard]] MessageChain plus(const T &a) const {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            MessageChain tmp(*this);
            tmp.emplace_back(a);
            return tmp;
        }

        [[nodiscard]] MessageChain plus(const MessageChain &mc) const;

        [[nodiscard]] MessageChain plus(const MessageSource &ms) const;

        template<class T>
        MessageChain operator+(const T &msg) const {
            return this->plus(msg);
        }

        bool operator==(const MessageChain &mc) const;

        bool operator!=(const MessageChain &mc) const;

        [[nodiscard]] bool empty() const;

        /// @brief 回复并发送
        /// @param s 内容
        /// @param groupid 如果是来源于TempGroupMessage就要提供(因为要找到那个Member)
        /// @note 可以改MessageSource里的内容, 客户端在发送的时候并不会校验MessageSource的内容正确性(比如改originalMessage来改引用的文本的内容, 或者改id来定位到其他信息)
        /// @detail 支持以下类型传入
        /// - std::string / const char* 相当于传入PlainText(str)
        /// - SingleMessage的各种派生类
        /// - MessageChain
        /// @deprecated use Contact.quoteAndSend or `this->quoteAndSend1(s, groupId, env)`, since v2.8.1
        template<class T>
        ShouldNotUse("use Contact.quoteAndSend") MessageSource
                quoteAndSendMessage(T s, QQID groupid = -1, void *env = nullptr) = delete;

    public: // static functions
        /// @brief 找到miraiCode结尾的`]`
        /// @param s 文本
        /// @param start 开始位置
        /// @return 如果不存在返回-1, 存在则返回index
        static size_t findEnd(const std::string &s, size_t start);

        /// 从miraicode string构建MessageChain
        static MessageChain deserializationFromMiraiCode(const std::string &m);

        static MessageChain deserializationFromMessageSourceJson(const std::string &msg, bool origin = true) {
            return deserializationFromMessageSourceJson(nlohmann::json::parse(msg), origin);
        }

        /// 从MessageSource json中构建MessageChain, 常用于Incoming message
        /// @attention 本方法并不会自动附加MessageSource到MessageChain, 需要用.plus方法自行附加
        static MessageChain deserializationFromMessageSourceJson(nlohmann::json j, bool origin = true);

        /// 从 Message json中构建MessageChain, 常用于Incoming message
        /// @attention 本方法并不会自动附加MessageSource到MessageChain, 需要用.plus方法自行附加
        static MessageChain deserializationFromMessageJson(const nlohmann::json &j);

    private: // private methods
        void constructMessages() {}

        template<typename T1, typename ... T2, typename = std::enable_if_t<std::is_base_of_v<SingleMessage, std::decay_t<T1>>>>
        void constructMessages(T1 &&h, T2 &&...args) {
            emplace_back(std::forward<T1>(h));
            constructMessages(std::forward<T2>(args)...);
        }

        template<typename... T2>
        void constructMessages(const std::string &h, T2 &&...args) {
            emplace_back(PlainText(h));
            constructMessages(std::forward<T2>(args)...);
        }

        template<typename... T>
        void constructMessages(const MessageChain &mc, T &&...args) {
            insert(end(), mc.begin(), mc.end());
            constructMessages(std::forward<T>(args)...);
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MESSAGECHAIN_H
