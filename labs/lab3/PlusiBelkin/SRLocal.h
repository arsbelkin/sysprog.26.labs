#pragma once
#include "Sessions.h"
#include <vector>
#include <map>
#include <thread>


class SRLocalBelkin : public ISenderBelklin, IReceiverBelkin {
public:
	int id;

	SRLocalBelkin(int id = -1) : id(id) {};

	static std::vector<std::thread> threads;
	static std::map<int, std::shared_ptr<SessionBelkin>> sessions;
	static std::mutex mx;

	static std::wstring path;

	virtual void send(MessageBelkin& msg) const;
	virtual void receive(MessageBelkin& msg) const;

	static void worker(int sessionID);
	static void addThread(int sessionID);
	static void waitThreads();
};
