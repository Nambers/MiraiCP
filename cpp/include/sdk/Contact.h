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

#ifndef MIRAICP_PRO_CONTACT_H
#define MIRAICP_PRO_CONTACT_H


#include "MessageChain.h"
#include <nlohmann/json.hpp>
#include <string>


namespace MiraiCP {
    /// Contact 类型
    enum ContactType {
        MIRAI_CONTACT = 0,
        /// 好友
        MIRAI_FRIEND = 1,
        /// 群聊
        MIRAI_GROUP = 2,
        /// 群成员
        MIRAI_MEMBER = 3,
        /// bot
        MIRAI_BOT = 4,
        MIRAI_OTHERTYPE = 5,
    };

    struct IMiraiData;
    struct IContactData;

    namespace internal {
        std::string getNickFromIContactPtr(IContactData *);
        std::string getAvatarUrlFromIContactPtr(IContactData *);
    } // namespace internal

    /*!
    * @brief group, friend, member的父类
    * @doxygenEg{1002, message.cpp, 发送以及回复群消息}
    */
    class MIRAICP_EXPORT Contact {
        template<typename ClassType, typename InternalDataType>
        friend struct ContactDataHelper;
        // attrs
    protected:
        /// 内部数据
        std::shared_ptr<IContactData> InternalData;

    public:
        /*!
         * @brief 无参初始化Contact类型
         * @internal 禁止使用
         */
        Contact() = delete;

    protected:
        explicit Contact(std::shared_ptr<IContactData> Data);

    public:
        /// 虚类 destructor
        virtual ~Contact() = default;

        bool operator==(const Contact &c) const;

        /**
         * @brief 设置内部数据指针
         * @note dev: 避免直接使用 InternalData, 请使用该接口操作 InternalData 指针
         */
        void SetInternalData(std::shared_ptr<IContactData> Data);

        /**
         * @brief 尝试一次数据刷新
         * @note 不保证进行刷新, 一般为内部调用. 强制刷新请使用 forceRefreshInfo()
         * @see forceRefreshInfo
         */
        void refreshInfo();

        /**
         * @brief 强制下次 refreshInfo() 调用时刷新数据
         * @note 尽可能调用该函数, 避免不必要的刷新
         * @see refreshInfo
         */
        void forceRefreshNextTime();

        /**
         * @brief 强制数据刷新
         * @note 频繁刷新可能会有性能损耗
         * @see refreshInfo, forceRefreshNextTime
         */
        void forceRefreshNow();

        /**
         * @brief 当前对象类型
         * @see ContactType
         * @note dev: 不会修改, 不需要锁
         *     - ContactType::MIRAI_FRIEND 好友
         *     - ContactType::MIRAI_GROUP 群聊
         *     - ContactType::MIRAI_MEMBER 群成员
         */
        [[nodiscard]] ContactType type() const;

        /**
         * @brief id 在全部情况存在
         * @note dev: 不会修改，不需要锁
         *      - 当前type为Friend时，为好友id
         *      - 当前type为Group时，为群id
         *      - 当前type为Member时，为群成员id
         * @see MIRAI_CONTACT
         */
        [[nodiscard]] QQID id() const;

        /**
         * 所属bot
         * @note dev: 不会修改，不需要锁
         */
        [[nodiscard]] QQID botid() const;

        /**
         * @brief 回复并发送
         * @param s 内容
         * @detail 支持以下类型传入
         * - std::string / const char* 相当于传入PlainText(str)
         * - SingleMessage的各种派生类
         * - MessageChain
         * @param ms 回复的信息的MessageSource
         * @note 可以改MessageSource里的内容, 客户端在发送的时候并不会校验MessageSource的内容正确性(比如改originalMessage来改引用的文本的内容, 或者改id来定位到其他信息)
         */
        template<typename T>
        MessageSource quoteAndSendMessage(const T &s, MessageSource ms) const {
            return unpackQuote(s, ms);
        }

        /**
         * @brief 回复并发送
         * @param val 内容
         * @note 可以改MessageSource里的内容, 客户端在发送的时候并不会校验MessageSource的内容正确性(比如改originalMessage来改引用的文本的内容, 或者改id来定位到其他信息)
         * @detail 支持以下类型传入
         * - std::string / const char* 相当于传入PlainText(str)
         * - SingleMessage的各种派生类
         * - MessageChain
         */
        template<typename... T>
        MessageSource quoteAndSendMessage(const MessageSource &ms, T &&...val) const {
            return unpackQuote(MessageChain(std::forward<T>(val)...), ms);
        }

