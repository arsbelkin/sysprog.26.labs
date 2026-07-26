#include <iostream>
#include <windows.h>
#include <mutex>
#include "SRLocal.h"
#include <fstream>

#include "../DLLMMFBelkin/mmfStategy.h"

using namespace std;


int main()
{
    std::locale::global(std::locale("rus_rus.866"));
    wcin.imbue(std::locale());
    wcout.imbue(std::locale());

    HANDLE hConfirmEvent = CreateEvent(NULL, FALSE, FALSE, L"ConfirmEvent");
	HANDLE hMsgEvent = CreateEvent(NULL, FALSE, FALSE, L"MsgEvent");

	int i = 0;
	bool flag = true;

	while (flag)
	{
		WaitForSingleObject(hMsgEvent, INFINITE);

		MessageBelkin msg = MessageBelkin::receiveMessage(SRMapBelkin());

		switch (msg.header.to)
		{
		case -2:
			switch (msg.header.messageType)
			{
			case MT_START:
				SRLocalBelkin::addThread(i);
				i++;
				SetEvent(hConfirmEvent);
				break;

			case MT_STOP:
				if (i) {
					MessageBelkin::sendMessage(SRLocalBelkin(--i), MT_STOP);
					SetEvent(hConfirmEvent);
				}
				else {
					flag = false;
				}
				break;

			case MT_CLOSE:
				while (i > 0) {
					MessageBelkin::sendMessage(SRLocalBelkin(--i), MT_STOP);
				}
				flag = false;
				break;

			case MT_DATA:
				lock_guard lg(SRLocalBelkin::mx);
				wcout << msg.data << endl;
				SetEvent(hConfirmEvent);
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

			SetEvent(hConfirmEvent);
			break;

		default:
			msg.send(SRLocalBelkin());

			SetEvent(hConfirmEvent);
			break;
		}
	}
	SRLocalBelkin::waitThreads();

	SetEvent(hConfirmEvent);

    return 0;
}
