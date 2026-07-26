#pragma once
#include "windows.h"


struct MsgStructBelkin {
    int to;
    int msgType;
    wchar_t* data;
};


extern "C" {
    __declspec(dllexport) bool initDll();
    __declspec(dllexport) void closeDll();

    __declspec(dllexport) void sendMsgFromSharp(MsgStructBelkin msg);
}
