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
	procession->registerEvent([](GroupMessageEvent param)->void {
		//����д���Լ�����Ⱥ��Ϣ�Ĵ���
		logger->Info("hi");
		param.group.SendMsg(param.sender.at());
		});
	procession->registerEvent([=](SchedulingEvent e) {
		if (e.id == 1) {
			/*do something*/
			Friend(groupid).SendMsg("���Ƕ�ʱ����");
		}
		});
	procession->registerEvent([&](PrivateMessageEvent param)->void {
		//����д���Լ�����˽����Ϣ�Ĵ���
		logger->Info("hi");
		SetScheduling(100, 1);
		groupid = param.sender.id;
		param.sender.SendMsg(param.message);
		});
	procession->registerEvent([](GroupInviteEvent param)->bool {
		//����Ⱥ���룬trueͬ���Ⱥ,false��ͬ��
		if (param.sender.id == 11111) {
			return ACCEPT;
		}
		return ACCEPT;
		});
	procession->registerEvent([](NewFriendRequestEvent param)->bool {
		//�º�������
		logger->Info("�º�������������" + to_string(param.sender.id));
		if (param.message == "hhh") {
			return ACCEPT;
		}
		return ACCEPT;
		});
	procession->registerEvent([](MemberLeaveEvent p) {
		p.group.SendMsg(p.member.nameCard + "�뿪�˱�Ⱥ");
		});
	procession->registerEvent([](MemberJoinEvent param)->void {
		if (param.type == INVITE) {
			//�ó�Ա�Ǳ��������ģ�������param.invitor���������Ͷ�û��
		}
		else if (param.type == ACTIVE) {
			//�ó�Ա�����������
		}
		logger->Info(to_string(param.member.id) + "���뱾Ⱥ");
		param.group.SendMsg("��ӭ" + param.member.nameCard + "���뱾Ⱥ");
		});
}
void onDisable() {
	/*�������*/
}