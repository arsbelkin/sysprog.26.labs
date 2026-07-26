#include "SRBroker.h"
#include <iostream>
#include <fstream>

#include "../DllTcpBelkin/tcpStrategy.h"

using namespace std;


std::map<int, std::jthread> SRBrokerBelkin::threads;
std::map<int, std::shared_ptr<SessionBelkin>> SRBrokerBelkin::sessions;
std::mutex SRBrokerBelkin::mx;

std::wstring SRBrokerBelkin::path = L"./msgs";


void SRBrokerBelkin::send(MessageBelkin& msg) const {
	lock_guard lg(mx);

	if (id < 0)
		sessions[msg.header.to]->addMessage(msg);
	else
		sessions[id]->addMessage(msg);
}


void SRBrokerBelkin::receive(MessageBelkin& msg) const {
	if (!sessions[id]->getMessage(msg))
		throw runtime_error("No messages!");
}


std::wstring SRBrokerBelkin::getSessionsIDs() {
	lock_guard lg(mx);

	if (sessions.empty())
		return L"";

	std::wstringstream ss;
	for (const auto& s : sessions)
		ss << s.first << L",";

	return ss.str();
}


void SRBrokerBelkin::writeInFile(MessageBelkin& msg) {
	wofstream out(path + L"/" + std::to_wstring(msg.header.to + 1) + L".txt", ios::app);
	out.imbue(std::locale(""));

	if (out.is_open()) {
		out << msg.data << endl;
	}
}


void SRBrokerBelkin::worker(int sessionID)
{
	auto session = make_shared<SessionBelkin>(sessionID);

	mx.lock();
	CreateDirectory(path.c_str(), NULL);
	sessions[sessionID] = session;
	mx.unlock();

	{
		lock_guard lg(mx);
		wcout << L"client " << session->sessionID << L" connected" << endl;
	}

	while (true)
	{
		MessageBelkin msg = MessageBelkin::receiveMessage(SRBrokerBelkin(sessionID));

		switch (msg.header.messageType)
		{
		case MT_DISCONNECT:
		{
			lock_guard lg(mx);
			sessions.erase(sessionID);
			wcout << L"client " << sessionID << L" disconnected" << endl;
			return;
		}
		case MT_DATA:
		{
			{
				lock_guard lg(mx);
				writeInFile(msg);
			}

			msg.send(SRSocketBelkin(sessionID));
			break;
		}
		}
	}
}


void SRBrokerBelkin::addThread(int sessionID) {
	threads.try_emplace(sessionID, worker, sessionID);
}


void SRBrokerBelkin::waitThreads() {
	threads.clear();
}
