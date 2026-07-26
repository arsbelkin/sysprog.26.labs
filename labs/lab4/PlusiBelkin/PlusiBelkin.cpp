#include <iostream>
#include <mutex>
#include "SRBroker.h"
#include <fstream>

#include "../DllTcpBelkin/sharpStrategy.h"
#include "../DllTcpBelkin/tcpStrategy.h"

using namespace std;


int main()
{
    std::locale::global(std::locale("rus_rus.866"));
    wcin.imbue(std::locale());
	wcout.imbue(std::locale());

	if (!initDll()) {
		wcout << L"сервер не запущен" << endl;
		return 1;
	}

	while (true)
	{
		MessageBelkin msg = MessageBelkin::receiveMessage(SRSocketBelkin());

		switch (msg.header.to)
		{
		case -2:
			switch (msg.header.messageType)
			{
			case MT_CONNECT:
				SRBrokerBelkin::addThread(msg.header.from);

				MessageBelkin::sendMessage(SRSocketBelkin(msg.header.from), MT_INFO,
					to_wstring(msg.header.from),
					msg.header.from, msg.header.to);
				break;

			case MT_DISCONNECT:
				MessageBelkin::sendMessage(SRBrokerBelkin(msg.header.from), MT_DISCONNECT);
				
				MessageBelkin::sendMessage(SRSocketBelkin(msg.header.from), MT_DISCONNECT,
					L"",
					msg.header.from, msg.header.to);
				break;

			case MT_INFO:
				MessageBelkin::sendMessage(SRSocketBelkin(msg.header.from), MT_INFO,
					SRBrokerBelkin::getSessionsIDs(),
					msg.header.from, msg.header.to);
				break;
			}

			break;

		case -1:
			for (const auto& s : SRBrokerBelkin::sessions) {
				if (s.first != msg.header.from) {
					msg.header.to = s.first;
					msg.send(SRBrokerBelkin());
				}
			}

			MessageBelkin::sendMessage(SRSocketBelkin(msg.header.from), MT_INFO,
				SRBrokerBelkin::getSessionsIDs(),
				msg.header.from, msg.header.to);
			break;

		default:
			msg.send(SRBrokerBelkin());

			MessageBelkin::sendMessage(SRSocketBelkin(msg.header.from), MT_INFO,
				SRBrokerBelkin::getSessionsIDs(),
				msg.header.from, msg.header.to);
			break;
		}
	}
	SRBrokerBelkin::waitThreads();

	closeDll();

    return 0;
}
