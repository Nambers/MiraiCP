/*
 * Copyright (c) 2020 - 2021. Eritque arcus and contributors.
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
@file:Suppress("INVISIBLE_MEMBER", "INVISIBLE_REFERENCE")

package group

import net.mamoe.mirai.Bot
import net.mamoe.mirai.internal.message.OnlineMessageSourceFromGroupImpl
import net.mamoe.mirai.internal.network.protocol.data.proto.MsgComm
import net.mamoe.mirai.message.data.OnlineMessageSource
import net.mamoe.mirai.utils.MiraiInternalApi

private inline fun String.forEachHexChunkIndexed(block: (index: Int, char1: Char, char2: Char) -> Unit) {
    var index = 0
    forEachHexChunk { char1: Char, char2: Char ->
        block(index++, char1, char2)
    }
}

private inline fun String.forEachHexChunk(block: (char1: Char, char2: Char) -> Unit) {
    var chunkSize = 0
    var char1: Char = 0.toChar()
    for ((index, c) in this.withIndex()) { // compiler optimization
        if (c == ' ') {
            if (chunkSize != 0) {
                throw IllegalArgumentException("Invalid size of chunk at index ${index.minus(1)}")
            }
            continue
        }
        if (c in 'a'..'f' || c in 'A'..'F' || c in '0'..'9') { // compiler optimization
            when (chunkSize) {
                0 -> {
                    chunkSize = 1
                    char1 = c
                }
                1 -> {
                    block(char1, c)
                    chunkSize = 0
                }
            }
        } else {
            throw IllegalArgumentException("Invalid char '$c' at index $index")
        }
    }
    if (chunkSize != 0) {
        throw IllegalArgumentException("Invalid size of chunk at end of string")
    }
}

fun Byte.Companion.parseFromHexChunk(char1: Char, char2: Char): Byte {
    return (char1.digitToInt(16).shl(SIZE_BITS / 2) or char2.digitToInt(16)).toByte()
}

fun String.countHexBytes(): Int {
    var chunkSize = 0
    var count = 0
    for ((index, c) in this.withIndex()) {
        if (c == ' ') {
            if (chunkSize != 0) {
                throw IllegalArgumentException("Invalid size of chunk at index ${index.minus(1)}")
            }
            continue
        }
        if (c in 'a'..'f' || c in 'A'..'F' || c in '0'..'9') {
            when (chunkSize) {
                0 -> {
                    chunkSize = 1
                }
                1 -> {
                    count++
                    chunkSize = 0
                }
            }
        } else {
            throw IllegalArgumentException("Invalid char '$c' at index $index")
        }
    }
    if (chunkSize != 0) {
        throw IllegalArgumentException("Invalid size of chunk at end of string")
    }
    return count
}

fun String.hexToBytes(): ByteArray {
    val array = ByteArray(countHexBytes())
    forEachHexChunkIndexed { index, char1, char2 ->
        array[index] = Byte.parseFromHexChunk(char1, char2)
    }
    return array
}

// This function and above utils tools are from net.mamoe.mirai
@OptIn(MiraiInternalApi::class)
fun source(b: Bot, sender: Long, groupid: Long): OnlineMessageSource = OnlineMessageSourceFromGroupImpl(
    b,
    listOf(
        MsgComm.Msg(
            msgHead = MsgComm.MsgHead(
                fromUin = sender,
                toUin = b.id,
                msgType = 82,
                msgSeq = 1628,
                msgTime = 1629,
                msgUid = 1441,
                groupInfo = MsgComm.GroupInfo(
                    groupCode = groupid,
                    groupType = 1,
                    groupInfoSeq = 624,
                    groupCard = "user3",
                    groupLevel = 1,
                    groupCardType = 2,
                    groupName = "testtest".toByteArray(), /* 74 65 73 74 74 65 73 74 */
                ),
                fromAppid = 1,
                fromInstid = 1,
                userActive = 1,
            ),
            contentHead = MsgComm.ContentHead(
                pkgNum = 1,
            ),
            msgBody = net.mamoe.mirai.internal.network.protocol.data.proto.ImMsgBody.MsgBody(
                richText = net.mamoe.mirai.internal.network.protocol.data.proto.ImMsgBody.RichText(
                    attr = net.mamoe.mirai.internal.network.protocol.data.proto.ImMsgBody.Attr(
                        codePage = 0,
                        time = 162,
                        random = -313,
                        size = 9,
                        effect = 0,
                        charSet = 134,
                        pitchAndFamily = 0,
                        fontName = "微软雅黑",
                    ),
                    elems = mutableListOf(
                        net.mamoe.mirai.internal.network.protocol.data.proto.ImMsgBody.Elem(
                            text = net.mamoe.mirai.internal.network.protocol.data.proto.ImMsgBody.Text(
                                str = "123123123",
                            ),
                        ), net.mamoe.mirai.internal.network.protocol.data.proto.ImMsgBody.Elem(
                            elemFlags2 = net.mamoe.mirai.internal.network.protocol.data.proto.ImMsgBody.ElemFlags2(
                                msgRptCnt = 1,
                            ),
                        ), net.mamoe.mirai.internal.network.protocol.data.proto.ImMsgBody.Elem(
                            generalFlags = net.mamoe.mirai.internal.network.protocol.data.proto.ImMsgBody.GeneralFlags(
                                pbReserve = "08 01 20 CB 50 80 01 01 C8 01 00 F0 01 00 F8 01 00 90 02 00 98 03 00 A0 03 00 B0 03 00 C0 03 00 D0 03 00 E8 03 00 8A 04 02 10 02 90 04 80 01 B8 04 00 C0 04 00 CA 04 00 F8 04 00 88 05 00".hexToBytes(),
                            ),
                        ), net.mamoe.mirai.internal.network.protocol.data.proto.ImMsgBody.Elem(
                            extraInfo = net.mamoe.mirai.internal.network.protocol.data.proto.ImMsgBody.ExtraInfo(
                                nick = "user3",
                                level = 1,
                                groupMask = 1,
                            ),
                        )
                    ),
                ),
            ),
        )
    )
)