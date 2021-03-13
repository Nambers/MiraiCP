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
		logger->Info(e.message);
		e.group.SendMsg(e.message);
		SetScheduling(100, {e.message, to_string(e.group.id)});
		});
	procession->registerEvent([](SchedulingEvent e) {
		Group(stoi(e.ids[1])).SendMsg(e.ids[0]);
		});
	

}
void onDisable() {
	/*�������,�����˳��Ż����*/
}