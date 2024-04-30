#pragma once

#include <PeecHandler.hpp>
#include <PeecMessageTypes.hpp>
#include <PeecCommonHeaders.hpp>

#include "PeecServerFileContainer.hpp"

class DownloadHandler : public Net::MessageHandler<MessageTypes>
{
public:
	std::shared_ptr<FileContainerMap> containerServer;

	explicit DownloadHandler(std::shared_ptr<FileContainerMap> _container) : containerServer(_container) {}

	Net::Message<MessageTypes> handle(Net::OWN_MSG_PTR<MessageTypes> _msg) override
	{
		JSON jsonRequestData = JSON::parse(_msg->remoteMsg.GetStrData());

		if (containerServer->find(jsonRequestData["FILENAME"]) != containerServer->end())
		{
			spdlog::info("Found");
		}

		return Net::MessageFactory<MessageTypes>::CreateMessage(MessageTypes::DownloadFile, MessageStatus::FAILURE);
	}
};