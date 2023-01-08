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

#ifndef MIRAICP_PRO_CONTACT_H
#define MIRAICP_PRO_CONTACT_H

#include "IMiraiData.h"
#include "KtOperation.h"
#include "LowLevelAPI.h"
#include "MessageChain.h"
#include <json_fwd.hpp>
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

    /// Contact 内部储存数据抽象类
    struct IContactData : public IMiraiData {
        /// 昵称或者群名片
        std::string _nickOrNameCard;
        /// 头像 url
        std::string _avatarUrl;
        /// id
        QQID _id{};
        /// 所属的 bot 的 id
        QQID _botId{};
        /// Contact 类型
        ContactType _type = MIRAI_CONTACT;

        void deserialize(nlohmann::json inJson) override;

        nlohmann::json internalToJson() const override;

        /// 刷新数据
        void refreshInfo() override;

        /**
         * 更新传入 json 中的数据, 在调用后, 原 json 数据会被更新
         * @param jsonToUpdate 需更新的 json 数据
         * @see nlohmann::update
         */
        void updateJson(nlohmann::json &jsonToUpdate) const;
    };

    struct GroupRelatedData : public IContactData {
        typedef IContactData Super;
        /// group id
        QQID _groupId;

        nlohmann::json internalToJson() const override;

        explicit GroupRelatedData(QQID in_groupid) : _groupId(in_groupid) {}
    };

    /*!
    * @brief group, friend, member的父类
    * @doxygenEg{1002, message.cpp, 发送以及回复群消息}
    */
    class Contact {
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
        explicit Contact(std::shared_ptr<IContactData> Data) {
            SetInternalData(std::move(Data));
        }

    public:
        /// 虚类 destructor
        virtual ~Contact() = default;

        bool operator==(const Contact &c) const {
            return this->id() == c.id();
        }

        /**
         * @brief 设置内部数据指针
         * @note dev: 避免直接使用 InternalData, 请使用该接口操作 InternalData 指针
         */
        void SetInternalData(std::shared_ptr<IContactData> Data) { InternalData = std::move(Data); }

        /**
         * @brief 尝试一次数据刷新
         * @note 不保证进行刷新, 一般为内部调用. 强制刷新请使用 forceRefreshInfo()
         * @see forceRefreshInfo
         */
        void refreshInfo() {
            InternalData->requestRefresh();
        }

        /**
         * @brief 强制下次 refreshInfo() 调用时刷新数据
         * @note 尽可能调用该函数, 避免不必要的刷新
         * @see refreshInfo
         */
        void forceRefreshNextTime() {
            InternalData->forceRefreshNextTime();
        }

        /**
         * @brief 强制数据刷新
         * @note 频繁刷新可能会有性能损耗
         * @see refreshInfo, forceRefreshNextTime
         */
        void forceRefreshNow() {
            forceRefreshNextTime();
            refreshInfo();
        }

        /**
         * @brief 当前对象类型
         * @see ContactType
         * @note dev: 不会修改, 不需要锁
         *     - ContactType::MIRAI_FRIEND 好友
         *     - ContactType::MIRAI_GROUP 群聊
         *     - ContactType::MIRAI_MEMBER 群成员
         */
        ContactType type() const { return InternalData->_type; }

        /**
         * @brief id 在全部情况存在
         * @note dev: 不会修改，不需要锁
         *      - 当前type为Friend时，为好友id
         *      - 当前type为Group时，为群id
         *      - 当前type为Member时，为群成员id
         * @see MIRAI_CONTACT
         */
        QQID id() const { return InternalData->_id; }

        /**
         * 所属bot
         * @note dev: 不会修改，不需要锁
         */
        QQID botid() const { return InternalData->_botId; };

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
        MessageSource quoteAndSendMessage(const T &s, MessageSource ms) {
            return this->quoteAndSend1(s, ms);
        }

        /**
         * @brief 回复并发送
         * @param s 内容
         * @param groupid 如果是来源于TempGroupMessage就要提供(因为要找到那个Member)
         * @note 可以改MessageSource里的内容, 客户端在发送的时候并不会校验MessageSource的内容正确性(比如改originalMessage来改引用的文本的内容, 或者改id来定位到其他信息)
         * @detail 支持以下类型传入
         * - std::string / const char* 相当于传入PlainText(str)
         * - SingleMessage的各种派生类
         * - MessageChain
         */
        template<typename... T>
        MessageSource quoteAndSendMessage(const MessageSource &ms, T &&...val) {
            return this->quoteAndSend1(MessageChain(std::forward<T>(val)...), ms);
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
        MessageSource sendMessage(T &&...msg) {
            return this->sendMessage(MessageChain(std::forward<T>(msg)...));
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
        MessageSource sendMessage(T &&msg) {
            return this->unpackMsg(std::forward<T>(msg));
        }

    private: // private methods
        MessageSource quoteAndSend0(std::string msg, const MessageSource &ms);

        MessageSource quoteAndSend1(const SingleMessage &s, const MessageSource &ms) {
            return this->quoteAndSend0(MessageChain(s).toJson(), ms);
        }

        MessageSource quoteAndSend1(const std::string &s, const MessageSource &ms) {
            return this->quoteAndSend0(s, ms);
        }

        MessageSource quoteAndSend1(const MessageChain &mc, const MessageSource &ms) {
            return this->quoteAndSend0(mc.toJson(), ms);
        }

    public: // serialization
        /// 序列化到json对象
        nlohmann::json toJson() const { return InternalData->toJson(); }

        /// 将数据序列化进已有的json对象, 覆盖原有数据
        void updateJson(nlohmann::json &j) const { InternalData->updateJson(j); }

        /**
         * 序列化成文本, 可以通过deserializationFromString反序列化, 利于保存
         * @see Contact::fromString()
         */
        std::string toString() const {
            return toJson().dump();
        }

        /// 反序列化成Contact智能指针
        /// @param source 序列化后的文本
        static std::shared_ptr<Contact> deserializeToPointer(nlohmann::json source);

        // for derived class
        template<class T>
        static T deserialize(nlohmann::json source) {
            static_assert(std::is_base_of_v<Contact, T>, "Cannot deserialize class that isn't base on Contact");
            return T(std::move(source));
        }

        /**
        * @brief 上传本地图片，务必要用绝对路径
        * 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
        * @attention 最大支持图片大小为30MB
        * @throws
        * -可能抛出UploadException异常代表路径无效或大小大于30MB
        * -可能抛出MemberException找不到群或群成员
        */
        Image uploadImg(const std::string &path) const;

        FlashImage uploadFlashImg(const std::string &path) const;

    protected:
        /// 发送语音
        MessageSource sendVoiceImpl(std::string path) const;

        /// 发送纯文本信息
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        MessageSource sendMsgImpl(std::string msg) const;

        MessageSource unpackMsg(const MessageChain &msg) const {
            return sendMsgImpl(msg.toJson());
        }
        MessageSource unpackMsg(const MiraiCodeable &msg) const {
            return sendMsgImpl(MessageChain::deserializationFromMiraiCode(msg.toMiraiCode()).toJson());
        }
        MessageSource unpackMsg(const SingleMessage &msg) const {
            return sendMsgImpl(MessageChain(msg).toJson());
        }
        MessageSource unpackMsg(std::string msg) const {
            return sendMsgImpl(MessageChain(PlainText(std::move(msg))).toJson());
        }
        MessageSource unpackMsg(const char *msg) const {
            return sendMsgImpl(MessageChain(PlainText(msg)).toJson());
        }
    };

    /// @brief Contact类型的数据接口模板类
    /// @note dev: 任何操作内部数据的行为在此处定义。Contact只处理 InternalData 这个 shared_ptr 本身
    template<typename ClassType, typename InternalDataType>
    struct ContactDataHelper {
        typedef InternalDataType DataType;

    protected:
        using json = nlohmann::json;

        /**
         * @brief 获取指向的数据的裸指针，实际取内容时必须通过该函数，否则可能会取不到。
         * @note dev: 为了让getter正常地拥有const语义, 该函数不可以由外部调用.
         *  该函数使用const qualifier, 但返回非const的指针类型. 若允许外部调用可能造成数据问题。
         * @see IMPL_GETTER, INLINE_GETTER
         */
        DataType *GetDataInternal() const {
            auto clz_ptr = static_cast<const ClassType *>(this);
            assert(clz_ptr->InternalData != nullptr);
            return static_cast<DataType *>(clz_ptr->InternalData.get());
        }

    public:
        /// 群名称，群成员群名片，或好友昵称
        std::string nickOrNameCard() const {
            auto dataPtr = GetDataInternal();
            dataPtr->requestRefresh();
            std::shared_lock<std::shared_mutex> local_lck(dataPtr->getMutex());
            return dataPtr->_nickOrNameCard;
        };

        /// 头像url地址
        std::string avatarUrl() const {
            auto dataPtr = GetDataInternal();
            dataPtr->requestRefresh();
            std::shared_lock<std::shared_mutex> local_lck(dataPtr->getMutex());
            return dataPtr->_avatarUrl;
        };
    };

    class INudgeSupport {
    public:
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
