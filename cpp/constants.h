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