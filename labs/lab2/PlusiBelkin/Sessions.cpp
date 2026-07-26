#include "Sessions.h"

using namespace std;


SessionBelkin::SessionBelkin(int sessionID) : sessionID(sessionID) {
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}


SessionBelkin::~SessionBelkin() {
	CloseHandle(hEvent);
}


void SessionBelkin::addMessage(MessageBelkin& msg) {
	lock_guard lg(mx);
	messages.push(msg);
	SetEvent(hEvent);
}


bool SessionBelkin::getMessage(MessageBelkin& msg) {
	bool res = false;
	WaitForSingleObject(hEvent, INFINITE);

	lock_guard lg(mx);

	if (!messages.empty()) {
		msg = messages.front();
		messages.pop();
		res = true;
	}

	if (messages.empty()) {
		ResetEvent(hEvent);
	}

	return res;
}