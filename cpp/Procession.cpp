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
		e.group.SendMiraiCode(At(e.sender));
		});
	

}
void onDisable() {
	/*�������,�����˳��Ż����*/
}