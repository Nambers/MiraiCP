#include "pch.h"
unsigned long groupid = 0;
void onEnable() {
	/*�������*/
	/*
	ע���¼�����-�û��Զ���
	logger - ��־���
		logger->Info(string)������Ϣ����־
		logger->Warning(string)���;��漶��־
		logger->Error(string)���ʹ�����־
	procession �㲥Դ
		procession->registerEvent(lambda) ע�����
		procession->registerEvent([](GroupMessageEvent param){ \*����*\});�Ǽ���Ⱥ��Ϣ
		procession->registerEvent([](PrivateMessageEvent param){ \*����*\});�Ǽ���˽����Ϣ
		...
	��������param�������lambda����ʹ��param.xxx������
	*/
	procession->registerEvent([](GroupMessageEvent e) {
		e.init();
		try {
			Member m = Member(481784853, e.group.id);
			m.init();
			m.Kick("this_is_reason");
		}
		catch (BotException err) {
			//Ȩ�޲���
			logger->Error(err.what());
		}
		catch (MemberException err) {
			if (err.type == 1) {
				//�Ҳ���Ⱥ
			}
			if (err.type == 2) {
				//�Ҳ���Ⱥ��Ա
			}
		}
		});

}
void onDisable() {
	/*�������,�����˳��Ż����*/
}