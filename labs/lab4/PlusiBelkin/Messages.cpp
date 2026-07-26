#include "Messages.h"

using namespace std;


MessageBelkin::MessageBelkin(MessageTypesBelkin messageType, const std::wstring& data) : data(data) {
	header = { messageType, int(data.length() * sizeof(wchar_t)) };
}

void MessageBelkin::sendMessage(const ISenderBelklin& sender, MessageTypesBelkin messageType, const std::wstring& data, int to, int from) {
	MessageBelkin msg(to, from, messageType, data);
	msg.send(sender);
}

MessageBelkin MessageBelkin::receiveMessage(const IReceiverBelkin& receiver) {
	MessageBelkin msg;
	msg.receive(receiver);
	return msg;
}
