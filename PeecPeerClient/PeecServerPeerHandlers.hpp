#pragma once

#include <PeecHandler.hpp>
#include <PeecMessageTypes.hpp>
#include <PeecCommonHeaders.hpp>

#include "PeecServerFileContainer.hpp"

#include <fstream>

class DownloadHandler : public Net::MessageHandler<MessageTypes>
{
public:
	std::shared_ptr<FileContainerMap> containerServer;

	explicit DownloadHandler(std::shared_ptr<FileContainerMap> _container) : containerServer(_container) {}

	Net::Message<MessageTypes> handle(Net::OWN_MSG_PTR<MessageTypes> _msg) override
	{
		JSON jsonRequestData = JSON::parse(_msg->remoteMsg.GetStrData());

		if (!Utils::ValidateExistsVarJSON(jsonRequestData, "FILENAME", nullptr))
		{
			return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::DownloadFile,
																	MessageStatus::FAILURE,
																	JSON::parse(R"({"MESSAGE":"Invalidate data"})").dump());
		}

		if (containerServer->find(jsonRequestData["FILENAME"]) != containerServer->end())
		{
			FileS::PathStruct path(containerServer->at(jsonRequestData["FILENAME"]).filePath);

			FileS::FileIO file(path, std::ios::in | std::ios::binary);
			if (file.IsOpen())
			{
				std::vector<std::uint8_t> buffer(file.Size());

				file.Read(reinterpret_cast<char*>(buffer.data()), buffer.size());
				return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::DownloadFile, MessageStatus::SUCCESS, buffer);
			}
			else
			{
				return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::DownloadFile,
																		MessageStatus::FAILURE,
																		JSON::parse(R"({"MESSAGE":"Open is failed"})").dump());
			}
		} 
		else
		{
			return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::DownloadFile,
																	MessageStatus::FAILURE,
																	JSON::parse(R"({"MESSAGE":"File not registered"})").dump());
		}

		return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::DownloadFile, MessageStatus::FAILURE);
	}
};