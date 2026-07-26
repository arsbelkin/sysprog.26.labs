#include "pch.h"
#include "tcpStrategy.h"
#include "sharpStrategy.h"

using namespace std;


std::mutex SRSocketBelkin::mx;
std::condition_variable SRSocketBelkin::cv;
std::queue<MessageBelkin> SRSocketBelkin::qMsg;
std::map<int, std::shared_ptr<tcp::socket>> SRSocketBelkin::sockets;
int SRSocketBelkin::i = 0;

boost::asio::io_context SRSocketBelkin::io_context;

std::shared_ptr<tcp::socket> SRSocketBelkin::clientSocket = nullptr;
boost::asio::io_context SRSocketBelkin::client_io_context;


void SRSocketBelkin::send(MessageBelkin& msg) const {
	if (id < -1) {
		lock_guard lg(mx);
		qMsg.push(msg);

		cv.notify_one();
	}
	else if (id == -1) {
		sendSocket(*clientSocket, msg);
	}
	else {
		lock_guard lg(mx);
		auto socket = sockets.at(msg.header.from);
		sendSocket(*socket, msg);
	}
}


void SRSocketBelkin::receive(MessageBelkin& msg) const {
	unique_lock<mutex> lock(mx);
	cv.wait(lock, [] { return !qMsg.empty(); });

	msg = qMsg.front();
	qMsg.pop();
}


void SRSocketBelkin::sendSocket(tcp::socket& socket, MessageBelkin& msg) {
	boost::system::error_code error;

	boost::asio::write(socket, boost::asio::buffer(&msg.header, sizeof(msg.header)), error);

	if (error)
		throw boost::system::system_error(error);

	if (msg.header.size > 0)
	{
		boost::asio::write(socket, boost::asio::buffer(msg.data.c_str(), msg.header.size), error);
	}

	if (error)
		throw boost::system::system_error(error);
}


MessageBelkin SRSocketBelkin::receiveSocket(tcp::socket& socket) {
	boost::system::error_code error;

	MessageHeaderBelkin h;
	boost::asio::read(socket, boost::asio::buffer(&h, sizeof(h)), error);

	if (error)
		throw boost::system::system_error(error);

	if (h.size < 0)
		return MessageBelkin(h.to, h.from, static_cast<MessageTypesBelkin>(h.messageType), L"");

	wstring str;
	str.resize(h.size / sizeof(wchar_t));

	boost::asio::read(socket, boost::asio::buffer(str.data(), h.size), error);

	if (error)
		throw boost::system::system_error(error);

	return MessageBelkin(h.to, h.from, static_cast<MessageTypesBelkin>(h.messageType), str);
}


void SRSocketBelkin::worker(std::shared_ptr<tcp::socket> socket, int clientID) {
	try
	{
		while (true) {
			MessageBelkin msg = receiveSocket(*socket);
			msg.header.from = clientID;
			msg.send(SRSocketBelkin());
		}
	}
	catch (const boost::system::system_error& e)
	{
		lock_guard lg(mx);
		sockets.erase(clientID);
	}
}
