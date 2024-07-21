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

	bool Ping();
	Net::OWN_MSG_PTR<Net::PeecMessage> WaitingResponse(std::uint64_t _mlsec = 0);
};

[[maybe_unused]] inline Net::OWN_MSG_PTR<Net::PeecMessage> ClientIMPL::GETRequest(MessageTypes _type, bool _wait = true)
{
	Net::PeecMessage msg = Net::MessageFactory::CreateMessage(_type, MessageStatus::UNDEFINED);

	Send(msg);

	return _wait ? WaitingResponse() : nullptr;
}

[[maybe_unused]] inline Net::OWN_MSG_PTR<Net::PeecMessage> ClientIMPL::POSTRequest(MessageTypes _type, const JSON& _data, bool _wait = true)
{
	Net::PeecMessage msg = Net::MessageFactory::CreateMessage(_type, MessageStatus::UNDEFINED, _data.dump());

	Send(msg);

	return _wait ? WaitingResponse() : nullptr;
}

[[maybe_unused]] inline bool ClientIMPL::Ping()
{
	Net::PeecMessage msg = Net::MessageFactory::CreateMessage(MessageTypes::ServerPing, MessageStatus::UNDEFINED);

	Send(msg);
	
	Net::OWN_MSG_PTR<Net::PeecMessage> replyMsg = WaitingResponse(5000);

	if (replyMsg == nullptr) {
		return false;
	}

	return replyMsg->remoteMsg.GetStatus() == MessageStatus::SUCCESS ? true : false;
}

[[maybe_unused]] inline Net::OWN_MSG_PTR<Net::PeecMessage> ClientIMPL::WaitingResponse(std::uint64_t _mlsec)
{
	Incoming().wait(_mlsec);
	return !Incoming().empty() ? Incoming().pop_front() : nullptr;
}
