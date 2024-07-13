#pragma once

#include <network/MessageInterface.hpp>
#include <utils/CommandParser.hpp>
#include <utils/Utils.hpp>

#include <PeecMessageTypes.hpp>

#include "PeecClientPeerIMPL.hpp"
#include "PeecServerPeerIMPL.hpp"

void PrintReply(const std::string& _data, MessageStatus _status, MessageTypes _type)
{
	spdlog::info("Reply: {0} Status: {1} Type: {2}", _data, static_cast<uint32_t>(_status), static_cast<uint32_t>(_type));
}

void CommandList(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer, std::shared_ptr<ServerIMPL> _speer)
{
	Net::OWN_MSG_PTR<Net::PeecMessage> msg = _cpeer->GETRequest(MessageTypes::GetFileList);
	PrintReply(msg->remoteMsg.ConvertToString(), msg->remoteMsg.GetStatus(), msg->remoteMsg.GetType());

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
	JSON jsonReq;
	jsonReq["FILENAME"] = _command.arguments[0];

	Net::OWN_MSG_PTR<Net::PeecMessage> msg = _cpeer->POSTRequest(MessageTypes::FileLocation, jsonReq);
	PrintReply(msg->remoteMsg.ConvertToString(), msg->remoteMsg.GetStatus(), msg->remoteMsg.GetType());
}

void CommandRegistration(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer, std::shared_ptr<ServerIMPL> _speer)
{
	FileS::PathStruct path(_command.arguments[0]);

	if (!FileS::FileSystemManager::FileExists(path))
	{
		spdlog::error("File [{}] doesn't exist", path.GetPath());
		return;
	}

	JSON jsonReq;
	jsonReq["FILENAME"] = path.GetPathFileName();
	jsonReq["FILELENGTH"] = path.GetFileLenght();
	jsonReq["ADDRESS"] = _speer->GetAddress();
	jsonReq["PORT"] = _speer->GetPort();

	Net::OWN_MSG_PTR<Net::PeecMessage> msg = _cpeer->POSTRequest(MessageTypes::FileRegistration, jsonReq);
	PrintReply(msg->remoteMsg.ConvertToString(), msg->remoteMsg.GetStatus(), msg->remoteMsg.GetType());
					
	if (msg->remoteMsg.GetStatus() == MessageStatus::FAILURE)
	{
		spdlog::error("failed registration file: {0}", msg->remoteMsg.ConvertToJson()["MESSAGE"]);
		return;
	}

	_speer->AddRegisteredFile(path.GetPathFileName(), path.GetPath(), path.GetFileLenght());
}


void DownloadFilePart(const std::string& _filename, const std::string& _address, const uint16_t& _port, const size_t& _start, const size_t& _end, std::vector<char*>& _buffer)
{
}

void CommandDownloadFile(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer)
{
	FileS::PathStruct path(_command.arguments[0]);

	JSON jsonReq;
	jsonReq["FILENAME"] = path.GetPathFileName();

	Net::OWN_MSG_PTR<Net::PeecMessage> msgPorts = _cpeer->POSTRequest(MessageTypes::FileLocation, jsonReq);
	JSON jsonRespPorts = msgPorts->remoteMsg.ConvertToJson();

	if (msgPorts->remoteMsg.GetStatus() == MessageStatus::FAILURE)
	{
		spdlog::error("failed to get port: {0}", jsonRespPorts["MESSAGE"]);
		return;
	}

	ClientIMPL peerConnection;
	peerConnection.Connect(jsonRespPorts.front()["ADDRESS"], jsonRespPorts.front()["PORT"]);

	Net::OWN_MSG_PTR<Net::PeecMessage> msg = peerConnection.POSTRequest(MessageTypes::DownloadFile, jsonReq);
		
	if (msg->remoteMsg.GetStatus() == MessageStatus::FAILURE)
	{
		spdlog::error("failed download file: {0}", msg->remoteMsg.ConvertToJson()["MESSAGE"]);
		return;
	}
	
	FileS::FileIO file(path, std::ios::out | std::ios::binary);
	if (!file.IsOpen())
	{
		spdlog::error("failed writing data to file: {0}", path.GetPathFileName());
		return;
	}

	file.Write(reinterpret_cast<const char*>(msg->remoteMsg.Body().Data().data()), msg->remoteMsg.BSize());
}

void CommandExit(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer, std::shared_ptr<ServerIMPL> _speer, std::shared_ptr<Utils::CommandParser> _parser)
{
	_parser->StopParse();
	_cpeer->Disconnect();
	_speer->Stop();
}