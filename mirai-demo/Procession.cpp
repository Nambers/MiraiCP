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
		procession.registerEvent(lambda) ��ʼ�����¼��������ظ�����
		procession.registerEvent([](GroupMessageEvent param){ \*����*\});�Ǽ���Ⱥ��Ϣ
		procession.registerEvent([](PrivateMessageEvent param){ \*����*\});�Ǽ���Ⱥ��Ϣ
		...
	��������param�������lambda����ʹ��param.xxx������
	*/

	procession->registerEvent([](GroupMessageEvent param)->void {
		//����д���Լ�����Ⱥ��Ϣ�Ĵ���
		logger->Info("hi");
		param.group.SendMsg(param.sender.at()+param.message);
		});
	procession->registerEvent([](PrivateMessageEvent param)->void {
		//����д���Լ�����˽����Ϣ�Ĵ���
		logger->Info("hi");
		param.sender.SendMsg(param.message);
		vector<string> temp = Image::GetImgIdFromMiraiCode(param.message);
		for (int i = 0; i < temp.size(); i++) {
			logger->Info(temp[i]);
			logger->Info("ͼƬ���ص�ַ:" + Image(param.env, temp[i]).queryURL());
		}
		});
	procession->registerEvent([](GroupInviteEvent param)->bool{
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
}