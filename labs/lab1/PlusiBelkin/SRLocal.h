#pragma once
#include "Sessions.h"
#include <vector>
#include <map>


class SRLocalBelkin : public ISenderBelklin, IReceiverBelkin {
public:
	int id;

	SRLocalBelkin(int id = -1) : id(id) {};

	static std::vector<HANDLE> hThreads;
	static std::map<int, std::shared_ptr<SessionBelkin>> sessions;
	static std::mutex mx;

	virtual void send(MessageBelkin& msg) const;
	virtual void receive(MessageBelkin& msg) const;

	static DWORD WINAPI worker(LPVOID lpParam);
	static void addThread(int sessionID);
	static void waitThreads();
};