/*
 * Copyright (c) 2020 - 2022. Eritque arcus and contributors.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version(in your opinion).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

package tech.eritquearcus.miraicp.shared

import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.Serializable
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import kotlinx.serialization.serializer
import net.mamoe.mirai.Bot
import net.mamoe.mirai.contact.*
import net.mamoe.mirai.data.RequestEventData.Factory.toRequestEventData
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.message.MessageSerializers
import net.mamoe.mirai.message.data.MessageChain.Companion.serializeToJsonString
import net.mamoe.mirai.message.data.MessageSource
import net.mamoe.mirai.message.data.toMessageChain
import net.mamoe.mirai.utils.MiraiExperimentalApi

@OptIn(ExperimentalSerializationApi::class)
val json by lazy {
    Json {
        serializersModule = MessageSerializers.serializersModule
        ignoreUnknownKeys = true
        encodeDefaults = true
        explicitNulls = false
    }
}
object Packets {
    private fun Group.toContact(): Contact = Contact(2, this.id, this.bot.id)
    private fun Member.toContact(): Contact = Contact(3, this.id, this.bot.id, this.group.id)
    private fun AnonymousMember.toContact(): Contact =
        Contact(6, this.id, this.bot.id, this.group.id)

    private fun Friend.toContact(): Contact = Contact(1, this.id, this.bot.id)
    private fun Bot.toContact(): Contact = Contact(4, this.id, this.id)
    private fun Stranger.toContact(): Contact = Contact(5, this.id, this.bot.id)
    private fun net.mamoe.mirai.contact.Contact.toContact(): Contact = when (this) {
        is Group -> this.toContact()
        is Member -> this.toContact()
        is AnonymousMember -> this.toContact()
        is Friend -> this.toContact()
        is Bot -> (this as Bot).toContact()
        is Stranger -> this.toContact()
        else -> {
            PublicSharedData.logger.error("MiraiCP遇到意料之中的问题, 请到github仓库发送issue和黏贴本信息以修复此问题, 位置:Contact.toContact(), info:${this}")
            throw IllegalArgumentException("Unknown contact type")
        }
    }

    private fun UserOrBot.toContact(): Contact = when (this) {
        is Member -> toContact()
        is Bot -> toContact()
        is Friend -> toContact()
        else -> {
            PublicSharedData.logger.error("MiraiCP遇到意料之中的问题, 请到github仓库发送issue和黏贴本信息以修复此问题, 位置:UserOrBot.toContact(), info:${this}")
            throw IllegalArgumentException("unsupported contact type")
        }
    }

    @Serializable
    data class Contact(
        /**
         * 1 - Friend
         * 2 - Group
         * 3 - Member
         * 4 - Bot
         * 5 - Stranger
         * 6- Anonymous Member
         */
        val type: Int = -1,
        /**
         * NotNullable
         * friendId / GroupId / MemberId / BotId / StrangerId / Anonymous MemberId
         */
        val id: Long = 0,
        /**
         * NotNullAble
         * BotId for All
         */
        val botId: Long = 0,
        /**
         * NotNullable
         * GroupId for Member
         */
        val groupId: Long = 0,
    )

    object Incoming {
        @Serializable
        data class Request(
            val text: String,
            val accept: Boolean,
            val botid: Long,
            val ban: Boolean? = null,
        )
    }

    object Outgoing {
        interface EventPacket {
            val eventId: Int
        }

        @Serializable
        data class EventData<T: EventPacket>(
            val eventData: T,
            val eventId: Int = -1,
            var subject: Contact? = Contact(),
            var `object`: Contact? = Contact(),
            val botId: Long = 0,
        )

        @Serializable
        class BotOnline : EventPacket {
            override val eventId: Int = 0
        }

        @Serializable
        class BotJoinGroup : EventPacket {
            override val eventId = 1

            /**
             * 1 - Invite
             * 2 - Active
             * 3 - Retrieve
             */
            private val eventType: Int

            /// what group the bot join
            private val group: Contact

            /// who invite the bot
            private val invitor: Contact?

            @OptIn(MiraiExperimentalApi::class)
            constructor(event: BotJoinGroupEvent) {
                group = event.group.toContact()
                when (event) {
                    is BotJoinGroupEvent.Invite -> {
                        eventType = 1
                        invitor = event.invitor.toContact()
                    }

                    is BotJoinGroupEvent.Active -> {
                        eventType = 2
                        invitor = null
                    }

                    is BotJoinGroupEvent.Retrieve -> {
                        eventType = 3
                        invitor = null
                    }
                }
            }
        }

        @Serializable
        class BotInvitedJoinGroupRequest : EventPacket {
            override val eventId = 2

            /// request serialization data, use to accept or reject
            private val request: String

            /// eventId, unique for each request
            private val requestEventId: Long

            /// group name
            private val groupName: String

            private val invitorNick: String

            constructor(event: BotInvitedJoinGroupRequestEvent) {
                val data = event.toRequestEventData()
                request = json.encodeToString(data)
                requestEventId = data.eventId
                groupName = data.groupName
                invitorNick = data.invitorNick
            }
        }

        @Serializable
        class BotLeave : EventPacket {
            override val eventId = 3

            /**
             * 1 - Active
             * 2 - Kick
             * 3 - Disband
             */
            val eventType: Int

            @OptIn(MiraiExperimentalApi::class)
            constructor(event: BotLeaveEvent) {
                eventType = when (event) {
                    is BotLeaveEvent.Active -> 1
                    is BotLeaveEvent.Kick -> 2
                    is BotLeaveEvent.Disband -> 3
                }
            }
        }

        @Serializable
        class MessageEventData : EventPacket {
            override val eventId = 4
            private val eventType: Int

            /// serialization json string of MessageChain without source
            private val message: String

            /// MessageSource
            private val source: String


            constructor(event: MessageEvent) {
                eventType = when (event) {
                    is FriendMessageEvent -> 1
                    is GroupMessageEvent -> 2
                    is GroupTempMessageEvent -> 3
                    is StrangerMessageEvent -> 4
                    else -> {
                        PublicSharedData.logger.error("MiraiCP遇到意料之中的问题, 请到github仓库发送issue和黏贴本信息以修复此问题, 位置:MessageEventData.constructor, info:${event}")
                        throw IllegalArgumentException("Unknown message event type")
                    }
                }

                message = json.encodeToString(event.message.let {
                    if (it.contains(MessageSource)) it.drop(1)
                    else it
                })
                source = json.encodeToString(
                    MessageSerializers.serializersModule.serializer(),
                    event.message[MessageSource]!!
                )
            }
        }

        @Serializable
        class MessageRecall : EventPacket {
            override val eventId = 5

            /// unique id for each message
            private val messageIds: IntArray

            /// unique id for each message
            private val messageInternalIds: IntArray

            /// when is the message send
            private val messageTime: Int

            /// who send the message
            private val author: Contact

            /// recall type
            private val eventType: Int

            constructor(event: MessageRecallEvent) {
                messageIds = event.messageIds
                messageInternalIds = event.messageInternalIds
                messageTime = event.messageTime
                when (event) {
                    is MessageRecallEvent.GroupRecall -> {
                        author = event.author.toContact()
                        eventType = 1
                    }

                    is MessageRecallEvent.FriendRecall -> {
                        author = event.author.toContact()
                        eventType = 2
                    }
                }
            }
        }

        @Serializable
        class MessagePreSend : EventPacket {
            override val eventId = 6
            private val message: String

            constructor(event: MessagePreSendEvent) {
                message = event.message.toMessageChain().serializeToJsonString()
            }
        }

        @Serializable
        class Nudge : EventPacket {
            override val eventId = 7

            /// who receive the nudge
            val target: Contact

            /// text in nudge
            val action: String

            /// text in nudge
            val suffix: String

            constructor(event: NudgeEvent) {
                target = event.target.toContact()
                action = event.action
                suffix = event.suffix
            }
        }

        @Serializable
        class NewFriendRequest : EventPacket {
            override val eventId = 8

            /// request serialization data, use to accept or reject
            private val request: String

            /// eventId, unique for each request
            private val requestEventId: Long

            /// message from requester
            private val message: String

            constructor(event: NewFriendRequestEvent) {
                val data = event.toRequestEventData()
                request = json.encodeToString(data)
                requestEventId = data.eventId
                message = data.message
            }
        }

        @Serializable
        class MemberLeave : EventPacket {
            override val eventId = 9

            /**
             * 1 - Kick
             * 2 - Quit
             */
            private val eventType: Int

            /**
             * if leaveType == 2, it should be null
             * if leaveType = 1, it could be a Member or Bot itself
             */
            private val operator: Contact?

            constructor(event: MemberLeaveEvent) {
                when (event) {
                    is MemberLeaveEvent.Kick -> {
                        eventType = 1
                        operator = event.operator?.toContact() ?: event.group.botAsMember.toContact()
                    }

                    is MemberLeaveEvent.Quit -> {
                        eventType = 2
                        operator = null
                    }
                }
            }
        }

        @Serializable
        class MemberJoin : EventPacket {
            override val eventId = 10

            /**
            invite - 1
            active - 2
            retrieve - 3
             */
            private val eventType: Int

            /// if joinType != 1, it should be null
            private val invitor: Contact?

            constructor(event: MemberJoinEvent) {
                when (event) {
                    is MemberJoinEvent.Invite -> {
                        eventType = 1
                        invitor = event.invitor.toContact()
                    }

                    is MemberJoinEvent.Active -> {
                        eventType = 2
                        invitor = null
                    }

                    is MemberJoinEvent.Retrieve -> {
                        eventType = 3
                        invitor = null
                    }
                }
            }
        }

        @Serializable
        class MemberJoinRequest : EventPacket {
            override val eventId = 11

            /// request serialization data, use to accept or reject
            private val requestData: String

            /// invitor
            private val invitor: Contact?

            /// nick of requester
            private val fromNick: String

            constructor(event: MemberJoinRequestEvent) {
                val data = event.toRequestEventData()
                requestData = json.encodeToString(data)
                invitor = event.invitor?.toContact()
                fromNick = event.fromNick
            }
        }

        @Serializable
        data class LibLoaderEvent(
            val name: String,
            val content: String? = null,
            override val eventId: Int = 1000,
        ) : EventPacket
    }

    object Utils {
        private inline fun <reified T : Outgoing.EventPacket> BotEvent.eventToJson(
            event: T,
            builder: (Outgoing.EventData<T>) -> Unit
        ) =
            Json.encodeToString(
                Outgoing.EventData(event, eventId = event.eventId).also { builder(it) })

        fun MessageEvent.toEventData(): String = eventToJson(Outgoing.MessageEventData(this)) {
            when (this) {
                is GroupMessageEvent -> {
                    it.subject = this.group.toContact()
                    it.`object` = this.sender.toContact()
                }

                is FriendMessageEvent -> {
                    it.subject = this.friend.toContact()
                    it.`object` = this.friend.toContact()
                }

                is GroupTempMessageEvent -> {
                    it.subject = this.group.toContact()
                    it.`object` = this.sender.toContact()
                }

                is StrangerMessageEvent -> {
                    it.subject = this.stranger.toContact()
                    it.`object` = this.stranger.toContact()
                }

                else -> {
                    PublicSharedData.logger.error("MiraiCP遇到意料之中的问题, 请到github仓库发送issue和黏贴本信息以修复此问题, 位置:MessageEvent.toEventData(), info:${this}")
                }
            }
        }

        fun MemberLeaveEvent.toEventData(): String = eventToJson(Outgoing.MemberLeave(this)) {
            it.subject = this.group.toContact()
            it.`object` = this.member.toContact()
        }

        fun MemberJoinEvent.toEventData(): String = eventToJson(Outgoing.MemberJoin(this)) {
            it.subject = this.group.toContact()
            it.`object` = this.member.toContact()
        }

        fun NewFriendRequestEvent.toEventData(): String = eventToJson(Outgoing.NewFriendRequest(this)) {
            it.subject = this.fromGroup?.toContact() ?: Contact(2, this.fromGroupId, this.bot.id)
            it.`object` = Contact(1, this.fromId, this.bot.id)
        }

        fun MessageRecallEvent.toEventData(): String = eventToJson(Outgoing.MessageRecall(this)) {
            when (this) {
                is MessageRecallEvent.GroupRecall -> {
                    it.subject = this.group.toContact()
                    it.`object` = this.operator?.toContact() ?: this.group.botAsMember.toContact()
                }

                is MessageRecallEvent.FriendRecall -> {
                    it.subject = this.operator.toContact()
                    it.`object` = this.operator.toContact()
                }
            }
        }

        fun BotJoinGroupEvent.toEventData(): String = eventToJson(Outgoing.BotJoinGroup(this)) {
            it.subject = this.group.toContact()
            it.`object` = this.group.botAsMember.toContact()
        }

        fun BotInvitedJoinGroupRequestEvent.toEventData(): String =
            eventToJson(Outgoing.BotInvitedJoinGroupRequest(this)) {
                it.subject = Contact(2, this.groupId, this.bot.id)
                it.`object` = invitor?.toContact() ?: Contact(1, invitorId, this.bot.id)
            }

        fun NudgeEvent.toEventData(): String = eventToJson(Outgoing.Nudge(this)) {
            it.subject = this.subject.toContact()
            it.`object` = this.from.toContact()
        }

        @OptIn(MiraiExperimentalApi::class)
        fun BotLeaveEvent.toEventData(): String = eventToJson(Outgoing.BotLeave(this)) {
            it.subject = this.group.toContact()
            when (it.eventData.eventType) {
                1 -> it.`object` = this.group.botAsMember.toContact()
                2 -> it.`object` = (this as BotLeaveEvent.Kick).operator.toContact()
                3 -> it.`object` = (this as BotLeaveEvent.Disband).operator.toContact()
            }
        }

        fun MemberJoinRequestEvent.toEventData(): String = eventToJson(Outgoing.MemberJoinRequest(this)) {
            it.subject = group?.toContact() ?: Contact(2, groupId, bot.id)
            it.`object` = Contact(3, fromId, bot.id, groupId)
        }

        fun MessagePreSendEvent.toEventData(): String = eventToJson(Outgoing.MessagePreSend(this)) {
            it.subject = this.target.toContact()
//            it.`object` = this.bot.toContact()
        }

        fun BotOnlineEvent.toEventData(): String = eventToJson(Outgoing.BotOnline()) {
//            it.subject = this.bot.toContact()
//            it.`object` = this.bot.toContact()
        }
    }
}