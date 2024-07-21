#pragma once

#include <network/MessageInterface.hpp>
#include <utils/CommandParser.hpp>
#include <utils/ThreadPool.hpp>
#include <utils/Utils.hpp>

#include <PeecMessageTypes.hpp>
#include <PeecChunk.hpp>

#include "PeecClientPeerIMPL.hpp"
#include "PeecServerPeerIMPL.hpp"

void PrintReply(const std::string& _data, MessageStatus _status, MessageTypes _type) {
	spdlog::info("Reply: {0} Status: {1} Type: {2}", _data, static_cast<uint32_t>(_status), static_cast<uint32_t>(_type));
}

void CommandList(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer, std::shared_ptr<ServerIMPL> _speer) {
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

void CommandLocation(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer) {
	JSON jsonReq;
	jsonReq["FILENAME"] = _command.arguments[0];

	Net::OWN_MSG_PTR<Net::PeecMessage> msg = _cpeer->POSTRequest(MessageTypes::FileLocation, jsonReq);
	PrintReply(msg->remoteMsg.ConvertToString(), msg->remoteMsg.GetStatus(), msg->remoteMsg.GetType());
}

void CommandRegistration(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer, std::shared_ptr<ServerIMPL> _speer) {
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


void DownloadFilePart(const std::string& _filename, const std::string& _address, const uint16_t& _port, const size_t& _offsets, std::shared_ptr<std::vector<Net::byte_type>> _buffer) {
	ClientIMPL peerConnection;
	peerConnection.Connect(_address, _port);

	if (!peerConnection.IsConnected()) {
		return;
	}

	JSON jsonReqDownload;
	jsonReqDownload["FILENAME"] = _filename;
	jsonReqDownload["OFFSETS"] = _offsets;

	Net::OWN_MSG_PTR<Net::PeecMessage> msg = peerConnection.POSTRequest(MessageTypes::DownloadFile, jsonReqDownload);
	
	if (msg->remoteMsg.GetStatus() == MessageStatus::FAILURE) {
		spdlog::error("failed download file: {0}", msg->remoteMsg.ConvertToJson()["MESSAGE"]);
		return;
	}

	std::copy(msg->remoteMsg.Body().Data().begin(), msg->remoteMsg.Body().Data().end(), _buffer->begin() + _offsets);
}

void CommandDownloadFile(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer) {
	std::string fileName = _command.arguments[0];

	JSON jsonReqLocation;
	jsonReqLocation["FILENAME"] = fileName;

	Net::OWN_MSG_PTR<Net::PeecMessage> msgPorts = _cpeer->POSTRequest(MessageTypes::FileLocation, jsonReqLocation);
	JSON jsonRespPorts = msgPorts->remoteMsg.ConvertToJson();

	if (msgPorts->remoteMsg.GetStatus() == MessageStatus::FAILURE)
	{
		spdlog::error("failed to get port: {0}", jsonRespPorts["MESSAGE"]);
		return;
	}
	
	std::size_t fileLength = jsonRespPorts.back()["FILELENGTH"];
	std::size_t countChunks = CalculateChunkCount(fileLength);
	std::size_t countPeers = jsonRespPorts.size() - 1;
	std::shared_ptr<std::vector<Net::byte_type>> bufferChunks = std::make_shared<std::vector<Net::byte_type>>(fileLength);
	
	DWORD cpuCount = Utils::GetCountCPU();
	Pool::ThreadPool loadThread{cpuCount};

	for (uint32_t i = 0; i < countChunks; i++)
	{

		std::string address = jsonRespPorts.at(i % countPeers)["ADDRESS"];
		std::uint16_t port = jsonRespPorts.at(i % countPeers)["PORT"];
		std::size_t offsets = i * CHUNK_SIZE;

		loadThread.Submit(&DownloadFilePart, fileName, address, port, offsets, bufferChunks);
	}

	loadThread.Join();

 	FileS::FileIO file(fileName, std::ios::out | std::ios::binary);
	if (!file.IsOpen())
	{
		spdlog::error("failed writing data to file: {0}", fileName);
		return;
	}

	file.Write(reinterpret_cast<const char*>(bufferChunks->data()), bufferChunks->size());
}

void CommandExit(const Utils::Command& _command, std::shared_ptr<ClientIMPL> _cpeer, std::shared_ptr<ServerIMPL> _speer, std::shared_ptr<Utils::CommandParser> _parser) {
	_parser->StopParse();
	_cpeer->Disconnect();
	_speer->Stop();
}