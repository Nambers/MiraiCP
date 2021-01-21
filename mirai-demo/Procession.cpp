#include "pch.h"
class procession: public Procession{
	//主要处理位置
public:
	void GroupMessage(Group g, Friend f, string message) {
		g.SendMsg(message);
		logger.Info(message);
	}
	void PrivateMessage(Friend f, string message) {
		f.SendMsg(message);
	}
	string GroupInvite(Group g, Friend f) {
		return accept;
	}
	string NewFrinedRequest(Friend f, string message) {
		return accept;
	}
};