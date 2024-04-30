#pragma once

#include <PeecServerInterface.hpp>
#include <PeecFilesystem.hpp>

#include "PeecServerPeerHandlers.hpp"
#include "PeecServerFileContainer.hpp"

class ServerIMPL : public Net::ServerInterface<MessageTypes>
{
private:
	Net::HandlerMediator<MessageTypes> mediator;
	std::shared_ptr<FileContainerMap> container = std::make_shared<FileContainerMap>();

#ifdef _DEBUG
	std::vector<std::string> debugC;
#endif

public:
	explicit ServerIMPL(const uint16_t& _port, const std::uint8_t& _threadCount) : Net::ServerInterface<MessageTypes>(_port), mediator(_threadCount)
	{
		mediator.RegisterHandler(MessageTypes::DownloadFile, std::make_unique<DownloadHandler>(container));
	};

#ifdef _DEBUG
	std::vector<std::string> GetContainer() const { return debugC; }
#endif

	void AddRegisteredFile(const std::string& _fileName, const std::string& _filePath, const std::size_t& _fileLength);

	void OnMessage(std::shared_ptr<Net::OwnerMessage<MessageTypes>> _ownMsg) override;

	void OnConnect(std::shared_ptr<Net::Connection<MessageTypes>> _handleClient) override;

	void OnDisconnect(std::shared_ptr<Net::Connection<MessageTypes>> _handleClient) override;
};

inline void ServerIMPL::AddRegisteredFile(const std::string& _fileName, const std::string& _filePath, const std::size_t& _fileLength)
{
	container->emplace(std::piecewise_construct, 
					   std::forward_as_tuple(_fileName), 
					   std::forward_as_tuple(_fileName, _filePath, _fileLength));
#ifdef _DEBUG
	debugC.push_back(_fileName);
#endif
}

inline void ServerIMPL::OnMessage(std::shared_ptr<Net::OwnerMessage<MessageTypes>> _ownMsg)
{
	mediator.HandleMessage(_ownMsg);
}

inline void ServerIMPL::OnConnect(std::shared_ptr<Net::Connection<MessageTypes>> _handleClient)
{
}

inline void ServerIMPL::OnDisconnect(std::shared_ptr<Net::Connection<MessageTypes>> _handleClient)
{
}