#include "pch.h"

const std::string MiraiCPVersion = "v2.5.0-patch-2";
/*��Ҫ��̬�ķ���ֵ*/

static const bool REJECT = false;
static const bool ACCEPT = true;

/*Ⱥ��Ա�˳��¼�*/

//���߳�
static const int T_KICK = 1;
//�����˳�
static const int T_QUIT = 2;

/*��Ⱥ��Ա�¼�*/

/*���������*/
static const int T_INVITE = 1;
/*��������*/
static const int T_ACTIVE = 2;
/*��Ⱥ���ָ�Ⱥ��λ��*/
static const int T_Retrieve = 3;

/*Ⱥ��ԱȨ��*/
//Ⱥ��
static const int P_Owner = 2;
//����Ա
static const int P_Administer = 1;
//Ⱥ��Ա
static const int P_Member = 0;

/*
LightApp���1
*/
struct LightAppStyle1{
	//Сͼ���ַ
	std::string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//С���⣬�ö��ŷָ���
	std::string titles = "{\"title\":\"1\", \"value\":\"t1\"},"
					"{\"title\":\"2\", \"value\":\"t2\"}";
	//����İ�ť�����ǲ��ܰ����ö��ŷָ���
	std::string buttons = "{\"name\":\"T1\",\"action\":\"\"}"
					"{\"name\":\"T2\",\"action\":\"\"}";
	//�����
	std::string appName = "test1";
	//С����ı���
	std::string title = "test2";
};

struct LightAppStyle2 {
	//Сͼ��
	std::string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//Ԥ��ͼ��ͼ
	std::string preview = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//СͼƬ�Ҳ�С����
	std::string title = "Test1";
	//ͼƬ�ϱ���
	std::string title2 = "Test2";
};

struct LightAppStyle3 {
	//�����ת����
	std::string url = "https://www.baidu.com";
	std::string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//Ԥ��ͼ
	std::string preview = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//�����
	std::string title = "Test1";
	//ͼƬ������
	std::string description = "Test2";
};