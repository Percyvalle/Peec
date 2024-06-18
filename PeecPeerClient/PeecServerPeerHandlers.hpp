#pragma once

#include <network/Handler.hpp>
#include <filesystem/FileSystem.hpp>

#include <PeecMessageTypes.hpp>
#include <PeecMessageStatus.hpp>
#include <PeecCommonHeaders.hpp>

#include "PeecServerFileContainer.hpp"

#include <fstream>

class DownloadHandler : public Net::MessageHandler<MessageTypes, MessageStatus>
{
public:
	std::shared_ptr<FileContainerMap> containerServer;

	explicit DownloadHandler(std::shared_ptr<FileContainerMap> _container) : containerServer(_container) {}

	Net::Message<MessageTypes, MessageStatus> handle(Net::OWN_MSG_PTR<MessageTypes, MessageStatus> _msg) override
	{
		JSON jsonRequestData = JSON::parse(_msg->remoteMsg.GetStrData());

		if (!Utils::ValidateExistsVarJSON(jsonRequestData, "FILENAME", nullptr))
		{
			return Net::MessageFactory<MessageTypes, MessageStatus>::CreateMessage(MessageTypes::DownloadFile,
																	MessageStatus::FAILURE,
																	JsonMSGDump("MESSAGE", "Invalidate data"));
		}

		if (containerServer->find(jsonRequestData["FILENAME"]) != containerServer->end())
		{
			FileS::PathStruct path(containerServer->at(jsonRequestData["FILENAME"]).filePath);
	
			FileS::FileIO file(path, std::ios::in | std::ios::binary);
			if (file.IsOpen())
			{
				std::vector<std::uint8_t> buffer(file.Size());

				file.Read(reinterpret_cast<char*>(buffer.data()), buffer.size());
				return Net::MessageFactory<MessageTypes, MessageStatus>::CreateMessage(MessageTypes::DownloadFile, MessageStatus::SUCCESS, buffer);
			}
			else
			{
				return Net::MessageFactory<MessageTypes, MessageStatus>::CreateMessage(MessageTypes::DownloadFile,
																		MessageStatus::FAILURE,
																		JsonMSGDump("MESSAGE", "Open is failed"));
			}
		} 

		return Net::MessageFactory<MessageTypes, MessageStatus>::CreateMessage(MessageTypes::DownloadFile, MessageStatus::FAILURE);
	}
};