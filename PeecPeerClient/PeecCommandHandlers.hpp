#pragma once

#include <PeecCommandParser.hpp>
#include <PeecStructMessage.hpp>
#include <PeecMessageTypes.hpp>
#include <PeecUtils.hpp>

#include "PeecClientPeerIMPL.hpp"
#include "PeecServerPeerIMPL.hpp"

void PrintReply(const std::string& _data, const std::string& _status, const uint32_t& _type)
{
	spdlog::info("Reply: {0} Status: {1} Type: {2}", _data, _status, _type);
}

void CommandList(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer, std::shared_ptr<ServerIMPL> _speer)
{
	Net::OWN_MSG_PTR<MessageTypes> msg = _cpeer->GETRequest(MessageTypes::GetFileList);
	PrintReply(msg->remoteMsg.GetStrData(), Utils::ConvertStatusToStr(msg->remoteMsg.GetStatus()), static_cast<uint32_t>(msg->remoteMsg.GetType()));

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

		Net::OWN_MSG_PTR<MessageTypes> msg = _cpeer->POSTRequest(MessageTypes::FileLocation, jsonReq);
		PrintReply(msg->remoteMsg.GetStrData(), Utils::ConvertStatusToStr(msg->remoteMsg.GetStatus()), static_cast<uint32_t>(msg->remoteMsg.GetType()));
	}
}

void CommandRegistration(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer, std::shared_ptr<ServerIMPL> _speer)
{
	if (!_command.arguments.empty())
	{
		FileS::PathStruct path(_command.arguments[0]);

		if (FileS::FileManager::FileExists(path))
		{
			JSON jsonReq;
			jsonReq["FILENAME"] = path.GetPathFileName();
			jsonReq["FILELENGTH"] = path.GetFileLenght();

			Net::OWN_MSG_PTR<MessageTypes> msg = _cpeer->POSTRequest(MessageTypes::FileRegistration, jsonReq);
			PrintReply(msg->remoteMsg.GetStrData(), Utils::ConvertStatusToStr(msg->remoteMsg.GetStatus()), static_cast<uint32_t>(msg->remoteMsg.GetType()));
					
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
		JSON jsonReq;
		jsonReq["FILENAME"] = _command.arguments[0];

		ClientIMPL cPeerr;
		cPeerr.Connect("127.0.0.1", static_cast<uint16_t>(std::stoi(_command.arguments[1])));

		Net::OWN_MSG_PTR<MessageTypes> msg = cPeerr.POSTRequest(MessageTypes::DownloadFile, jsonReq);
		PrintReply(msg->remoteMsg.GetStrData(), Utils::ConvertStatusToStr(msg->remoteMsg.GetStatus()), static_cast<uint32_t>(msg->remoteMsg.GetType()));
		cPeerr.Disconnect();
	}
}

void CommandExit(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer, std::shared_ptr<ServerIMPL> _speer, std::shared_ptr<Utils::CommandParser> _parser)
{
	_parser->StopParse();
	_cpeer->Disconnect();
	_speer->Stop();
}