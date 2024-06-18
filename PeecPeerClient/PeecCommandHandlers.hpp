#pragma once

#include <utils/CommandParser.hpp>
#include <network/StructMessage.hpp>
#include <utils/Utils.hpp>

#include <PeecMessageTypes.hpp>

#include "PeecClientPeerIMPL.hpp"
#include "PeecServerPeerIMPL.hpp"

void PrintReply(const std::string& _data, const std::string& _status, const uint32_t& _type)
{
	spdlog::info("Reply: {0} Status: {1} Type: {2}", _data, _status, _type);
}

void CommandList(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer, std::shared_ptr<ServerIMPL> _speer)
{
	Net::OWN_MSG_PTR<MessageTypes, MessageStatus> msg = _cpeer->GETRequest(MessageTypes::GetFileList);
	// PrintReply(msg->remoteMsg.GetStrData(), Utils::ConvertStatusToStr(msg->remoteMsg.GetStatus()), static_cast<uint32_t>(msg->remoteMsg.GetType()));

#ifdef _DEBUG
	std::vector<std::string> containerServer = _speer->GetContainer();
	for (const std::string& i : containerServer)
	{
		spdlog::info("[ {} ]", i);
	}
#endif
}

void CommandLocation(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer)
{
	if (!_command.arguments.empty())
	{
		JSON jsonReq;
		jsonReq["FILENAME"] = _command.arguments[0];

		Net::OWN_MSG_PTR<MessageTypes, MessageStatus> msg = _cpeer->POSTRequest(MessageTypes::FileLocation, jsonReq);
		// PrintReply(msg->remoteMsg.GetStrData(), Utils::ConvertStatusToStr(msg->remoteMsg.GetStatus()), static_cast<uint32_t>(msg->remoteMsg.GetType()));
	}
}

void CommandRegistration(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer, std::shared_ptr<ServerIMPL> _speer)
{
	if (!_command.arguments.empty())
	{
		FileS::PathStruct path(_command.arguments[0]);

		if (FileS::FileSystemManager::FileExists(path))
		{
			JSON jsonReq;
			jsonReq["FILENAME"] = path.GetPathFileName();
			jsonReq["FILELENGTH"] = path.GetFileLenght();
			jsonReq["ADDRESS"] = _speer->GetAddress();
			jsonReq["PORT"] = _speer->GetPort();

			Net::OWN_MSG_PTR<MessageTypes, MessageStatus> msg = _cpeer->POSTRequest(MessageTypes::FileRegistration, jsonReq);
			// PrintReply(msg->remoteMsg.GetStrData(), Utils::ConvertStatusToStr(msg->remoteMsg.GetStatus()), static_cast<uint32_t>(msg->remoteMsg.GetType()));
					
			if (msg->remoteMsg.GetStatus() == MessageStatus::SUCCESS)
			{
				_speer->AddRegisteredFile(path.GetPathFileName(), path.GetPath(), path.GetFileLenght());
			}
		}
		else
		{
			spdlog::error("File [{}] doesn't exist", path.GetPath());
		}
	}
}

void CommandDownloadFile(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer)
{
	if (!_command.arguments.empty())
	{
		FileS::PathStruct path(_command.arguments[0]);

		JSON jsonReq;
		jsonReq["FILENAME"] = path.GetPathFileName();

		Net::OWN_MSG_PTR<MessageTypes, MessageStatus> msgPorts = _cpeer->POSTRequest(MessageTypes::FileLocation, jsonReq);
		JSON jsonRespPorts = msgPorts->remoteMsg.GetJSONData();

		if (msgPorts->remoteMsg.GetStatus() == MessageStatus::SUCCESS)
		{
			ClientIMPL peerConnection;
			peerConnection.Connect(jsonRespPorts.back()["ADDRESS"], jsonRespPorts.back()["PORT"]);

			Net::OWN_MSG_PTR<MessageTypes, MessageStatus> msg = peerConnection.POSTRequest(MessageTypes::DownloadFile, jsonReq);
		
			if (msg->remoteMsg.GetStatus() == MessageStatus::SUCCESS)
			{
				FileS::FileIO file(path, std::ios::out | std::ios::binary);
				if (file.IsOpen())
				{
					file.Write(reinterpret_cast<const char*>(msg->remoteMsg.body.data.data()), msg->remoteMsg.body.data.size());
				}
				else
				{
					spdlog::error("Failed writing data to file: {0}", path.GetPathFileName());
				}
			}
			else
			{
				spdlog::error("Failed download file: {0}", msg->remoteMsg.GetJSONData()["MESSAGE"]);
			}
		}
		else
		{
			spdlog::error("Failed to get port: {0}", jsonRespPorts["MESSAGE"]);
		}
	}
}

void CommandExit(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer, std::shared_ptr<ServerIMPL> _speer, std::shared_ptr<Utils::CommandParser> _parser)
{
	_parser->StopParse();
	_cpeer->Disconnect();
	_speer->Stop();
}