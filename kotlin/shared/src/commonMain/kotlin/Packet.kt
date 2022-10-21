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

import kotlinx.serialization.Serializable
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import kotlinx.serialization.serializer
import net.mamoe.mirai.Bot
import net.mamoe.mirai.contact.*
import net.mamoe.mirai.event.events.*
import net.mamoe.mirai.message.MessageSerializers
import net.mamoe.mirai.message.data.MessageSource

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
        val id: Long? = null,
        val id2: Long? = null,
        val botId: Long = -1,
    )

    object Incoming {

    }

    object Outgoing {
        @Serializable
        data class MessageEventData(
            val message: String,
            val source: String,
        )

        @Serializable
        data class MemberLeave(
            val leaveType: Int,
            val operator: Contact?,
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
        private fun Group.toContact(): Contact = Contact(2, this.id, 0, this.bot.id)
        private fun Member.toContact(): Contact = Contact(3, this.id, this.group.id, this.bot.id)
        private fun AnonymousMember.toContact(): Contact =
            Contact(6, this.id, this.group.id, this.bot.id)

        private fun Friend.toContact(): Contact = Contact(1, this.id, 0, this.bot.id)
        private fun Bot.toContact(): Contact = Contact(4, this.id, 0, this.id)
        private fun Stranger.toContact(): Contact = Contact(5, this.id, 0, this.bot.id)
        private fun net.mamoe.mirai.contact.Contact.toContact(): Contact = when (this) {
            is Group, is Friend, is Member, is Bot, is Stranger, is AnonymousMember -> this.toContact()
            else -> {
                PublicSharedData.logger.error("MiraiCP遇到意料之中的问题, 请到github仓库发送issue和黏贴本信息以修复此问题, 位置:Contact.toContact(), info:${this}")
                throw IllegalArgumentException("Unknown contact type")
            }
        }

        private fun <T : Any> BotEvent.eventToJson(event: T, builder: (Outgoing.EventData<T>) -> Unit) =
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

                is MemberLeaveEvent.Quit -> Outgoing.MemberLeave(
                    2, null
                )
            }

        fun MemberLeaveEvent.toEventData(): String = eventToJson(toMemberLeaveEventData()) {
            it.subject = this.group.toContact()
            it.`object` = this.member.toContact()
        }
    }
}