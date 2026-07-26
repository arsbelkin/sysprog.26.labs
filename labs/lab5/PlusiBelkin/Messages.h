#pragma once
#include <string>
#include "Interfaces.h"


enum MessageTypesBelkin {
	MT_CONNECT,
	MT_DISCONNECT,
	MT_INFO,
	MT_DATA
};


struct MessageHeaderBelkin {
	int messageType;
	int size;
	int to;
	int from;
};


struct MessageBelkin {
	MessageHeaderBelkin header = { 0 };
	std::wstring data;
	MessageBelkin() = default;

	MessageBelkin(MessageTypesBelkin messageType, const std::wstring& data = L"");
	MessageBelkin(int to, int from, MessageTypesBelkin messageType, const std::wstring& data) : data(data) {
		header = { messageType, int(data.length() * sizeof(wchar_t)), to, from};
	}

	void send(const ISenderBelklin& sender) {
		sender.send(*this);
	}

	void receive(const IReceiverBelkin& receiver) {
		receiver.receive(*this);
	}

	static void sendMessage(const ISenderBelklin& sender, MessageTypesBelkin messageType, const std::wstring& data = L"", int to = -2, int from = 0);
	static MessageBelkin receiveMessage(const IReceiverBelkin& receiver);
};
