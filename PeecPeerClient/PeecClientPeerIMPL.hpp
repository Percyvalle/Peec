#pragma once

#include <network/ClientInterface.hpp>
#include <network/ServerInterface.hpp>
#include <filesystem/FileSystem.hpp>

#include <PeecStructMessage.hpp>

class ClientIMPL : public Net::ClientInterface<Net::PeecMessage>
{
public:
	
	Net::OWN_MSG_PTR<Net::PeecMessage> GETRequest(MessageTypes _type, bool _wait);
	Net::OWN_MSG_PTR<Net::PeecMessage> POSTRequest(MessageTypes _type, const JSON& _data, bool _wait);

	Net::OWN_MSG_PTR<Net::PeecMessage> WaitingResponse();
};

[[maybe_unused]] inline Net::OWN_MSG_PTR<Net::PeecMessage> ClientIMPL::GETRequest(MessageTypes _type, bool _wait = true)
{
	Net::PeecMessage msg;
	msg.SetType(_type);
	
	Send(msg);

	return _wait ? WaitingResponse() : nullptr;
}

[[maybe_unused]] inline Net::OWN_MSG_PTR<Net::PeecMessage> ClientIMPL::POSTRequest(MessageTypes _type, const JSON& _data, bool _wait = true)
{
	Net::PeecMessage msg;
	msg.SetType(_type);
	msg << _data.dump();

	Send(msg);

	return _wait ? WaitingResponse() : nullptr;
}

[[maybe_unused]] inline Net::OWN_MSG_PTR<Net::PeecMessage> ClientIMPL::WaitingResponse()
{
	Incoming().wait();
	return Incoming().pop_front();
}
