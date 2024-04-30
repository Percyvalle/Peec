#pragma once

#include <PeecClientInterface.hpp>
#include <PeecServerInterface.hpp>
#include <PeecMessageTypes.hpp>
#include <PeecFilesystem.hpp>

class ClientIMPL : public Net::ClientInterface<MessageTypes>
{
public:

	Net::OWN_MSG_PTR<MessageTypes> GETFilesList();
	Net::OWN_MSG_PTR<MessageTypes> GETFileLocation(const JSON& _data);

	Net::OWN_MSG_PTR<MessageTypes> RegistrationFile(const JSON& _data);

	Net::OWN_MSG_PTR<MessageTypes> WaitingResponse();
};

inline Net::OWN_MSG_PTR<MessageTypes> ClientIMPL::GETFilesList()
{
	Net::Message<MessageTypes> msg;
	msg.SetType(MessageTypes::GetFileList);

	Send(msg);

	return WaitingResponse();
}

inline Net::OWN_MSG_PTR<MessageTypes> ClientIMPL::GETFileLocation(const JSON& _data)
{
	Net::Message<MessageTypes> msg;
	msg.SetType(MessageTypes::FileLocation);
	msg << _data.dump();

	Send(msg);

	return WaitingResponse();
}

inline Net::OWN_MSG_PTR<MessageTypes> ClientIMPL::RegistrationFile(const JSON& _data)
{
	Net::Message<MessageTypes> msg;
	msg.SetType(MessageTypes::FileRegistration);
	msg << _data.dump();

	Send(msg);

	return WaitingResponse();
}

inline Net::OWN_MSG_PTR<MessageTypes> ClientIMPL::WaitingResponse()
{
	Incoming().wait();
	return Incoming().pop_front();
}
