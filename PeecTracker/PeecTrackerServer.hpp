#pragma once

#include <PeecServerInterface.hpp>

#include "PeecTrackerHandlers.hpp"

class TrackerServer : public Net::ServerInterface<MessageTypes> {
private:
	Net::HandlerMediator<MessageTypes> mediator;
	std::shared_ptr<ContainerTackerServer> container = std::make_shared<ContainerTackerServer>();

public:
	explicit TrackerServer(const uint16_t& _port, const std::uint8_t& _countThread);

	void OnMessage(std::shared_ptr<Net::OwnerMessage<MessageTypes>> _ownMsg) override;

	void OnConnect(std::shared_ptr<Net::Connection<MessageTypes>> _handleClient) override;

	void OnDisconnect(std::shared_ptr<Net::Connection<MessageTypes>> _handleClient) override;
};
