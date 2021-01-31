package org.example.mirai.plugin

class Config {
    //��Ϣ�¼�
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
    //Ⱥ����
    data class GroupInvite(
        val groupid: Long,
        val invitorid: Long,
        val type: Int = 3
    )
    //��������
    data class NewFriendRequest(
        val friendid: Long,
        val message: String,
        val type: Int = 4
    )
    //Ⱥ��Ա����
    data class MemberJoin(
        val groupid: Long,
        val memberid: Long,
        /*
        invite - 1
        active - 2
        retrieve - 3
         */
        val jointype: Int,
        //���û����Ϊ0
        val invitorid: Long = 0,
        val type: Int = 5
    )
}