#pragma once
#include "../PlusiBelkin/Messages.h"


struct MsgStructBelkin {
	int to;
	MessageTypesBelkin msgType;
	wchar_t* data;
};


bool initClient();

extern "C" {
	__declspec(dllexport) bool initDll();
	__declspec(dllexport) void closeDll();

	__declspec(dllexport) void sendMsgFromSharp(MsgStructBelkin msg);
	__declspec(dllexport) BOOL receiveMsgFromSharp(MsgStructBelkin* msg);
}
