#pragma once

#include <network/ServerInterface.hpp>
#include <filesystem/FileSystem.hpp>

#include "PeecServerPeerHandlers.hpp"
#include "PeecServerFileContainer.hpp"

#include <PeecStructMessage.hpp>

class ServerIMPL : public Net::ServerInterface<Net::PeecMessage>
{
private:
	std::atomic_bool stopThread = false;
	Net::HandlerMediator<Net::PeecMessage, MessageTypes> mediator;
	std::shared_ptr<FileContainerMap> container = std::make_shared<FileContainerMap>();

	std::thread executeThread;
#ifdef _DEBUG
	std::vector<std::string> debugC;
#endif

public:
	explicit ServerIMPL(const uint16_t& _port, const std::uint8_t& _threadCount) : Net::ServerInterface<Net::PeecMessage>(_port), mediator(_threadCount)
	{
		mediator.RegisterHandler(MessageTypes::DownloadFile, std::make_unique<DownloadHandler>(container));
	}

	~ServerIMPL()
	{
		stopThread.store(true, std::memory_order_release);
		if (executeThread.joinable())
		{
			executeThread.join();
		}
	}

#ifdef _DEBUG
	std::vector<std::string> GetContainer() const { return debugC; }
#endif
		
	void ExecuteUpdateServer(bool _isThread = false) 
	{
		std::function executeFun = [this]()
			{
				while (!stopThread.load(std::memory_order_acquire))
				{
					Update();
					CheckClientConnection();
				}
			};

		if (_isThread)
		{
			executeThread = std::thread(executeFun);
		}
		else
		{
			executeFun();
		}

	};

	void AddRegisteredFile(const std::string& _fileName, const std::string& _filePath, const std::size_t& _fileLength);

	void OnMessage(std::shared_ptr<Net::OwnerMessage<Net::PeecMessage>> _ownMsg) override;

	void OnConnect(std::shared_ptr<Net::Connection<Net::PeecMessage>> _handleClient) override;

	void OnDisconnect(std::shared_ptr<Net::Connection<Net::PeecMessage>> _handleClient) override;
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
	 
inline void ServerIMPL::OnMessage(std::shared_ptr<Net::OwnerMessage<Net::PeecMessage>> _ownMsg)
{
	if (!_ownMsg->remoteMsg.ConvertToString().empty())
	{
		spdlog::info("Message Request: {0}", _ownMsg->remoteMsg.ConvertToString());
	}

	mediator.HandleMessage(_ownMsg);
}

inline void ServerIMPL::OnConnect(std::shared_ptr<Net::Connection<Net::PeecMessage>> _handleClient)
{
	spdlog::info("Client connection: {0}:{1}", _handleClient->GetAddressRemote(), _handleClient->GetPortRemote());
}

inline void ServerIMPL::OnDisconnect(std::shared_ptr<Net::Connection<Net::PeecMessage>> _handleClient)
{
	spdlog::info("Client Disconnect");
}