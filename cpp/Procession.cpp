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
	procession->registerEvent([](GroupInviteEvent e) {
		try {
			e.init();
		}catch (MiraiCPException error) {
			/*�������ֻ�ܻ�ȡ�Ƿ����쳣������Ҫ��ϸ�ֵ��쳣�ಶ�����֪��������ʲô�쳣*/
			logger->Error(error.what());
		}
		//����ʾ��: ��Ϊ��ǰ�¼���δ��Ⱥ�����Է�����Ϣ�ᱨ��
		e.group.SendMsg("����");
		return false;
		});

}
void onDisable() {
	/*�������,�����˳��Ż����*/
}