        /**
         * @brief 发送信息
         * @tparam T 类型
         * 支持:
         * - SingleMessage的派生类
         * - MessageChain
         * - std::string / const char* 相当于发送PlainText()
         * @param msg 内容
         * @return MessageSource
         */
        template<typename... T>
        MessageSource sendMessage(T &&...msg) const {
            return unpackMsg(MessageChain(std::forward<T>(msg)...));
        }

        /**
         * @brief 发送一条Message
         * @detail 支持
         *  - std::string: 相当于发送PlainText(str)
         *  - MiraiCode 相当于发送反序列化MiraiCode后的
         *  - 各种SingleMessage的派生类
         *  - MessageChain
         *  @param msg Message
         *  @return MessageSource
         */
        template<typename T>
        MessageSource sendMessage(T &&msg) const {
            return unpackMsg(MessageChain(std::forward<T>(msg)));
        }

    public: // serialization
        /// 序列化到json对象
        [[nodiscard]] nlohmann::json toJson() const;

        /// 将数据序列化进已有的json对象, 覆盖原有数据
        void updateJson(nlohmann::json &j) const;

        /**
         * 序列化成文本, 可以通过deserializationFromString反序列化, 利于保存
         * @see Contact::fromString()
         */
        [[nodiscard]] std::string toString() const;

        /// 反序列化成Contact智能指针
        /// @param source 序列化后的文本
        static std::shared_ptr<Contact> deserializeToPointer(nlohmann::json source);

        // for derived class
        //        template<typename T>
        //        static T deserialize(nlohmann::json source) {
        //            static_assert(std::is_base_of_v<Contact, T>, "Cannot deserialize class that isn't base on Contact");
        //            return T(std::move(source));
        //        }

        /**
        * @brief 上传本地图片，务必要用绝对路径
        * 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
        * @attention 最大支持图片大小为30MB
        * @throws
        * -可能抛出UploadException异常代表路径无效或大小大于30MB
        * -可能抛出MemberException找不到群或群成员
        */
        [[nodiscard]] Image uploadImg(const std::string &path) const;

        [[nodiscard]] FlashImage uploadFlashImg(const std::string &path) const;

    protected:
        [[nodiscard]] MessageSource quoteAndSendImpl(std::string msg, const MessageSource &ms) const;

        [[nodiscard]] MessageSource unpackQuote(const SingleMessage &s, const MessageSource &ms) const;
        [[nodiscard]] MessageSource unpackQuote(const std::string &s, const MessageSource &ms) const;
        [[nodiscard]] MessageSource unpackQuote(const MessageChain &mc, const MessageSource &ms) const;

        /// 发送语音
        [[nodiscard]] MessageSource sendVoiceImpl(std::string path) const;

        /// 发送纯文本信息
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        [[nodiscard]] MessageSource sendMsgImpl(std::string msg) const;

        [[nodiscard]] MessageSource unpackMsg(const MessageChain &msg) const;
        [[nodiscard]] MessageSource unpackMsg(const MiraiCodeable &msg) const;
        [[nodiscard]] MessageSource unpackMsg(std::string msg) const;
    };

    /// @brief Contact类型的数据接口模板类
    /// @note dev: 任何操作内部数据的行为在此处定义。Contact只处理 InternalData 这个 shared_ptr 本身
    template<typename ClassType, typename InternalDataType>
    struct ContactDataHelper {
        typedef InternalDataType DataType;
        using json = nlohmann::json;

    public:
        /// 群名称，群成员群名片，或好友昵称
        [[nodiscard]] std::string nickOrNameCard() const {
            auto clz_ptr = static_cast<const ClassType *>(this);
            return internal::getNickFromIContactPtr(clz_ptr->InternalData.get());
        };

        /// 头像url地址
        [[nodiscard]] std::string avatarUrl() const {
            auto clz_ptr = static_cast<const ClassType *>(this);
            return internal::getNickFromIContactPtr(clz_ptr->InternalData.get());
        };
    };

    class MIRAICP_EXPORT INudgeSupport {
    public:
        virtual ~INudgeSupport() = default;
        /*!
         * @brief 发送戳一戳
         * @warning 仅限Friend, Member类调用
         * @see MiraiCP::Friend::sendNudge, MiraiCP::Member::sendNudge
         * @throw MiraiCP::BotException, MiraiCP::IllegalStateException
         */
        virtual void sendNudge() = 0;
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_CONTACT_H
