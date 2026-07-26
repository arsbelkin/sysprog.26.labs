#pragma once
#include <queue>
#include "Messages.h"
#include <mutex>
#include <windows.h>


class SessionBelkin {
private:
	std::queue<MessageBelkin> messages;
	std::mutex mx;
	HANDLE hEvent;
public:
	int sessionID;

	SessionBelkin(int sessionID);
	~SessionBelkin();

	void addMessage(MessageBelkin& msg);
	bool getMessage(MessageBelkin& msg);
};
