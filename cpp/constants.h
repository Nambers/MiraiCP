#include "pch.h"


/*��Ҫ��̬�ķ���ֵ*/

static const bool REJECT = false;
static const bool ACCEPT = true;

/*Ⱥ��Ա�˳��¼�*/

//���߳�
static const int KICK = 1;
//�����˳�
static const int QUIT = 2;

/*��Ⱥ��Ա�¼�*/

/*���������*/
static const int INVITE = 1;
/*��������*/
static const int ACTIVE = 2;
/*��Ⱥ���ָ�Ⱥ��λ��*/
static const int Retrieve = 3;

/*Ⱥ��ԱȨ��*/
//Ⱥ��
static const int Owner = 2;
//����Ա
static const int Administer = 1;
//Ⱥ��Ա
static const int Mamber = 0;

/*
LightApp���1
*/
struct LightAppStyle1{
	//Сͼ���ַ
	string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//С���⣬�ö��ŷָ���
	string titles = "{\"title\":\"1\", \"value\":\"t1\"},"
					"{\"title\":\"2\", \"value\":\"t2\"}";
	//����İ�ť�����ǲ��ܰ����ö��ŷָ���
	string buttons = "{\"name\":\"T1\",\"action\":\"\"}"
					"{\"name\":\"T2\",\"action\":\"\"}";
	//�����
	string appName = "test1";
	//С����ı���
	string title = "test2";
};

struct LightAppStyle2 {
	//Сͼ��
	string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//Ԥ��ͼ��ͼ
	string preview = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//СͼƬ�Ҳ�С����
	string title = "Test1";
	//ͼƬ�ϱ���
	string title2 = "Test2";
};

struct LightAppStyle3 {
	//�����ת����
	string url = "https://www.baidu.com";
	string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//Ԥ��ͼ
	string preview = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//�����
	string title = "Test1";
	//ͼƬ������
	string description = "Test2";
};