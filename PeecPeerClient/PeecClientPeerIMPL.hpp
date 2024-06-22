#pragma once

#include <network/ClientInterface.hpp>
#include <network/ServerInterface.hpp>
#include <filesystem/FileSystem.hpp>

#include <PeecMessageTypes.hpp>
#include <PeecMessageStatus.hpp>

class ClientIMPL : public Net::ClientInterface<MessageTypes, MessageStatus>
{
public:
	
	Net::OWN_MSG_PTR<MessageTypes, MessageStatus> GETRequest(MessageTypes _type, bool _wait);
	Net::OWN_MSG_PTR<MessageTypes, MessageStatus> POSTRequest(MessageTypes _type, const JSON& _data, bool _wait);

	Net::OWN_MSG_PTR<MessageTypes, MessageStatus> WaitingResponse();
};

[[maybe_unused]] inline Net::OWN_MSG_PTR<MessageTypes, MessageStatus> ClientIMPL::GETRequest(MessageTypes _type, bool _wait = true)
{
	Net::Message<MessageTypes, MessageStatus> msg;
	msg.SetType(_type);
	
	Send(msg);

	return _wait ? WaitingResponse() : nullptr;
}

[[maybe_unused]] inline Net::OWN_MSG_PTR<MessageTypes, MessageStatus> ClientIMPL::POSTRequest(MessageTypes _type, const JSON& _data, bool _wait = true)
{
	Net::Message<MessageTypes, MessageStatus> msg;
	msg.SetType(_type);
	msg << _data.dump();

	Send(msg);

	return _wait ? WaitingResponse() : nullptr;
}

inline Net::OWN_MSG_PTR<MessageTypes, MessageStatus> ClientIMPL::WaitingResponse()
{
	Incoming().wait();
	return Incoming().pop_front();
}
