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


#include <json.hpp>
#include <string>
namespace MiraiCP {

    enum ContactType {
        MIRAI_CONTACT = 0,
        MIRAI_FRIEND = 1,
        MIRAI_GROUP = 2,
        MIRAI_MEMBER = 3,
        // bot
        MIRAI_OTHERTYPE = 4,
    };
    struct IContactData : public IMiraiData {
        std::string _nickOrNameCard;
        std::string _avatarUrl;
        QQID _id{};
        QQID _botid{};
        ContactType _type = MIRAI_CONTACT;

        void deserialize(nlohmann::json in_json) override;

        nlohmann::json toJson() const override;

        void refreshInfo() override;

        // todo(Antares): 用于quoteandsend，名字重新命名一下
        virtual nlohmann::json getSign() const;
    };

    struct GroupRelatedData : public IContactData {
        typedef IContactData Super;
        QQID _groupid;
        nlohmann::json toJson() const override;
        explicit GroupRelatedData(QQID in_groupid) : _groupid(in_groupid) {}
        nlohmann::json getSign() const override;
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
        std::shared_ptr<IContactData> InternalData;
        //        QQID _groupid;
        //        std::string _nickOrNameCard;
        //        std::string _avatarUrl;
        //        QQID _botid;
        //        bool _anonymous = false;

    public:
        /*!
         * @brief 无参初始化Contact类型
         * @internal 禁止使用
         */
        Contact() = delete;

        //        Contact() {
        //            //            this->_type = MIRAI_CONTACT;
        //            //            this->_id = 0;
        //            //            this->_botid = 0;
        //            //            this->_groupid = 0;
        //            //            this->_nickOrNameCard = "";
        //        }

    protected:
        explicit Contact(std::shared_ptr<IContactData> Data) {
            SetInternalData(std::move(Data));
        }

    public:
        /*!
         * @brief 初始化Contact类型
         * @note dev: 子类应调用该函数进行初始化
         */
        // Contact(QQID id, QQID inbotid, ContactType tp) : _id(id), _botid(inbotid), _type(tp) {}


        /*!
         * @brief 构造contact类型
         * @param type 类型
         *  @see Contact::type()
         * @param id ID
         *  @see Contact::id()
         * @param gid 是member的时候是群id，否则为0
         *  @see Contact::groupid
         * @param name 群名片或昵称或群名
         *  @see Contact::name()
         * @param botid 对应的botid
         */
        //        explicit Contact(int type, QQID id, QQID gid, const std::string &name, QQID botid, bool anonymous = false) {
        //            if (type < 0 || type > 4) throw APIException("Contact::type incorrect", MIRAICP_EXCEPTION_WHERE);
        //            this->_type = static_cast<ContactType>(type);
        //            this->_id = id;
        //            this->_groupid = gid;
        //            this->_nickOrNameCard = name;
        //            this->_botid = botid;
        //            this->_anonymous = anonymous;
        //        };

        // destructor
        virtual ~Contact() = default;

        bool operator==(const Contact &c) const {
            return this->id() == c.id();
        }

        /// @brief 设置内部数据指针
        /// @note dev: 避免直接使用 InternalData ，请使用该接口操作 InternalData 指针
        void SetInternalData(std::shared_ptr<IContactData> Data) { InternalData = std::move(Data); }

        /// @brief 尝试一次数据刷新
        /// @note 不一定会进行刷新，一般为内部调用。强制刷新请使用 forceRefreshInfo()
        /// @see forceRefreshInfo
        void refreshInfo() {
            InternalData->requestRefresh();
        }

        /// @brief 强制下次refreshInfo()调用时刷新数据
        /// @note 尽可能调用该函数，避免不必要的刷新
        /// @see refreshInfo
        void forceRefreshNextTime() {
            InternalData->forceRefreshNextTime();
        }

        /// @brief 强制数据刷新
        /// @note 频繁刷新可能会有性能损耗
        /// @see refreshInfo, forceRefreshNextTime
        void forceRefreshNow() {
            forceRefreshNextTime();
            refreshInfo();
        }

        /// @brief 当前对象类型
        /// @see ContactType
        /// @note dev: 不会修改，不需要锁
        ///     - ContactType::MIRAI_FRIEND 好友
        ///     - ContactType::MIRAI_GROUP 群聊
        ///     - ContactType::MIRAI_MEMBER 群成员
        ContactType type() const { return InternalData->_type; }

        /// @brief id 在全部情况存在
        /// @note dev: 不会修改，不需要锁
        ///     - 当前type为Friend时，为好友id
        ///     - 当前type为Group时，为群id
        ///     - 当前type为Member时，为群成员id
        QQID id() const { return InternalData->_id; }

        /// 所属bot
        /// @note dev: 不会修改，不需要锁
        QQID botid() const { return InternalData->_botid; };

        /// @brief 当type为3的时候存在，否则为0，可以看作补充id
        ///     - 当前type为1(Friend)时，为0
        ///     - 当前type为2(Group)时，为0
        ///     - 当前type为3(Member)时，为群号
        /// @attention 当当前type为2(Group)时，为0，不为群号，id才是群号
        // QQID groupid() const { return this->_groupid; }


        /// @deprecated since v2.8.1, use `sendMessage(MiraiCode)` or `sendMsgImpl(msg.toMiraiCode(), retryTime, true, env)`
        ShouldNotUse("Use sendMessage") MessageSource
                sendMiraiCode(const MiraiCode &msg, int retryTime = 3, void *env = nullptr) const = delete;

