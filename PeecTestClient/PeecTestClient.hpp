#pragma once

#include <PeecClientInterface.hpp>
#include <PeecCommonHeaders.hpp>
#include <PeecMessageTypes.hpp>
#include "PeecUtils.hpp"

class TestClient : public Net::ClientInterface<MessageTypes> 
{
public:
	Net::OWN_MSG_PTR<MessageTypes> RequestServerPing()
	{
		Net::Message<MessageTypes> msg;

		msg.SetType(MessageTypes::ServerPing);

		Send(msg);

		return WaitingResponse();
	}

	Net::OWN_MSG_PTR<MessageTypes> RequestFileRegistration()
	{
		Net::Message<MessageTypes> msg;

		msg.SetType(MessageTypes::FileRegistration);

		JSON jsonMsg;
		jsonMsg["FILENAME"] = "THISFILE";
		jsonMsg["FILELENGTH"] = 32;
		msg << jsonMsg.dump();

		Send(msg);

		return WaitingResponse();
	}

	Net::OWN_MSG_PTR<MessageTypes> RequestFileLocation()
	{
		Net::Message<MessageTypes> msg;

		msg.SetType(MessageTypes::FileLocation);

		JSON jsonMsg;
		jsonMsg["FILENAME"] = "THISFILE";
		msg << jsonMsg.dump();

		Send(msg);

		return WaitingResponse();
	}

	Net::OWN_MSG_PTR<MessageTypes> RequestGetFilesList()
	{
		Net::Message<MessageTypes> msg;

		msg.SetType(MessageTypes::GetFileList);

		Send(msg);

		return WaitingResponse();
	}


	Net::OWN_MSG_PTR<MessageTypes> WaitingResponse()
	{
		Incoming().wait();
		return Incoming().pop_front();
	}
};