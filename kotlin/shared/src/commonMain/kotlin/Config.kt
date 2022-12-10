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
import net.mamoe.mirai.message.data.ForwardMessage
import net.mamoe.mirai.message.data.RawForwardMessage

object Config {
    @Serializable
    data class Contact(
        val type: Int,
        val id: Long? = null,
        val groupid: Long? = null,
        val nickornamecard: String? = null,
        val botid: Long,
        val anonymous: Boolean = false,
    )

    @Serializable
    data class SendRequest(
        val contact: Contact,
        val content: String,
    )

    @Serializable
    data class ForwardMessageJson(
        val type: Int,
        val id: Long,
        val groupid: Long,
        val display: ForwardedMessageDisplay? = null,
        val content: Content,
    ) {
        @Serializable
        data class Content(
            val value: List<Value>,
        ) {
            @Serializable
            data class Value(
                val isForwardedMessage: Boolean? = null,
                val name: String,
                val id: Long,
                val time: Int,
                val message: String,
                val display: ForwardedMessageDisplay? = null,
            )
        }
    }

    // Announcement params
    @Serializable
    data class AP(
        val sendToNewMember: Boolean = false,
        /** 置顶. 可以有多个置顶公告 */
        val isPinned: Boolean = false,
        /** 显示能够引导群成员修改昵称的窗口 */
        val showEditCard: Boolean = false,
        /** 使用弹窗 */
        val showPopup: Boolean = false,
        /** 需要群成员确认 */
        val requireConfirmation: Boolean = false,
    )

    @Serializable
    data class OnlineA(
        val content: String,
        val fid: String,
        val imageid: String,
        val confirmationNum: Int,
        val senderid: Long,
        val groupid: Long,
        val botid: Long,
        val time: Long,
        val params: AP,
    )

    // Announcement identify
    @Serializable
    data class IdentifyA(
        val botid: Long,
        val groupid: Long,
        val type: Int,
        val fid: String? = null,
    )

    @Serializable
    data class BriefOfflineA(
        val content: String,
        val params: AP,
    )

    @Serializable
    data class GroupSetting(
        val name: String,
        val isMuteAll: Boolean,
        val isAllowMemberInvite: Boolean,
        val isAutoApproveEnabled: Boolean,
        val isAnonymousChatEnabled: Boolean,
    )

    @Serializable
    data class ContactInfo(
        val nickornamecard: String,
        val avatarUrl: String,
        val setting: GroupSetting = GroupSetting("", false, false, false, false),
    )

    @Serializable
    data class DInfo(
        val url: String,
        val md5: String,
        val sha1: String,
    )

    @Serializable
    data class FInfo(
        val size: Long,
        val uploaderid: Long,
        val expirytime: Long,
        val uploadtime: Long,
        val lastmodifytime: Long,
    )

    @Serializable
    data class FileInfoOut(
        val id: String,
        val internalid: Int = 0,
        val name: String = "",
        val path: String = "/",
        val dinfo: DInfo,
        val finfo: FInfo,
    )

    @Serializable
    data class ImgInfo(
        val size: Long,
        val width: Int?,
        val height: Int?,
        val md5: String? = null,
        val url: String? = null,
        val imageId: String? = null,
        val type: Int? = null,
        val isEmoji: Boolean? = null,
    )

    @Serializable
    data class Message(
        val messageSource: String,
    )

    @Serializable
    data class ForwardedMessageDisplay(
        val brief: String,
        val preview: List<String>,
        val source: String,
        val summary: String,
        val title: String,
    )

    @Serializable
    data class FileInfoIn(
        val path: String? = null,
        val id: String? = null,
        val filepath: String? = null,
    )

    @Serializable
    data class Log(
        val id: Long,
        val log: String,
        val name: String? = null,
    )

    @Serializable
    data class VoiceInfoIn(
        val path: String,
    )

    @Serializable
    data class Operation(
        val type: Int,
        val data: Data,
    ) {
        @Serializable
        data class Data(
            val source: String? = null,
            val miraiCode: Boolean? = null,
            val quit: Boolean? = null,
            val announcement: Boolean? = null,
            val fileName: String? = null,
            val botid: Long? = null,
            val contactSource: String? = null,
            val time: Long? = null,
            val message: String? = null,
            val text: String? = null,
            val messageSource: String? = null,
            val msg: String? = null,
            val sign: String? = null,
            val identify: String? = null,
            val halt: Boolean? = null,
            val admin: Boolean? = null,
            val command: String? = null,
            val newName: String? = null,
            val size: Long? = null,
            val width: Int? = null,
            val height: Int? = null,
            val md5: String? = null,
            val url: String? = null,
            val imageid: String? = null,
            val type: Int? = null,
        )
    }

    @Serializable
    data class QuoteSign(
        val MiraiCode: Boolean,
        val groupid: Long,
    )
}

class DisplayS(private val s: Config.ForwardedMessageDisplay) : ForwardMessage.DisplayStrategy {
    override fun generateBrief(forward: RawForwardMessage): String = s.brief
    override fun generatePreview(forward: RawForwardMessage): List<String> = s.preview
    override fun generateSource(forward: RawForwardMessage): String = s.source
    override fun generateSummary(forward: RawForwardMessage): String = s.summary
    override fun generateTitle(forward: RawForwardMessage): String = s.title
}

object CPPConfig {
    @Serializable
    data class cppPath(
        val path: String,
        val dependencies: List<String>? = null,
    )

    @Serializable
    data class AdvanceConfig(
//        val maxThread: Int? = null,
        val libLoaderPath: String? = null,
    )

    @Serializable
    data class PluginConfig(
        val pluginConfig: List<cppPath>,
        val advanceConfig: AdvanceConfig? = null,
    )

    @Serializable
    data class LoaderConfig(
        val accounts: List<Account>? = null,
        val cppPaths: List<cppPath>,
        val advanceConfig: AdvanceConfig? = null,
    ) {
        @Serializable
        data class Account(
            val id: Long,
            val passwords: String,
            val protocol: String? = null,
            val heatBeat: String? = null,
            val md5: Boolean? = null,
            val autoLogin: Boolean? = null,
            var logined: Boolean? = null,
        )
    }
}