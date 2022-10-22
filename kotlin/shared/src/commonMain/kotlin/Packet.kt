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
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.message.MessageSerializers
import net.mamoe.mirai.message.data.MessageSource

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
        val id: Long = -1,
        /**
         * NotNullable
         * GroupId for Member / BotId for other
         */
        val id2: Long = -1,
        /**
         * Nullable
         * BotId for Member / null for other
         */
        val id3: Long? = null,
    )

    object Incoming {

    }

    object Outgoing {
        @Serializable
        data class MessageEventData(
            /// MiraiCode for message
            val message: String,
            /// MessageSource
            val source: String,
        )

        @Serializable
        data class MemberLeave(
            /**
             * 1 - Kick
             * 2 - Quit
             */
            val leaveType: Int,
            /**
             * if leaveType == 2, it should be empty
             * if leaveType = 1, it could be a Member or Bot itself
             */
            val operator: Contact? = null,
        )

        @Serializable
        data class EventData<T>(
            val eventData: T,
            val eventId: Int = -1,
            var subject: Contact = Contact(),
            var `object`: Contact = Contact(),
            val botId: Long = -1,
        )
    }

    object Utils {
        private fun Group.toContact(): Contact = Contact(2, this.id, this.bot.id)
        private fun Member.toContact(): Contact = Contact(3, this.id, this.group.id, this.bot.id)
        private fun AnonymousMember.toContact(): Contact =
            Contact(6, this.id, this.group.id, this.bot.id)

        private fun Friend.toContact(): Contact = Contact(1, this.id, this.bot.id)
        private fun Bot.toContact(): Contact = Contact(4, this.id, this.id)
        private fun Stranger.toContact(): Contact = Contact(5, this.id, this.bot.id)
        private fun net.mamoe.mirai.contact.Contact.toContact(): Contact = when (this) {
            is Group, is Friend, is Member, is Bot, is Stranger, is AnonymousMember -> this.toContact()
            else -> {
                PublicSharedData.logger.error("MiraiCP遇到意料之中的问题, 请到github仓库发送issue和黏贴本信息以修复此问题, 位置:Contact.toContact(), info:${this}")
                throw IllegalArgumentException("Unknown contact type")
            }
        }

        private inline fun <reified T : Any> BotEvent.eventToJson(event: T, builder: (Outgoing.EventData<T>) -> Unit) =
            Json.encodeToString(Outgoing.EventData(event, botId = this.bot.id).also { builder(it) })

        private fun MessageEvent.toMessageEventData(): Outgoing.MessageEventData = Outgoing.MessageEventData(
            this.message.serializeToMiraiCode(),
            PublicShared.json.encodeToString(
                MessageSerializers.serializersModule.serializer(),
                this.message[MessageSource]!!
            )
        )

        fun MessageEvent.toEventData(): String = eventToJson(toMessageEventData()) {
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

        private fun MemberLeaveEvent.toMemberLeaveEventData(): Outgoing.MemberLeave =
            when (this) {
                is MemberLeaveEvent.Kick -> Outgoing.MemberLeave(
                    1, this.operator?.toContact() ?: this.bot.toContact()
                )

                is MemberLeaveEvent.Quit -> Outgoing.MemberLeave(2)
            }

        fun MemberLeaveEvent.toEventData(): String = eventToJson(toMemberLeaveEventData()) {
            it.subject = this.group.toContact()
            it.`object` = this.member.toContact()
        }
    }
}