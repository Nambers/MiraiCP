#include "pch.h"

const std::string MiraiCPVersion = "v2.5.0-patch-2";
/*需要表态的返回值*/

static const bool REJECT = false;
static const bool ACCEPT = true;

/*群成员退出事件*/

//被踢出
static const int T_KICK = 1;
//主动退出
static const int T_QUIT = 2;

/*新群成员事件*/

/*被邀请进入*/
static const int T_INVITE = 1;
/*主动加入*/
static const int T_ACTIVE = 2;
/*旧群主恢复群主位置*/
static const int T_Retrieve = 3;

/*群成员权限*/
//群主
static const int P_Owner = 2;
//管理员
static const int P_Administer = 1;
//群成员
static const int P_Member = 0;

/*
LightApp风格1
*/
struct LightAppStyle1{
	//小图标地址
	std::string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//小标题，用逗号分割多个
	std::string titles = "{\"title\":\"1\", \"value\":\"t1\"},"
					"{\"title\":\"2\", \"value\":\"t2\"}";
	//下面的按钮，但是不能按，用逗号分割多个
	std::string buttons = "{\"name\":\"T1\",\"action\":\"\"}"
					"{\"name\":\"T2\",\"action\":\"\"}";
	//大标题
	std::string appName = "test1";
	//小标题的标题
	std::string title = "test2";
};

struct LightAppStyle2 {
	//小图标
	std::string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//预览图大图
	std::string preview = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//小图片右侧小标题
	std::string title = "Test1";
	//图片上标题
	std::string title2 = "Test2";
};

struct LightAppStyle3 {
	//点击跳转链接
	std::string url = "https://www.baidu.com";
	std::string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//预览图
	std::string preview = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
	//大标题
	std::string title = "Test1";
	//图片旁描述
	std::string description = "Test2";
};