#include "pch.h"
#include<algorithm>

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
	procession->registerEvent([](RecallEvent e) {
		e.init();
		if (e.type == 2) {
			Group g = Group(e.groupid);
			g.init();
			g.SendMsg(to_string(e.operatorid) + "������" + 
				to_string(e.authorid) + "��һ����Ϣ");
			
		}
		});
	procession->registerEvent([](GroupMessageEvent e) {
		e.init();
		e.group.SendMsg(e.group.name);
		e.group.SendMsg(e.group.MemberListToString());
		});
	

}
void onDisable() {
	/*�������,�����˳��Ż����*/
}