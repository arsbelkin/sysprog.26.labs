#include "pch.h"
#include "sharpStrategy.h"
#include "tcpStrategy.h"

#include <Windows.h>

#include <iostream>

#include <combaseapi.h>

using namespace std;


bool initClient() {
	if (SRSocketBelkin::clientSocket && SRSocketBelkin::clientSocket->is_open())
		return true;

	try {
		if (!SRSocketBelkin::clientSocket) {
			SRSocketBelkin::clientSocket = std::make_shared<tcp::socket>(SRSocketBelkin::client_io_context);
		}

		if (!SRSocketBelkin::clientSocket->is_open()) {
			tcp::resolver r(SRSocketBelkin::client_io_context);
			boost::asio::connect(*SRSocketBelkin::clientSocket, r.resolve("127.0.0.1", "12345"));
		}

		return true;
	}
	catch (const boost::system::system_error& e) {
		if (SRSocketBelkin::clientSocket) {
			SRSocketBelkin::clientSocket->close();
		}
		return false;
	}
	catch (...) {
		return false;
	}
}


extern "C"
{
	bool initDll() {
		try {
			auto a = make_shared<tcp::acceptor>(SRSocketBelkin::io_context, tcp::endpoint(tcp::v4(), 12345));
			
			std::thread([a]()
				{
					while (true)
					{
						try
						{
							auto socket = std::make_shared<tcp::socket>(SRSocketBelkin::io_context);
							a->accept(*socket);
							int ID;
							{
								lock_guard lg(SRSocketBelkin::mx);
								ID = SRSocketBelkin::i++;
								SRSocketBelkin::sockets[ID] = socket;
							}
							std::thread(SRSocketBelkin::worker, socket, ID).detach();
						}
						catch (const boost::system::system_error& e) {

						}
					}
				}
			).detach();

			return true;
		}
		catch (const boost::system::system_error& e) {
			return false;
		}
	}

	void closeDll() {
		try {
			SRSocketBelkin::io_context.stop();

			{
				std::lock_guard lg(SRSocketBelkin::mx);
				for (auto& pair : SRSocketBelkin::sockets) {
					if (pair.second && pair.second->is_open()) {
						pair.second->close();
					}
				}
				SRSocketBelkin::sockets.clear();
			}
		}
		catch (...)
		{

		}
	}

	void sendMsgFromSharp(MsgStructBelkin msg) {
		if (!initClient())
			return;

		try
		{
			MessageBelkin m(msg.to, 0, msg.msgType, std::wstring(msg.data));
			m.send(SRSocketBelkin(-1));
		}
		catch (const boost::system::system_error& e)
		{
			if (SRSocketBelkin::clientSocket)
				SRSocketBelkin::clientSocket->close();
		}
	}


	BOOL receiveMsgFromSharp(MsgStructBelkin* msg) {
		try {
			if (!SRSocketBelkin::clientSocket || !SRSocketBelkin::clientSocket->is_open()) {
				return false;
			}

			MessageBelkin m = SRSocketBelkin::receiveSocket(*SRSocketBelkin::clientSocket);
			msg->to = m.header.to;
			msg->msgType = static_cast<MessageTypesBelkin>(m.header.messageType);

			if (msg->msgType == MT_DISCONNECT) {
				SRSocketBelkin::clientSocket->close();
				return false;
			}
			
			size_t sizeInBytes = (m.data.length() + 1) * sizeof(wchar_t);
			wchar_t* buffer = (wchar_t*)CoTaskMemAlloc(sizeInBytes);

			if (!buffer) return false;

			wcscpy_s(buffer, m.data.length() + 1, m.data.c_str());

			msg->data = buffer;

			return true;
		}
		catch (...) {
			return false;
		}
	}
}
