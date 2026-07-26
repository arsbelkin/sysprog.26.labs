#include <iostream>
#include <mutex>
#include "SRLocal.h"
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

	int i = 0;
	bool flag = true;


	while (flag)
	{
		MessageBelkin msg = MessageBelkin::receiveMessage(SRSocketBelkin());

		switch (msg.header.to)
		{
		case -2:
			switch (msg.header.messageType)
			{
			case MT_START:
				SRLocalBelkin::addThread(i++);

				MessageBelkin::sendMessage(SRSocketBelkin(msg.header.from), MT_INFO,
					to_wstring(i),
					msg.header.to, msg.header.from);
				break;

			case MT_STOP:
				if (i > 0)
					MessageBelkin::sendMessage(SRLocalBelkin(--i), MT_STOP);
				
				MessageBelkin::sendMessage(SRSocketBelkin(msg.header.from), MT_INFO,
					to_wstring(i),
					msg.header.to, msg.header.from);
				break;

			case MT_INFO:
				MessageBelkin::sendMessage(SRSocketBelkin(msg.header.from), MT_INFO,
					to_wstring(i),
					msg.header.to, msg.header.from);
				break;

			case MT_DATA:
				lock_guard lg(SRLocalBelkin::mx);
				wcout << msg.data << endl;

				MessageBelkin::sendMessage(SRSocketBelkin(msg.header.from), MT_INFO,
					to_wstring(i),
					msg.header.to, msg.header.from);
				break;
			}

			break;

		case -1:
			{
				lock_guard lg(SRLocalBelkin::mx);
				wcout << msg.data << endl;
			}

			for (int j = 0; j < i; ++j) {
				msg.send(SRLocalBelkin(j));
			}

			MessageBelkin::sendMessage(SRSocketBelkin(msg.header.from), MT_INFO,
				to_wstring(i),
				msg.header.to, msg.header.from);
			break;

		default:
			msg.send(SRLocalBelkin());

			MessageBelkin::sendMessage(SRSocketBelkin(msg.header.from), MT_INFO,
				to_wstring(i),
				msg.header.to, msg.header.from);
			break;
		}
	}
	SRLocalBelkin::waitThreads();

	closeDll();

    return 0;
}
