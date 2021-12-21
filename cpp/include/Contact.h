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

#ifndef MIRAICP_PRO_CONTACT_H
#define MIRAICP_PRO_CONTACT_H

#include "MessageChain.h"

#include "json.hpp"
#include <string>

namespace MiraiCP {
    using QQID = unsigned long long;

    /*!
    * @brief group, friend, member的父类
     * @example 发送信息
     * 以群聊信息为例
     * @code
     * Event::processor.registerEvent<GroupMessageEvent>([](GroupMessageEvent e){
     * // 发送MessageChain
     * e.group.sendMessage(e.message);
     * // 发送SingleMessaeg
     * e.group.sendMessage(PlainText("a"));
     * e.group.sendMessaeg(e.sender.at(), PlainText(""), Face(5)));
     * }
     * @endcode
     * @example 回复信息
     * 以群聊信息为例
     * 和发送信息很像, 除了要多传入一个MessageSource
     * @code
     * Event::processor.registerEvent<GroupMessageEvent>([](GroupMessageEvent e){
     * e.group.quoteAndSend(e.message.source.value(), PlainText("a"));
     * }
     * @endcode
    */
    class Contact {
    private:
        /// 发送纯文本信息
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        MessageSource sendMsg0(const std::string &msg, int retryTime, bool miraicode = false,
                               JNIEnv * = nullptr) const;

        template<class T>
        MessageSource send1(T msg, int retryTime, JNIEnv *env) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
            return sendMsg0(msg.toMiraiCode(), retryTime, true, env);
        }

        MessageSource send1(MessageChain msg, int retryTime, JNIEnv *env) {
            return sendMsg0(msg.toMiraiCode(), retryTime, true, env);
        }

        MessageSource send1(MiraiCode msg, int retryTime, JNIEnv *env) {
            return sendMsg0(msg.toMiraiCode(), retryTime, true, env);
        }

        MessageSource send1(std::string msg, int retryTime, JNIEnv *env) {
            return sendMsg0(msg, retryTime, false, env);
        }

        MessageSource send1(const char *msg, int retryTime, JNIEnv *env) {
            return sendMsg0(std::string(msg), retryTime, false, env);
        }

        MessageSource quoteAndSend0(const std::string &msg, MessageSource ms, JNIEnv *env = nullptr);

