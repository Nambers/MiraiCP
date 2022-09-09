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

#include "MessageChain.h"

#include <json_fwd.hpp>
#include <string>

namespace MiraiCP {

    enum contactType {
        MIRAI_CONTACT = 0,
        MIRAI_FRIEND = 1,
        MIRAI_GROUP = 2,
        MIRAI_MEMBER = 3,
        MIRAI_OTHERTYPE = 4,
    };
    /*!
    * @brief group, friend, member的父类
    * @doxygenEg{1002, message.cpp, 发送以及回复群消息}
    */
    class Contact {
    protected: // attrs
        contactType _type;
        QQID _id;
        QQID _groupid;
        std::string _nickOrNameCard;
        std::string _avatarUrl;
        QQID _botid;
        bool _anonymous = false;

    protected:
        /// 发送语音
        MessageSource sendVoice0(const std::string &path);

    public:
        // constructors

        /*!
         * @brief 无参初始化Contact类型
         * @internal 一般在MiraiCp内部构造
         */
        Contact() {
            this->_type = MIRAI_CONTACT;
            this->_id = 0;
            this->_groupid = 0;
            this->_nickOrNameCard = "";
            this->_botid = 0;
        }

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
        explicit Contact(int type, QQID id, QQID gid, const std::string &name, QQID botid, bool anonymous = false) {
            if (type < 0 || type > 4) throw APIException("Contact::type incorrect", MIRAICP_EXCEPTION_WHERE);
            this->_type = static_cast<contactType>(type);
            this->_id = id;
            this->_groupid = gid;
            this->_nickOrNameCard = name;
            this->_botid = botid;
            this->_anonymous = anonymous;
        };

        //        Contact(Contact &&c) : _type(c._type), _id(c._id), _groupid(c._groupid), _botid(c._botid), _anonymous(c._anonymous), _nickOrNameCard(std::move(c._nickOrNameCard)), _avatarUrl(std::move(c._avatarUrl)) {
        //        }

        // destructor
        virtual ~Contact() = default;

        bool operator==(const Contact &c) const {
            return this->id() == c.id();
        }

        /// @brief 当前对象类型
        /// @see contactType
        ///     - 1 Friend 好友
        ///     - 2 Group 群聊
        ///     - 3 Member 群成员
        contactType type() const { return this->_type; }

        /// @brief id在全部情况存在
        ///     - 当当前type为1(Friend)时，为好友id
        ///     - 当当前type为2(Group)时，为群id
        ///     - 当当前type为3(Member)时，为群成员id
        QQID id() const { return this->_id; }

        /// @brief 当type为3的时候存在，否则为0，可以看作补充id
        ///     - 当当前type为1(Friend)时，为0
        ///     - 当当前type为2(Group)时，为0
        ///     - 当当前type为3(Member)时，为群号
        /// @attention 当当前type为2(Group)时，为0，不为群号，id才是群号
        QQID groupid() const { return this->_groupid; }

        /// 群名称，群成员群名片，或好友昵称
        std::string nickOrNameCard() const { return this->_nickOrNameCard; };

        /// 头像url地址
        std::string avatarUrl() const { return this->_avatarUrl; };

        /// 所属bot
        QQID botid() const { return this->_botid; };

    public: // serialization
        /// 序列化到json对象
        nlohmann::json toJson() const {
            nlohmann::json j;
            j["type"] = type();
            j["id"] = id();
            j["groupid"] = groupid();
            j["nickornamecard"] = nickOrNameCard();
            j["botid"] = botid();
            return j;
        }
        /// @deprecated since v2.8.1, use `this->toJson()`
        ShouldNotUse("use toJson") nlohmann::json serialization() const = delete;

        /// 序列化成文本，可以通过deserializationFromString反序列化，利于保存
        /// @see Contact::fromString()
        std::string toString() const {
            return this->toJson().dump();
        }
        /// @deprecated since v2.8.1, use `this->toString()`
        ShouldNotUse("use toString") std::string serializationToString() const = delete;

