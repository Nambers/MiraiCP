package org.example.mirai.plugin

class Config {
    data class GroupMessage(
        val groupid: Long,
        val senderid: Long,
        val message: String,
        val type: Int = 1
    )
    class PrivateMessage(
        val senderid: Long,
        val message: String,
        val type: Int = 2
    )
}