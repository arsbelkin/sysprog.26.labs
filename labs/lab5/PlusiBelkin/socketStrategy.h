#pragma once
#include "boost/asio.hpp"
#include "Messages.h"

using boost::asio::ip::tcp;


class SRSocketBelkin : public ISenderBelklin, public IReceiverBelkin {
public:
	std::shared_ptr<tcp::socket> socket;

	SRSocketBelkin(std::shared_ptr<tcp::socket> sct) : socket(sct) {};
	~SRSocketBelkin() = default;

	static int sessionID;

	virtual void send(MessageBelkin& msg) const;
	virtual void receive(MessageBelkin& msg) const;

	static void sendSocket(std::shared_ptr<tcp::socket> socket, MessageBelkin& msg);
	static MessageBelkin receiveSocket(std::shared_ptr<tcp::socket> socket);

	static void processClient(std::shared_ptr<tcp::socket> socket, int id);
};
