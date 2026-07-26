#include <iostream>
#include <windows.h>
#include <mutex>
#include "SRLocal.h"

using namespace std;


std::vector<HANDLE> SRLocalBelkin::hThreads;
std::map<int, std::shared_ptr<SessionBelkin>> SRLocalBelkin::sessions;
std::mutex SRLocalBelkin::mx;


int main()
{
    std::locale::global(std::locale("rus_rus.866"));
    wcin.imbue(std::locale());
    wcout.imbue(std::locale());

    HANDLE hStartEvent = CreateEvent(NULL, FALSE, FALSE, L"StartEvent");
    HANDLE hStopEvent = CreateEvent(NULL, FALSE, FALSE, L"StopEvent");
    HANDLE hConfirmEvent = CreateEvent(NULL, FALSE, FALSE, L"ConfirmEvent");
    HANDLE hCloseEvent = CreateEvent(NULL, FALSE, FALSE, L"CloseEvent");
    HANDLE hControlEvents[3] = { hStartEvent, hStopEvent, hCloseEvent };

	int i = 0;
	bool flag = true;


	while (flag)
	{
		int n = WaitForMultipleObjects(3, hControlEvents, FALSE, INFINITE) - WAIT_OBJECT_0;

		switch (n)
		{
		case 0:
			SRLocalBelkin::addThread(i);
			SetEvent(hConfirmEvent);
			i++;
			break;
		case 1:
			ResetEvent(hStopEvent);
			if (i) {
				MessageBelkin::sendMessage(SRLocalBelkin(--i), MT_CLOSE);
				SetEvent(hConfirmEvent);
			}
			else {
				flag = false;
			}

			break;
		case 2:
			while (i > 0) {
				MessageBelkin::sendMessage(SRLocalBelkin(--i), MT_CLOSE);
			}
			flag = false;
			break;
		}
	}
	SRLocalBelkin::waitThreads();

	SetEvent(hConfirmEvent);

    return 0;
}
