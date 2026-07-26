#include "./socketStrategy.h"
#include "./SRBroker.h"

#include <iostream>

using namespace std;


int SRSocketBelkin::sessionID = 0;


void SRSocketBelkin::send(MessageBelkin& msg) const {
	sendSocket(socket, msg);
}


void SRSocketBelkin::receive(MessageBelkin& msg) const {
	msg = receiveSocket(socket);
}


void SRSocketBelkin::sendSocket(std::shared_ptr<tcp::socket> socket, MessageBelkin& msg) {
	boost::system::error_code error;

	boost::asio::write(*socket, boost::asio::buffer(&msg.header, sizeof(msg.header)), error);

	if (error)
		throw boost::system::system_error(error);

	if (msg.header.size > 0)
	{
		boost::asio::write(*socket, boost::asio::buffer(msg.data.c_str(), msg.header.size), error);
	}

	if (error)
		throw boost::system::system_error(error);
}


MessageBelkin SRSocketBelkin::receiveSocket(std::shared_ptr<tcp::socket> socket) {
	boost::system::error_code error;

	MessageHeaderBelkin h;
	boost::asio::read(*socket, boost::asio::buffer(&h, sizeof(h)), error);

	if (error)
		throw boost::system::system_error(error);

	if (h.size < 0)
		return MessageBelkin(h.to, 0, static_cast<MessageTypesBelkin>(h.messageType), L"");

	wstring str;
	str.resize(h.size / sizeof(wchar_t));

	boost::asio::read(*socket, boost::asio::buffer(str.data(), h.size), error);

	if (error)
		throw boost::system::system_error(error);

	return MessageBelkin(h.to, 0, static_cast<MessageTypesBelkin>(h.messageType), str);
}


void SRSocketBelkin::processClient(std::shared_ptr<tcp::socket> socket, int id) {
	while (true)
	{
		MessageBelkin msg;
		try {
			msg = MessageBelkin::receiveMessage(SRSocketBelkin(socket));
		}
		catch (const boost::system::system_error& e) {
			MessageBelkin::sendMessage(SRBrokerBelkin(id), MT_DISCONNECT);
			return;
		}
		
		msg.header.from = id;

		switch (msg.header.to)
		{
		case -2:
			switch (msg.header.messageType)
			{
			case MT_CONNECT:
				SRBrokerBelkin::addSession(msg.header.from, socket);

				try {
					MessageBelkin::sendMessage(SRSocketBelkin(socket), MT_INFO,
						to_wstring(msg.header.from),
						msg.header.from, msg.header.to);
				}
				catch (const boost::system::system_error& e) {
					MessageBelkin::sendMessage(SRBrokerBelkin(id), MT_DISCONNECT);
					return;
				}
				break;

			case MT_DISCONNECT:
				MessageBelkin::sendMessage(SRBrokerBelkin(msg.header.from), MT_DISCONNECT);
				return;

			case MT_INFO:
				MessageBelkin(id, -2, MT_INFO, SRBrokerBelkin::getSessionsIDs()).send(SRBrokerBelkin());
				break;
			}
			break;

		case -1:
			for (const auto& s : SRBrokerBelkin::sessions)
			{
				msg.header.to = s.first;
				msg.send(SRBrokerBelkin());
			}
				
			MessageBelkin(id, -2, MT_INFO, SRBrokerBelkin::getSessionsIDs()).send(SRBrokerBelkin());
			break;

		default:
			msg.send(SRBrokerBelkin());
			MessageBelkin(id, -2, MT_INFO, SRBrokerBelkin::getSessionsIDs()).send(SRBrokerBelkin());
			break;
			
		break;
		}
	}
}