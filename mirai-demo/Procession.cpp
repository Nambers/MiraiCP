#include "pch.h"

class procession: public Procession{
public:
	void GroupMessage(Group g, Friend f) {
		g.SendMsg("hi");
	}
	void PrivateMessage(Friend f) {
		f.SendMsg("hi");
	}
};