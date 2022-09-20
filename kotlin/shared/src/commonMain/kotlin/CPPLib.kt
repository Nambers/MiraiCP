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

enum class Operation_code {
    Recall,
    Send,
    RefreshInfo,
    UploadImg,
    QueryBFL,
    QueryBGL,
    SendFile,
    RemoteFileInfo,
    QueryImgInfo,
    MuteM,
    QueryM,
    KickM,
    QueryOwner,
    UploadVoice,
    QueryML,
    GroupSetting,
    Buildforward,
    Nfroperation,
    Gioperation,
    SendWithQuote,
    Announcement,
    Timer,
    Nudge,
    NextMsg,
    ModifyAdmin,
    MemberJoinRequest,
    ImageUploaded,
    CommandReg,
    ChangeNameCard
}

expect object CPPLib {
    fun init(
        // 本地地址
        libPath: List<String>? = emptyList(),
        cfgPath: String? = "",
        // 只在libPath == null下生效
        callback: () -> Unit = {}
    )

    //cd shared/build/classes/kotlin/main && javap.exe -s tech.eritquearcus.miraicp.shared.CPP_lib
    fun contact(source: String): Config.Contact
    suspend fun KSend(source: String, miraiCode: Boolean): String

    // 日志发送接口
    fun KSendLog(log: String, level: Int)

    suspend fun KSendFile(source: String, contactSource: String): String

    suspend fun KRemoteFileInfo(source: String, contactSource: String): String

    // new friend request operation
    suspend fun KNfroperation(text: String): String

    // Group invite operation
    suspend fun KGioperation(text: String): String

    suspend fun KAnnouncement(identify: String, source: String?): String

    suspend fun kQueryImgInfo(source: String): String

    // 通用接口
    fun KOperation(content: String): String
}
