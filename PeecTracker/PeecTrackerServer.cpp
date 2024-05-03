#include "PeecTrackerServer.hpp"

TrackerServer::TrackerServer(const uint16_t& _port, const std::uint8_t& _countThread = 1) : Net::ServerInterface<MessageTypes>(_port), mediator(_countThread)
{
	mediator.RegisterHandler(MessageTypes::ServerPing, std::make_unique<ServerPingHandler>());
	mediator.RegisterHandler(MessageTypes::FileLocation, std::make_unique<FileLocationHandler>(container));
	mediator.RegisterHandler(MessageTypes::FileRegistration, std::make_unique<FileRegistrationHandler>(container));
	mediator.RegisterHandler(MessageTypes::GetFileList, std::make_unique<GetFileListHandler>(container));
}

void TrackerServer::OnMessage(std::shared_ptr<Net::OwnerMessage<MessageTypes>> _ownMsg)
{
	if (!_ownMsg->remoteMsg.GetStrData().empty())
	{
		spdlog::info("Message Request: {0}", _ownMsg->remoteMsg.GetStrData());
	}

	mediator.HandleMessage(_ownMsg);
}

void TrackerServer::OnConnect(std::shared_ptr<Net::Connection<MessageTypes>> _handleClient)
{
	spdlog::info("Client connection: {0}:{1}", _handleClient->GetAddressRemote(), _handleClient->GetPortRemote());
}

void TrackerServer::OnDisconnect(std::shared_ptr<Net::Connection<MessageTypes>> _handleClient)
{
	spdlog::info("Client Disconnect");
}