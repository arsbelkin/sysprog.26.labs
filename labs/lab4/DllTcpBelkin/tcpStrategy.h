#pragma once
#include "../PlusiBelkin/Messages.h"
#include <queue>
#include <condition_variable>
#include <map>
#include <mutex>

#include "boost/asio.hpp"

using boost::asio::ip::tcp;


class __declspec(dllexport) SRSocketBelkin : public ISenderBelklin, public IReceiverBelkin {
public:
	int id;

	SRSocketBelkin(int id = -2) : id(id) {};
	~SRSocketBelkin() = default;

	static std::mutex mx;
	static std::condition_variable cv;
	static std::queue<MessageBelkin> qMsg;
	static std::map<int, std::shared_ptr<tcp::socket>> sockets;
	static int i;

	static boost::asio::io_context io_context;

	static std::shared_ptr<tcp::socket> clientSocket;
	static boost::asio::io_context client_io_context;

	virtual void send(MessageBelkin& msg) const;
	virtual void receive(MessageBelkin& msg) const;

	static void sendSocket(tcp::socket& socket, MessageBelkin& msg);
	static MessageBelkin receiveSocket(tcp::socket& socket);

	static void worker(std::shared_ptr<tcp::socket> socket, int clientID);
};
