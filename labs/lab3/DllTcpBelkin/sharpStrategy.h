#pragma once


struct MsgStructBelkin {
	int to;
	int msgType;
	wchar_t* data;
};


bool initClient();

extern "C" {
	__declspec(dllexport) bool initDll();
	__declspec(dllexport) void closeDll();

	__declspec(dllexport) void sendMsgFromSharp(MsgStructBelkin msg);
	__declspec(dllexport) int receiveMsgFromSharp();
}
