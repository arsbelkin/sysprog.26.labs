#include "pch.h"
#include "MMFBelkin.h"
#include "mmfStategy.h"


extern "C"
{
    bool initDll() {
        SRMapBelkin::hMutex = CreateMutex(NULL, FALSE, L"DllMutex");
        SRMapBelkin::hMsgEvent = CreateEvent(NULL, FALSE, FALSE, L"MsgEvent");

        return (SRMapBelkin::hMutex != NULL && SRMapBelkin::hMsgEvent != NULL);
    }

    void closeDll() {
        if (SRMapBelkin::hFileMap) {
            CloseHandle(SRMapBelkin::hFileMap);
            SRMapBelkin::hFileMap = NULL;
        }

        if (SRMapBelkin::hMsgEvent) {
            CloseHandle(SRMapBelkin::hMsgEvent);
            SRMapBelkin::hMsgEvent = NULL;
        }

        if (SRMapBelkin::hMutex) {
            CloseHandle(SRMapBelkin::hMutex);
            SRMapBelkin::hMutex = NULL;
        }
    }

    void sendMsgFromSharp(MsgStructBelkin msg) {
        MessageBelkin m(msg.to, static_cast<MessageTypesBelkin>(msg.msgType), std::wstring(msg.data));
        SRMapBelkin::sendMsg(m);
    }
}
