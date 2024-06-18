#pragma once

#include <network/ServerInterface.hpp>
#include <filesystem/FileSystem.hpp>

#include "PeecServerPeerHandlers.hpp"
#include "PeecServerFileContainer.hpp"

class ServerIMPL : public Net::ServerInterface<MessageTypes, MessageStatus>
{
private:
	Net::HandlerMediator<MessageTypes, MessageStatus> mediator;
	std::shared_ptr<FileContainerMap> container = std::make_shared<FileContainerMap>();

	std::thread executeThread;
#ifdef _DEBUG
	std::vector<std::string> debugC;
#endif

public:
	explicit ServerIMPL(const uint16_t& _port, const std::uint8_t& _threadCount) : Net::ServerInterface<MessageTypes, MessageStatus>(_port), mediator(_threadCount)
	{
		mediator.RegisterHandler(MessageTypes::DownloadFile, std::make_unique<DownloadHandler>(container));
	}

	~ServerIMPL()
	{
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
				while (true)
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

	void OnMessage(std::shared_ptr<Net::OwnerMessage<MessageTypes, MessageStatus>> _ownMsg) override;

	void OnConnect(std::shared_ptr<Net::Connection<MessageTypes, MessageStatus>> _handleClient) override;

	void OnDisconnect(std::shared_ptr<Net::Connection<MessageTypes, MessageStatus>> _handleClient) override;
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
	 
inline void ServerIMPL::OnMessage(std::shared_ptr<Net::OwnerMessage<MessageTypes, MessageStatus>> _ownMsg)
{
	if (!_ownMsg->remoteMsg.GetStrData().empty())
	{
		spdlog::info("Message Request: {0}", _ownMsg->remoteMsg.GetStrData());
	}

	mediator.HandleMessage(_ownMsg);
}

inline void ServerIMPL::OnConnect(std::shared_ptr<Net::Connection<MessageTypes, MessageStatus>> _handleClient)
{
	spdlog::info("Client connection: {0}:{1}", _handleClient->GetAddressRemote(), _handleClient->GetPortRemote());
}

inline void ServerIMPL::OnDisconnect(std::shared_ptr<Net::Connection<MessageTypes, MessageStatus>> _handleClient)
{
	spdlog::info("Client Disconnect");
}