#pragma once
#include "Sessions.h"
#include <map>
#include <thread>

#include "boost/asio.hpp"

using boost::asio::ip::tcp;


class SRBrokerBelkin : public ISenderBelklin, IReceiverBelkin {
public:
	int id;

	SRBrokerBelkin(int id = -1) : id(id) {};

	static std::map<int, std::jthread> threads;
	static std::map<int, std::shared_ptr<SessionBelkin>> sessions;
	static std::mutex mx;

	static std::wstring path;

	virtual void send(MessageBelkin& msg) const;
	virtual void receive(MessageBelkin& msg) const;

	static void worker(int sessionID, std::shared_ptr<tcp::socket> socket);
	static void addSession(int sessionID, std::shared_ptr<tcp::socket> socket);
	static void waitThreads();

	static void writeInFile(MessageBelkin& msg);
	static std::wstring getSessionsIDs();
};