        /*!
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

        /*!
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

        /*!
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

        /// @brief 发送一条Message
        /// @detail 支持
        /// - std::string: 相当于发送PlainText(str)
        /// - MiraiCode 相当于发送反序列化MiraiCode后的
        /// - 各种SingleMessage的派生类
        /// - MessageChain
        /// @param msg Message
        /// @param retryTime 重试次数
        /// @return MessageSource
        template<typename T>
        MessageSource sendMessage(T &&msg, int retryTime = 3) {
            return this->unpackMsg(std::forward<T>(msg), retryTime);
        }

        /// @deprecated since v2.8.1, use `sendMessage(msg)` or `sendMsgImpl(msg, retryTime, false, env)`
        ShouldNotUse("Use sendMessage")
                MessageSource sendMsg(const std::string &msg, int retryTime = 3, void *env = nullptr) = delete;

        /// @deprecated since v2.8.1, use `sendMessage(MiraiCode)` or `sendMsgImpl(msg.toMiraiCode(), retryTime, false, env);`
        ShouldNotUse("Use sendMessage")
                MessageSource sendMsg(const MiraiCode &msg, int retryTime = 3, void *env = nullptr) = delete;

        /// @deprecated since v2.8.1, use `sendMessage(Tools::VectorToString(std::move(msg)))` or `sendMsgImpl(Tools::VectorToString(std::move(msg)), retryTime, false, env);`
        ShouldNotUse("Use sendMessage")
                MessageSource sendMsg(std::vector<std::string> msg, int retryTime = 3, void *env = nullptr) = delete;


        //        template<class T>
        //        T &to() {
        //            static_assert(std::is_base_of_v<Contact, T>);
        //            return static_cast<T &>(*this);
        //        }
        //
        //        template<class T>
        //        const T &to_const() const {
        //            static_assert(std::is_base_of_v<Contact, T>);
        //            return static_cast<const T &>(*this);
        //        }

    private: // private methods
        MessageSource quoteAndSend0(std::string msg, const MessageSource &ms);

        MessageSource quoteAndSend1(const SingleMessage &s, const MessageSource &ms) {
            return this->quoteAndSend0(s.toMiraiCode(), ms);
        }

        MessageSource quoteAndSend1(const std::string &s, const MessageSource &ms) {
            return this->quoteAndSend0(s, ms);
        }

        MessageSource quoteAndSend1(const MessageChain &mc, const MessageSource &ms) {
            return this->quoteAndSend0(mc.toMiraiCode(), ms);
        }

    public: // serialization
        /// 序列化到json对象
        nlohmann::json toJson() const { return InternalData->toJson(); } //{
                                                                         //            nlohmann::json j;
                                                                         //            j["type"] = type();
                                                                         //            j["id"] = id();
                                                                         //            j["groupid"] = groupid();
                                                                         //            j["nickornamecard"] = nickOrNameCard();
                                                                         //            j["botid"] = botid();
                                                                         //            return j;
                                                                         //        }
        /// @deprecated since v2.8.1, use `this->toJson()`
        ShouldNotUse("use toJson") nlohmann::json serialization() const;

        /// 序列化成文本，可以通过deserializationFromString反序列化，利于保存
        /// @see Contact::fromString()
        std::string toString() const {
            return toJson().dump();
        }
        /// @deprecated since v2.8.1, use `this->toString()`
        ShouldNotUse("use toString") std::string serializationToString() const = delete;
        /// @deprecated since v2.8.1, use `Contact::deserialize(source)`
        // ShouldNotUse("use deserialize") static Contact deserializationFromString(const std::string &source);

        /// 反序列化成Contact智能指针
        /// @param source 序列化后的文本
        static std::shared_ptr<Contact> deserializeToPointer(nlohmann::json source);

        // for derived class
        template<class T>
        static T deserialize(nlohmann::json source) {
            static_assert(std::is_base_of_v<Contact, T>, "Cannot deserialize class that isn't base on Contact");
            //            T contactObject = T();
            //            contactObject.deserializeWriter(source);
            //            contactObject.refreshInfo();
            //            return contactObject;
            return T(std::move(source));
        }

        /*!
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
        MessageSource sendMsgImpl(std::string msg, int retryTime, bool miraicode = false) const;

        MessageSource unpackMsg(const SingleMessage &msg, int retryTime) const {
            return sendMsgImpl(msg.toMiraiCode(), retryTime, true);
        }

        MessageSource unpackMsg(std::string msg, int retryTime) const {
            return sendMsgImpl(std::move(msg), retryTime, false);
        }

        MessageSource unpackMsg(const char *msg, int retryTime) const {
            return sendMsgImpl(std::string(msg), retryTime, false);
        }
    };

    /// @brief Contact类型的数据接口模板类
    /// @note dev: 任何操作内部数据的行为在此处定义。Contact只处理 InternalData 这个 shared_ptr 本身
    template<typename ClassType, typename InternalDataType>
    struct ContactDataHelper {
        using json = nlohmann::json;
        //        typedef Contact Super;
        typedef InternalDataType DataType;

        DataType *GetDataInternal() {
            auto clz_ptr = static_cast<ClassType *>(this);
            assert(clz_ptr->InternalData != nullptr);
            return static_cast<DataType *>(clz_ptr->InternalData.get());
        }

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
