#include "pch.h"

void func() {
	//���¸�ֵgenv��ȷ���������
	getEnv();

	/*ִ�в���*/
	Friend(1111).SendMsg("hi");

	//��jvm�������̣߳�envʧЧ
	gvm->DetachCurrentThread();
}
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
		thread th1(func);
		th1.join();
		});
	

}
void onDisable() {
	/*�������,�����˳��Ż����*/
}