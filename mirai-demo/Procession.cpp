#include "pch.h"
void onEnable() {
	/*�������*/
}
void onDisable() {
	/*�������*/
}
void EventRegister() {
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

	procession->registerEvent([](GroupMessageEvent param)->void {
		//����д���Լ�����Ⱥ��Ϣ�Ĵ���
		logger->Info("hi");
		param.group.SendMsg(param.sender.at());
		});
	procession->registerEvent([](PrivateMessageEvent param)->void {
		//����д���Լ�����˽����Ϣ�Ĵ���
		logger->Info("hi");
		param.sender.SendMsg(param.message);
		});
	procession->registerEvent([](GroupInviteEvent param)->bool {
		//����Ⱥ���룬trueͬ���Ⱥ,false��ͬ��
		if (param.sender.id == 11111) {
			return true;
		}
		return true;
		});
	procession->registerEvent([](NewFriendRequestEvent param)->bool {
		//�º�������
		logger->Info("�º�������������" + to_string(param.sender.id));
		if (param.message == "hhh") {
			return false;
		}
		return true;
		});
	procession->registerEvent([](MemberJoinEvent param)->void {
		logger->Info(to_string(param.member.id) + "���뱾Ⱥ");
		param.group.SendMsg("��ӭ" + param.member.nameCard + "���뱾Ⱥ");
		});
}