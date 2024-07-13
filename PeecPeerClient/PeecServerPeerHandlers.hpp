#pragma once

#include <network/Handler.hpp>
#include <filesystem/FileSystem.hpp>

#include <PeecStructMessage.hpp>
#include <PeecCommonHeaders.hpp>

#include "PeecServerFileContainer.hpp"

#include <fstream>

class DownloadHandler : public Net::MessageHandler<Net::PeecMessage>
{
public:
	std::shared_ptr<FileContainerMap> containerServer;

	explicit DownloadHandler(std::shared_ptr<FileContainerMap> _container) : containerServer(_container) {}

	Net::PeecMessage handle(Net::OWN_MSG_PTR<Net::PeecMessage> _msg) override
	{
		JSON jsonRequestData = JSON::parse(_msg->remoteMsg.ConvertToString());

		if (!Utils::ValidateExistsVarJSON(jsonRequestData, "FILENAME", nullptr))
		{
			return Net::MessageFactory::CreateMessage(MessageTypes::DownloadFile,
													  MessageStatus::FAILURE,
													  JsonMSGDump("MESSAGE", "Invalidate data"));
		}

		if (containerServer->find(jsonRequestData["FILENAME"]) != containerServer->end())
		{
			FileS::PathStruct path(containerServer->at(jsonRequestData["FILENAME"]).filePath);
	
			FileS::FileIO file(path, std::ios::in | std::ios::binary);
			if (!file.IsOpen())
			{
				return Net::MessageFactory::CreateMessage(MessageTypes::DownloadFile,
														  MessageStatus::FAILURE,
														  JsonMSGDump("MESSAGE", "Open is failed"));
			}

			std::vector<std::uint8_t> buffer(file.Size());
			file.Read(reinterpret_cast<char*>(buffer.data()), buffer.size());

			return Net::MessageFactory::CreateMessage(MessageTypes::DownloadFile, MessageStatus::SUCCESS, buffer);
		} 

		return Net::MessageFactory::CreateMessage(MessageTypes::DownloadFile, MessageStatus::FAILURE);
	}
};