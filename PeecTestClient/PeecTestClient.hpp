#pragma once

#include <network/ClientInterface.hpp>
#include <utils/Utils.hpp>

#include <PeecMessageTypes.hpp>
#include <PeecMessageStatus.hpp>

class TestClient : public Net::ClientInterface<MessageTypes, MessageStatus> 
{
public:
	Net::OWN_MSG_PTR<MessageTypes, MessageStatus> RequestServerPing()
	{
		Net::Message<MessageTypes, MessageStatus> msg;

		msg.SetType(MessageTypes::ServerPing);

		Send(msg);

		return WaitingResponse();
	}

	Net::OWN_MSG_PTR<MessageTypes, MessageStatus> RequestFileRegistration()
	{
		Net::Message<MessageTypes, MessageStatus> msg;

		msg.SetType(MessageTypes::FileRegistration);

		JSON jsonMsg;
		jsonMsg["FILENAME"] = "THISFILE";
		jsonMsg["FILELENGTH"] = 32;
		jsonMsg["ADDRESS"] = GetAddress();
		jsonMsg["PORT"] = GetPort();
		msg << jsonMsg.dump();

		Send(msg);

		return WaitingResponse();
	}

	Net::OWN_MSG_PTR<MessageTypes, MessageStatus> RequestFileLocation()
	{
		Net::Message<MessageTypes, MessageStatus> msg;

		msg.SetType(MessageTypes::FileLocation);

		JSON jsonMsg;
		jsonMsg["FILENAME"] = "THISFILE";
		msg << jsonMsg.dump();

		Send(msg);

		return WaitingResponse();
	}

	Net::OWN_MSG_PTR<MessageTypes, MessageStatus> RequestGetFilesList()
	{
		Net::Message<MessageTypes, MessageStatus> msg;

		msg.SetType(MessageTypes::GetFileList);

		Send(msg);

		return WaitingResponse();
	}


	Net::OWN_MSG_PTR<MessageTypes, MessageStatus> WaitingResponse()
	{
		Incoming().wait();
		return Incoming().pop_front();
	}
};