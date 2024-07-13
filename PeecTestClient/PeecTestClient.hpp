#pragma once

#include <network/ClientInterface.hpp>
#include <utils/Utils.hpp>

#include <PeecStructMessage.hpp>

class TestClient : public Net::ClientInterface<Net::PeecMessage> 
{
public:
	Net::OWN_MSG_PTR<Net::PeecMessage> RequestServerPing()
	{
		Net::PeecMessage msg;

		msg.SetType(MessageTypes::ServerPing);

		Send(msg);

		return WaitingResponse();
	}

	Net::OWN_MSG_PTR<Net::PeecMessage> RequestFileRegistration()
	{
		Net::PeecMessage msg;

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

	Net::OWN_MSG_PTR<Net::PeecMessage> RequestFileLocation()
	{
		Net::PeecMessage msg;

		msg.SetType(MessageTypes::FileLocation);

		JSON jsonMsg;
		jsonMsg["FILENAME"] = "THISFILE";
		msg << jsonMsg.dump();

		Send(msg);

		return WaitingResponse();
	}

	Net::OWN_MSG_PTR<Net::PeecMessage> RequestGetFilesList()
	{
		Net::PeecMessage msg;

		msg.SetType(MessageTypes::GetFileList);

		Send(msg);

		return WaitingResponse();
	}


	Net::OWN_MSG_PTR<Net::PeecMessage> WaitingResponse()
	{
		Incoming().wait();
		return Incoming().pop_front();
	}
};