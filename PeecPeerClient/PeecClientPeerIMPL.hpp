#pragma once

#include <PeecClientInterface.hpp>
#include <PeecServerInterface.hpp>
#include <PeecMessageTypes.hpp>
#include <PeecFilesystem.hpp>

class ClientIMPL : public Net::ClientInterface<MessageTypes>
{
public:
	
	Net::OWN_MSG_PTR<MessageTypes> GETRequest(MessageTypes _type);
	Net::OWN_MSG_PTR<MessageTypes> POSTRequest(MessageTypes _type, const JSON& _data);

	Net::OWN_MSG_PTR<MessageTypes> WaitingResponse();
};

inline Net::OWN_MSG_PTR<MessageTypes> ClientIMPL::GETRequest(MessageTypes _type)
{
	Net::Message<MessageTypes> msg;
	msg.SetType(_type);
	
	Send(msg);

	return WaitingResponse();
}

inline Net::OWN_MSG_PTR<MessageTypes> ClientIMPL::POSTRequest(MessageTypes _type, const JSON& _data)
{
	Net::Message<MessageTypes> msg;
	msg.SetType(_type);
	msg << _data.dump();

	Send(msg);

	return WaitingResponse();
}

inline Net::OWN_MSG_PTR<MessageTypes> ClientIMPL::WaitingResponse()
{
	Incoming().wait();
	return Incoming().pop_front();
}