        /// 反序列化成bot，可以通过serializationToString序列化，利于保存
        /// @see Contact::serializationToString()
        /// @param source 序列化后的文本
        /// @throw APIException
        static Contact deserialize(const std::string &source);

        static Contact deserialize(nlohmann::json source);

        // for derive class
        template<class T>
        static T deserialize(const nlohmann::json &source) {
            static_assert(std::is_base_of_v<Contact, T>, "Cannot deserialize class that isn't base on Contact");
            T re = T();
            re._type = static_cast<contactType>(source["type"]);
            re._id = source["id"];
            re._groupid = source["gid"];
            re._nickOrNameCard = source["name"];
            re._botid = source["botid"];
            re._anonymous = source["anonymous"];
            re.refreshInfo();
            return re;
        }

    public:
        /// @deprecated since v2.8.1, use `Contact::deserialize(source)`
        ShouldNotUse("use deserialize") static Contact deserializationFromString(const std::string &source) = delete;

        /// @deprecated since v2.8.1, use `sendMessage(MiraiCode)` or `sendMsg0(msg.toMiraiCode(), retryTime, true, env)`
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
        template<class T>
        MessageSource quoteAndSendMessage(T s, MessageSource ms) {
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
        template<class... T>
        MessageSource quoteAndSendMessage(MessageSource ms, T... val) {
            return this->quoteAndSendMessage(MessageChain(val...), std::move(ms));
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
        template<class... T>
        MessageSource sendMessage(T... msg) {
            return this->sendMessage(MessageChain(msg...));
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
        template<class T>
        MessageSource sendMessage(T msg, int retryTime = 3) {
            return this->send1(msg, retryTime);
        }

        /// @deprecated since v2.8.1, use `sendMessage(msg)` or `sendMsg0(msg, retryTime, false, env)`
        ShouldNotUse("Use sendMessage") MessageSource sendMsg(const std::string &msg, int retryTime = 3, void *env = nullptr) = delete;

        /// @deprecated since v2.8.1, use `sendMessage(MiraiCode)` or `sendMsg0(msg.toMiraiCode(), retryTime, false, env);`
        ShouldNotUse("Use sendMessage") MessageSource sendMsg(const MiraiCode &msg, int retryTime = 3, void *env = nullptr) = delete;

        /// @deprecated since v2.8.1, use `sendMessage(Tools::VectorToString(std::move(msg)))` or `sendMsg0(Tools::VectorToString(std::move(msg)), retryTime, false, env);`
        ShouldNotUse("Use sendMessage") MessageSource sendMsg(std::vector<std::string> msg, int retryTime = 3, void *env = nullptr) = delete;

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

        template<class T>
        T to() {
            static_assert(std::is_base_of_v<Contact, T>);
            return T(*this);
        }

    private: // private methods
        /// 发送纯文本信息
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        MessageSource sendMsg0(const std::string &msg, int retryTime, bool miraicode = false) const;

        template<class T>
        MessageSource send1(T msg, int retryTime) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
            return sendMsg0(msg.toMiraiCode(), retryTime, true);
        }

        MessageSource send1(MessageChain msg, int retryTime) {
            return sendMsg0(msg.toMiraiCode(), retryTime, true);
        }

        MessageSource send1(MiraiCode msg, int retryTime) {
            return sendMsg0(msg.toMiraiCode(), retryTime, true);
        }

        MessageSource send1(std::string msg, int retryTime) {
            return sendMsg0(msg, retryTime, false);
        }

        MessageSource send1(const char *msg, int retryTime) {
            return sendMsg0(std::string(msg), retryTime, false);
        }

        MessageSource quoteAndSend0(const std::string &msg, MessageSource ms);

        template<class T>
        MessageSource quoteAndSend1(T s, MessageSource ms) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
            return this->quoteAndSend0(s.toMiraiCode(), ms);
        }

        MessageSource quoteAndSend1(std::string s, MessageSource ms) {
            return this->quoteAndSend0(s, ms);
        }

        MessageSource quoteAndSend1(MessageChain mc, MessageSource ms) {
            return this->quoteAndSend0(mc.toMiraiCode(), ms);
        }
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