        template<class T>
        MessageSource quoteAndSend1(T s, MessageSource ms, JNIEnv *env = nullptr) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
            return this->quoteAndSend0(s.toMiraiCode(), ms, env);
        }

        MessageSource quoteAndSend1(std::string s, MessageSource ms, JNIEnv *env) {
            return this->quoteAndSend0(s, ms, env);
        }

        MessageSource quoteAndSend1(MessageChain mc, MessageSource ms, JNIEnv *env) {
            return this->quoteAndSend0(mc.toMiraiCode(), ms, env);
        }

    protected:
        int _type = 0;
        QQID _id;
        QQID _groupid;
        std::string _nickOrNameCard;
        std::string _avatarUrl;
        QQID _botid;
        bool _anonymous = false;

        /// 发送语音
        MessageSource sendVoice0(const std::string &path, JNIEnv * = nullptr);

    public:
        /*!
         * @brief 无参初始化Contact类型
         * @internal 一般在MiraiCp内部构造
         */
        Contact() {
            this->_type = 0;
            this->_id = 0;
            this->_groupid = 0;
            this->_nickOrNameCard = "";
            this->_botid = 0;
        }

        bool operator==(const Contact &c) const {
            return this->id() == c.id();
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
            this->_type = type;
            this->_id = id;
            this->_groupid = gid;
            this->_nickOrNameCard = name;
            this->_botid = botid;
            this->_anonymous = anonymous;
        };

        /// @brief 当前对象类型
        ///     - 1 Friend 好友
        ///     - 2 Group 群聊
        ///     - 3 Member 群成员
        int type() const { return this->_type; }

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
        /// @deprecated use toJson, since v2.8.1
        [[deprecated("use toJson")]] nlohmann::json serialization() const {
            return this->toJson();
        }

        /// 序列化成文本，可以通过deserializationFromString反序列化，利于保存
        /// @see Contact::fromString()
        std::string toString() const {
            return this->toJson().dump();
        }
        /// @deprecated use toString, since v2.8.1
        [[deprecated("use toString")]] std::string serializationToString() const {
            return this->toString();
        }

        /// 反序列化成bot，可以通过serializationToString序列化，利于保存
        /// @see Contact::serializationToString()
        /// @param source 序列化后的文本
        /// @throw APIException
        static Contact deserialize(const std::string &source);
        static Contact deserialize(nlohmann::json source);

        /// @deprecated use deserialize, since v2.8.1
        [[deprecated("use deserialize")]] static Contact deserializationFromString(const std::string &source) {
            return Contact::deserialize(source);
        }


        /// @brief 发送MiraiCode信息
        /// @param msg 发送的MiraiCode
        /// @param retryTime 当服务器无应答(通常由于发送消息频率太快导致)时的重试次数，每次重试相隔1s，-1为无限制，如果在重试次数用完后还是没能成功发送就会抛出TimeOutException
        /// @return MessageSource
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        /// @deprecated 用 sendMessage, since v2.8.1
        [[deprecated("Use sendMessage")]] MessageSource sendMiraiCode(const MiraiCode &msg, int retryTime = 3,
                                                                      JNIEnv *env = nullptr) const {
            return sendMsg0(msg.toMiraiCode(), retryTime, true, env);
        }

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
        MessageSource quoteAndSendMessage(T s, MessageSource ms, JNIEnv *env = nullptr) {
            return this->quoteAndSend1(s, ms, env);
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
        MessageSource sendMessage(T msg, int retryTime = 3, JNIEnv *env = nullptr) {
            return this->send1(msg, retryTime, env);
        }

        /// @brief 发送纯文本信息
        /// @param msg 发送的信息
        /// @param retryTime 当服务器无应答(通常由于发送消息频率太快导致)时的重试次数，每次重试相隔1s，-1为无限制，如果在重试次数用完后还是没能成功发送就会抛出TimeOutException
        /// @return MessageSource
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        /// @deprecated 用 sendMessage, since v2.8.1
        [[deprecated("Use sendMessage")]] MessageSource
        sendMsg(const std::string &msg, int retryTime = 3, JNIEnv *env = nullptr) {
            return sendMsg0(msg, retryTime, false, env);
        }

        /// @brief 以纯文本发送MiraiCode信息
        /// @param msg 发送的信息
        /// @param retryTime 当服务器无应答(通常由于发送消息频率太快导致)时的重试次数，每次重试相隔1s，-1为无限制，如果在重试次数用完后还是没能成功发送就会抛出TimeOutException
        /// @return MessageSource
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        /// @deprecated 用 sendMessage, since v2.8.1
        [[deprecated("Use sendMessage")]] MessageSource
        sendMsg(const MiraiCode &msg, int retryTime = 3, JNIEnv *env = nullptr) {
            return sendMsg0(msg.toMiraiCode(), retryTime, false, env);
        }

        /// @brief 发送纯文本信息
        /// @param msg 发送的信息
        /// @param retryTime 当服务器无应答(通常由于发送消息频率太快导致)时的重试次数，每次重试相隔1s，-1为无限制，如果在重试次数用完后还是没能成功发送就会抛出TimeOutException
        /// @return MessageSource
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        /// @deprecated 用 sendMessage, since v2.8.1
        [[deprecated("Use sendMessage")]] MessageSource sendMsg(std::vector<std::string> msg, int retryTime = 3,
                                                                JNIEnv *env = nullptr);

        /*!
        * @brief上传本地图片，务必要用绝对路径
        * 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
        * @attention 最大支持图片大小为30MB
        * @throws
        * -可能抛出UploadException异常代表路径无效或大小大于30MB
        * -可能抛出MemberException找不到群或群成员
        */
        Image uploadImg(const std::string &path, JNIEnv * = nullptr) const;

        /// 刷新当前对象信息
        virtual void refreshInfo(JNIEnv *){};
    };


} // namespace MiraiCP

#endif //MIRAICP_PRO_CONTACT_H
