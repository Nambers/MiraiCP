package org.example.mirai.plugin

class Config {
    data class GroupMessage(
        val groupid: Long,
        val senderid: Long,
        val message: String,
        val type: Int = 1
    )
    data class PrivateMessage(
        val senderid: Long,
        val message: String,
        val type: Int = 2
    )
    data class GroupInvite(
        val groupid: Long,
        val invitorid: Long,
        val type: Int = 3
    )
    data class NewFriendRequest(
        val friendid: Long,
        val message: String,
        val type: Int = 4
    )
}