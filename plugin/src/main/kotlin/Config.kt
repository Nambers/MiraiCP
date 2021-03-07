package org.example.mirai.plugin

class Config {
    //��Ϣ�¼�
    data class GroupMessage(
        val groupid: Long,
        val senderid: Long,
        val message: String,
        val Source: String,
        val type: Int = 1
    )
    data class PrivateMessage(
        val senderid: Long,
        val message: String,
        val Source: String,
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
    //Ⱥ��Ա�˳�
    data class MemberLeave(
        val groupid: Long,
        val memberid: Long,
        /*
        kick - 1
        quit - 2
         */
        val leavetype: Int,
        //���û����Ϊ0
        val operatorid: Long = 0,
        val type: Int = 6
    )
    //����
    data class RecallEvent(
        val Etype: Int,
        val authorid: Long,
        val operatorid: Long,
        val ids: String,
        val internalids: String,
        val time: Int,
        val groupid: Long = 0,
        val type: Int = 7
    )
}