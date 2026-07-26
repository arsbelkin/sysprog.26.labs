#pragma once


struct MessageBelkin;


class ISenderBelklin {
public:
	virtual void send(MessageBelkin& msg) const = 0;
};


class IReceiverBelkin {
public:
	virtual void receive(MessageBelkin& msg) const = 0;
};