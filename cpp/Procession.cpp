#include "pch.h"
unsigned long groupid = 0;
void onEnable() {
	/*�������*/
	/*
	ע���¼�����-�û��Զ���
	logger - ��־���
		logger.Info(string)������Ϣ����־
		logger.Warning(string)���;��漶��־
		logger.Error(string)���ʹ�����־
	procession �㲥Դ
		procession.registerEvent(lambda) ע�����
		procession.registerEvent([](GroupMessageEvent param){ \*����*\});�Ǽ���Ⱥ��Ϣ
		procession.registerEvent([](PrivateMessageEvent param){ \*����*\});�Ǽ���Ⱥ��Ϣ
		...
	��������param�������lambda����ʹ��param.xxx������
	*/
	procession->registerEvent([](MemberLeaveEvent e) {
		try {
			//���ڵ�ǰ��Ա�˳���Ⱥ�ģ����Ի��׳�MemberException��Ϊ�Ҳ�������
			e.init();
			e.member.Mute(5);
		}
		catch (MemberException error) {
			e.group.SendMsg("ʧ��:" + error.what());
		}
		});
}
void onDisable() {
	/*�������,�����˳��Ż����*/
}