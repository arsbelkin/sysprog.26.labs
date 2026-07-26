#include "SRLocal.h"
#include <iostream>
#include <fstream>

using namespace std;


std::vector<std::thread> SRLocalBelkin::threads;
std::map<int, std::shared_ptr<SessionBelkin>> SRLocalBelkin::sessions;
std::mutex SRLocalBelkin::mx;

std::wstring SRLocalBelkin::path = L"./msgs";


void SRLocalBelkin::send(MessageBelkin& msg) const {
	lock_guard lg(mx);

	if (id < 0)
		sessions[msg.header.to]->addMessage(msg);
	else
		sessions[id]->addMessage(msg);
}


void SRLocalBelkin::receive(MessageBelkin& msg) const {
	if (!sessions[id]->getMessage(msg))
		throw runtime_error("No messages!");
}


void SRLocalBelkin::worker(int sessionID)
{
	auto session = make_shared<SessionBelkin>(sessionID);

	mx.lock();
	CreateDirectory(path.c_str(), NULL);
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
			case MT_STOP:
			{
				lock_guard lg(mx);
				sessions.erase(sessionID);
				wcout << L"session " << sessionID + 1 << L" closed" << endl;
				return;
			}
			case MT_DATA:
			{
				lock_guard lg(mx);
				wofstream out(path + L"/" + std::to_wstring(sessionID + 1) + L".txt", ios::app);
				out.imbue(std::locale(""));

				if (out.is_open()) {
					out << msg.data << endl;
				}

				break;
			}
		}
	}
}


void SRLocalBelkin::addThread(int sessionID) {
	threads.push_back(std::move(thread(worker, sessionID)));
}


void SRLocalBelkin::waitThreads() {
	if (threads.empty())
		return;

	for (auto& t : threads)
	{
		t.join();
	}

	threads.clear();
}
