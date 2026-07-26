#include "SRLocal.h"
#include <iostream>

using namespace std;


void SRLocalBelkin::send(MessageBelkin& msg) const {
	sessions[id]->addMessage(msg);
}


void SRLocalBelkin::receive(MessageBelkin& msg) const {
	if (!sessions[id]->getMessage(msg))
		throw runtime_error("No messages!");
}


DWORD WINAPI SRLocalBelkin::worker(LPVOID lpParam)
{
	int sessionID = (int)lpParam;

	auto session = make_shared<SessionBelkin>(sessionID);

	mx.lock();
	sessions[sessionID] = session;
	mx.unlock();

	{
		lock_guard lg(mx);
		wcout << L"session " << session->sessionID + 1 << L" created" << endl;
	}
	
	while (true)
	{
		MessageBelkin msg = MessageBelkin::receiveMessage(SRLocalBelkin(sessionID));

		switch (msg.header.messageType)
		{
			case MT_CLOSE:
			{
				lock_guard lg(mx);
				sessions.erase(sessionID);
				wcout << L"session " << sessionID + 1 << L" closed" << endl;
				return 0;
			}
			case MT_DATA:
			{
				wcout << L"session " << sessionID + 1 << L" data " << msg.data << endl;
				Sleep(500 * sessionID);
				break;
			}
		}
	}
}


void SRLocalBelkin::addThread(int sessionID) {
	hThreads.push_back(CreateThread(NULL, 0, worker, (LPVOID)sessionID, 0, NULL));
}


void SRLocalBelkin::waitThreads() {
	for (auto& h : hThreads)
		CloseHandle(h);
}
