#pragma once

#include <network/ServerInterface.hpp>

#include "PeecTrackerHandlers.hpp"

class TrackerServer : public Net::ServerInterface<MessageTypes, MessageStatus> {
private:
	Net::HandlerMediator<MessageTypes, MessageStatus> mediator;
	std::shared_ptr<ContainerTackerServer> container = std::make_shared<ContainerTackerServer>();

public:
	explicit TrackerServer(const uint16_t& _port, const std::uint8_t& _countThread);

	void OnMessage(std::shared_ptr<Net::OwnerMessage<MessageTypes, MessageStatus>> _ownMsg) override;

	void OnConnect(std::shared_ptr<Net::Connection<MessageTypes, MessageStatus>> _handleClient) override;

	void OnDisconnect(std::shared_ptr<Net::Connection<MessageTypes, MessageStatus>> _handleClient) override;
};
