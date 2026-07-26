#include "pch.h"
#include "mmfStategy.h"
#include "memory"


HANDLE SRMapBelkin::hFileMap = NULL;
HANDLE SRMapBelkin::hMutex = NULL;
HANDLE SRMapBelkin::hMsgEvent = NULL;


int SRMapBelkin::receiveMsg(MessageHeaderBelkin* outHeader, wchar_t* outBuffer) {
    WaitForSingleObject(SRMapBelkin::hMutex, INFINITE);

    HANDLE hFileMapR = OpenFileMapping(FILE_MAP_READ, FALSE, L"DllMap");
    if (!hFileMapR) {
        ReleaseMutex(SRMapBelkin::hMutex);
        return -1;
    }

    BYTE* buff = (BYTE*)MapViewOfFile(hFileMapR, FILE_MAP_READ, 0, 0, 0);
    if (!buff) {
        CloseHandle(hFileMapR);
        ReleaseMutex(SRMapBelkin::hMutex);
        return -1;
    }

    MessageHeaderBelkin* hInMemory = (MessageHeaderBelkin*)buff;
    if (outHeader) {
        *outHeader = *hInMemory;
    }

    int dataSizeInBytes = hInMemory->size;
    int charsCount = dataSizeInBytes / sizeof(wchar_t);

    if (outBuffer == nullptr) {
        UnmapViewOfFile(buff);
        CloseHandle(hFileMapR);
        ReleaseMutex(SRMapBelkin::hMutex);
        return charsCount;
    }

    memcpy(outBuffer, buff + sizeof(MessageHeaderBelkin), dataSizeInBytes);
    outBuffer[charsCount] = L'\0';

    UnmapViewOfFile(buff);
    CloseHandle(hFileMapR);
    ReleaseMutex(SRMapBelkin::hMutex);

    return charsCount;
}


void SRMapBelkin::receive(MessageBelkin& msg) const {
    int charsCount = receiveMsg(&msg.header, nullptr);

    if (charsCount > 0) {
        std::unique_ptr<wchar_t[]> buffer = std::make_unique<wchar_t[]>(charsCount + 1);

        receiveMsg(&msg.header, buffer.get());

        msg.data = buffer.get();
    }
}


void SRMapBelkin::send(MessageBelkin& msg) const {
    sendMsg(msg);
}


void SRMapBelkin::sendMsg(MessageBelkin& msg) {
    WaitForSingleObject(SRMapBelkin::hMutex, INFINITE);

    if (SRMapBelkin::hFileMap) {
        CloseHandle(SRMapBelkin::hFileMap);
        SRMapBelkin::hFileMap = NULL;
    }

    SRMapBelkin::hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, msg.header.size + sizeof(MessageHeaderBelkin), L"DllMap");

    if (!SRMapBelkin::hFileMap) {
        ReleaseMutex(SRMapBelkin::hMutex);
        return;
    }

    BYTE* buff = (BYTE*)MapViewOfFile(SRMapBelkin::hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, msg.header.size + sizeof(MessageHeaderBelkin));

    if (!buff) {
        CloseHandle(SRMapBelkin::hFileMap);
        SRMapBelkin::hFileMap = NULL;
        ReleaseMutex(SRMapBelkin::hMutex);
        return;
    }

    memcpy(buff, &msg.header, sizeof(MessageHeaderBelkin));
    memcpy(buff + sizeof(MessageHeaderBelkin), msg.data.c_str(), msg.header.size);

    UnmapViewOfFile(buff);

    SetEvent(SRMapBelkin::hMsgEvent);
    ReleaseMutex(SRMapBelkin::hMutex);
}
