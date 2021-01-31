#include "pch.h"


/*需要表态的返回值*/

static const bool REJECT = false;
static const bool ACCEPT = true;

/*新群成员事件*/

/*被邀请进入*/
static const int INVITE = 1;
/*主动加入*/
static const int ACTIVE = 2;
/*旧群主恢复群主位置*/
static const int Retrieve = 3;