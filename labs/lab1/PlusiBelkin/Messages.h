#pragma once
#include <string>
#include "Interfaces.h"


enum MessageTypesBelkin {
	MT_CLOSE,
	MT_DATA
};


struct MessageHeaderBelkin {
	int messageType;
	int size;
};


struct MessageBelkin {
	MessageHeaderBelkin header = { 0 };
	std::wstring data;
	MessageBelkin() = default;

	MessageBelkin(MessageTypesBelkin messageType, const std::wstring& data = L"");
	void send(const ISenderBelklin& sender);
	void receive(const IReceiverBelkin& receiver);

	static void sendMessage(const ISenderBelklin& sender, MessageTypesBelkin messageType, const std::wstring& data = L"");
	static MessageBelkin receiveMessage(const IReceiverBelkin& receiver);
};