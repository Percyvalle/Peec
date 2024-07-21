#pragma once

#include <network/Handler.hpp>
#include <filesystem/FileSystem.hpp>

#include <PeecStructMessage.hpp>
#include <PeecCommonHeaders.hpp>

#include <PeecChunk.hpp>
#include "PeecServerFileContainer.hpp"

#include <fstream>

class DownloadHandler : public Net::MessageHandler<Net::PeecMessage>
{
public:
	std::shared_ptr<FileContainerMap> containerServer;

	explicit DownloadHandler(std::shared_ptr<FileContainerMap> _container) : containerServer(_container) {}

	Net::PeecMessage handle(Net::OWN_MSG_PTR<Net::PeecMessage> _msg) override
	{
		JSON jsonRequestData = _msg->remoteMsg.ConvertToJson();

		if (!Utils::ValidateExistsVarJSON(jsonRequestData, "FILENAME", "OFFSETS", nullptr))
		{
			return Net::MessageFactory::CreateMessage(MessageTypes::DownloadFile,
													  MessageStatus::FAILURE,
													  JsonMSGDump("MESSAGE", "Invalidate data"));
		}

		if (containerServer->find(jsonRequestData["FILENAME"]) != containerServer->end())
		{
			FileS::FileIO file(containerServer->at(jsonRequestData["FILENAME"]).filePath, std::ios::in | std::ios::binary);
			if (!file.IsOpen())
			{
				return Net::MessageFactory::CreateMessage(MessageTypes::DownloadFile,
														  MessageStatus::FAILURE,
														  JsonMSGDump("MESSAGE", "Open is failed"));
			}

			std::size_t fileSize = file.Size();
			std::size_t chunkRead = CHUNK_SIZE;
			std::size_t chunkOffsets = jsonRequestData["OFFSETS"];

			if ((chunkOffsets + CHUNK_SIZE) > fileSize)
			{			  
				chunkRead = fileSize - chunkOffsets;
			}
			std::vector<Net::byte_type> buffer(chunkRead);
			
			file.Read(reinterpret_cast<char*>(buffer.data() + chunkOffsets), chunkRead);

			return Net::MessageFactory::CreateMessage(MessageTypes::DownloadFile, MessageStatus::SUCCESS, buffer);
		} 

		return Net::MessageFactory::CreateMessage(MessageTypes::DownloadFile, MessageStatus::FAILURE);
	}
};