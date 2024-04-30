#pragma once

#include <PeecHandler.hpp>
#include <PeecMessageTypes.hpp>
#include <PeecCommonHeaders.hpp>
#include <PeecFilesystem.hpp>

#include "PeecContainer.hpp"

struct ServerPingHandler : public Net::MessageHandler<MessageTypes> 
{
	Net::Message<MessageTypes> handle(Net::OWN_MSG_PTR<MessageTypes> _msg) override
	{
		return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::ServerPing, MessageStatus::SUCCESS);
	}
};

struct FileRegistrationHandler : public Net::MessageHandler<MessageTypes>
{
	std::shared_ptr<ContainerTackerServer> containerServer;

	explicit FileRegistrationHandler(std::shared_ptr<ContainerTackerServer> _containerServer) : containerServer(_containerServer) {}

	Net::Message<MessageTypes> handle(Net::OWN_MSG_PTR<MessageTypes> _msg) override
	{
		JSON jsonRequestData = JSON::parse(_msg->remoteMsg.GetStrData());
		if (containerServer->FileIsExist(jsonRequestData["FILENAME"]))
		{
			return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::FileRegistration, 
																	MessageStatus::SUCCESS, 
																	JSON::parse(R"({"MESSAGE":"The file is already registered"})").dump());
		}

		std::size_t countChunks = CalculateChunkCount(jsonRequestData["FILELENGTH"]);
		FileInfo newFile{jsonRequestData["FILENAME"], countChunks, jsonRequestData["FILELENGTH"]};
		PeerInfo newPeer{_msg->remoteConnection->GetAddressRemote(), _msg->remoteConnection->GetPortRemote()};

		containerServer->AddFile(newFile, newPeer);

		return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::FileRegistration, MessageStatus::SUCCESS);
	}
};

struct FileLocationHandler : public Net::MessageHandler<MessageTypes> 
{
	std::shared_ptr<ContainerTackerServer> containerServer;

	explicit FileLocationHandler(std::shared_ptr<ContainerTackerServer> _containerServer) : containerServer(_containerServer) {}

	Net::Message<MessageTypes> handle(Net::OWN_MSG_PTR<MessageTypes> _msg) override
	{
		JSON jsonRequestData = JSON::parse(_msg->remoteMsg.GetStrData());
		if (containerServer->FileIsExist(jsonRequestData["FILENAME"]))
		{
			return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::FileLocation, MessageStatus::SUCCESS, 
																	containerServer->GetFileLocationJSON(jsonRequestData["FILENAME"]).dump());
		}
		return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::FileLocation, MessageStatus::FAILURE);
	}
};

struct GetFileListHandler : public Net::MessageHandler<MessageTypes>
{
	std::shared_ptr<ContainerTackerServer> containerServer;

	explicit GetFileListHandler(std::shared_ptr<ContainerTackerServer> _containerServer) : containerServer(_containerServer) {}

	Net::Message<MessageTypes> handle(Net::OWN_MSG_PTR<MessageTypes> _msg) override
	{
		if (!containerServer->filesList.empty())
		{
			return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::GetFileList, MessageStatus::SUCCESS, containerServer->GetFilesJSON().dump());
		}
		return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::GetFileList, MessageStatus::FAILURE);
	}
};