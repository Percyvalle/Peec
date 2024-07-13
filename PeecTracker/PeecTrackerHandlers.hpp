#pragma once

#include <network/Handler.hpp>
#include <filesystem/FileSystem.hpp>

#include <PeecStructMessage.hpp>

#include "PeecContainer.hpp"

struct ServerPingHandler : public Net::MessageHandler<Net::PeecMessage> 
{
	Net::PeecMessage handle(Net::OWN_MSG_PTR<Net::PeecMessage> _msg) override
	{
		return Net::MessageFactory::CreateMessage(MessageTypes::ServerPing, MessageStatus::SUCCESS);
	}
};

struct FileRegistrationHandler : public Net::MessageHandler<Net::PeecMessage>
{
	std::shared_ptr<ContainerTackerServer> containerServer;

	explicit FileRegistrationHandler(std::shared_ptr<ContainerTackerServer> _containerServer) : containerServer(_containerServer) {}

	Net::PeecMessage handle(Net::OWN_MSG_PTR<Net::PeecMessage> _msg) override
	{
		JSON jsonRequestData = _msg->remoteMsg.ConvertToJson();

		if (!Utils::ValidateExistsVarJSON(jsonRequestData, "FILENAME", "FILELENGTH", "ADDRESS", "PORT", nullptr))
		{
			return Net::MessageFactory::CreateMessage(MessageTypes::FileRegistration,
													  MessageStatus::FAILURE,
													  JsonMSGDump("MESSAGE", "Invalidate data"));
		}

		if (containerServer->FileExistsOnServer(jsonRequestData["FILENAME"]))
		{
			return Net::MessageFactory::CreateMessage(MessageTypes::FileRegistration,
													  MessageStatus::FAILURE,
													  JsonMSGDump("MESSAGE", "The file is already registered"));
		}

		std::size_t countChunks = CalculateChunkCount(jsonRequestData["FILELENGTH"]);
		FileInfo newFile{jsonRequestData["FILENAME"], countChunks, jsonRequestData["FILELENGTH"]};
		PeerInfo newPeer{jsonRequestData["ADDRESS"], jsonRequestData["PORT"]};

		containerServer->AddFile(newFile, newPeer);

		return Net::MessageFactory::CreateMessage(MessageTypes::FileRegistration, MessageStatus::SUCCESS);
	}
};

struct FileLocationHandler : public Net::MessageHandler<Net::PeecMessage>
{
	std::shared_ptr<ContainerTackerServer> containerServer;

	explicit FileLocationHandler(std::shared_ptr<ContainerTackerServer> _containerServer) : containerServer(_containerServer) {}

	Net::PeecMessage handle(Net::OWN_MSG_PTR<Net::PeecMessage> _msg) override
	{
		JSON jsonRequestData = _msg->remoteMsg.ConvertToJson();
		
		if (!Utils::ValidateExistsVarJSON(jsonRequestData, "FILENAME", nullptr))
		{
			return Net::MessageFactory::CreateMessage(MessageTypes::FileLocation,
													  MessageStatus::FAILURE,
													  JsonMSGDump("MESSAGE", "Invalidate data"));
		}
		
		if (containerServer->FileExistsOnServer(jsonRequestData["FILENAME"]))
		{
			JSON jsonResponse = containerServer->GetFileLocationJSON(jsonRequestData["FILENAME"]);
			jsonResponse.push_back(JSON::object());
			jsonResponse.back()["COUNT_CHUNK"] = containerServer->files[jsonRequestData["FILENAME"]].countChunks;

			return Net::MessageFactory::CreateMessage(MessageTypes::FileLocation,
																				   MessageStatus::SUCCESS, 
																				   jsonResponse.dump());
		}

		return Net::MessageFactory::CreateMessage(MessageTypes::FileLocation, MessageStatus::FAILURE, JsonMSGDump("MESSAGE", "File is not exists"));
	}
};

struct GetFileListHandler : public Net::MessageHandler<Net::PeecMessage>
{
	std::shared_ptr<ContainerTackerServer> containerServer;

	explicit GetFileListHandler(std::shared_ptr<ContainerTackerServer> _containerServer) : containerServer(_containerServer) {}

	Net::PeecMessage handle(Net::OWN_MSG_PTR<Net::PeecMessage> _msg) override
	{
		if (!containerServer->filesList.empty())
		{
			return Net::MessageFactory::CreateMessage(MessageTypes::GetFileList, MessageStatus::SUCCESS, containerServer->GetFilesJSON().dump());
		}
		return Net::MessageFactory::CreateMessage(MessageTypes::GetFileList, MessageStatus::FAILURE);
	}
};