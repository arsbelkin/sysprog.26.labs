#pragma once
#include "windows.h"
#include "../PlusiBelkin/Messages.h"


class __declspec(dllexport) SRMapBelkin : public ISenderBelklin, public IReceiverBelkin {
public:
	static HANDLE hFileMap;
	static HANDLE hMutex;
	static HANDLE hMsgEvent;

	SRMapBelkin() = default;
	~SRMapBelkin() = default;

	virtual void send(MessageBelkin& msg) const;
	virtual void receive(MessageBelkin& msg) const;

	static int receiveMsg(MessageHeaderBelkin* outHeader, wchar_t* outBuffer);
	static void sendMsg(MessageBelkin& msg);
};
