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
	procession->registerEvent([](GroupMessageEvent e) {
		logger->Info(to_string(e.sender.permission));
		if (e.sender.permission == Owner) {
			e.group.SendMsg("Owner");
		}
		if (e.sender.permission == Administer) {
			e.group.SendMsg("Administer");
		}
		if (e.sender.permission == Mamber) {
			e.group.SendMsg("Mamber");
		}
		});
}
void onDisable() {
	/*�������,�����˳��Ż����*/
}