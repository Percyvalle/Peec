#include <iostream>

#include <PeecCommandParser.hpp>

#include "PeecClientPeerIMPL.hpp"
#include "PeecServerPeerIMPL.hpp"

void PrintReply(const std::string& _data, const std::string& _status, const uint32_t& _type)
{
	spdlog::info("Reply: {0} Status: {1} Type: {2}", _data, _status, _type);
}

int main(int argc,char** argv)
{
	ServerIMPL sPeer(20720, 0);
	sPeer.Start();

	ClientIMPL cPeer;
	cPeer.Connect("127.0.0.1", 20228);

	Utils::CommandParser commandParser;
	commandParser.AddCommand("list");
	commandParser.AddCommand("location");
	commandParser.AddCommand("reg");
	commandParser.AddExitCommand("exit");

	commandParser.ExecuteParse();

	while (true)
	{
		if (!commandParser.Incoming().empty())
		{
			Utils::Command command = commandParser.Incoming().pop_front();

			if (command.titleCommand == "exit")
			{
				sPeer.Stop();
				cPeer.Disconnect();
				return EXIT_SUCCESS;
			}

			if (command.titleCommand == "location" && !command.arguments.empty())
			{
				JSON jsonReq;
				jsonReq["FILENAME"] = command.arguments[0];

				Net::OWN_MSG_PTR<MessageTypes> msg = cPeer.GETFileLocation(jsonReq);
				PrintReply(msg->remoteMsg.GetStrData(), Utils::ConvertStatusToStr(msg->remoteMsg.GetStatus()), static_cast<uint32_t>(msg->remoteMsg.GetType()));
			}

			if (command.titleCommand == "reg")
			{
				FileS::PathStruct path(command.arguments[0]);

				if (FileS::FileManager::FileExists(path))
				{
					JSON jsonReq;
					jsonReq["FILENAME"] = path.GetPathFileName();
					jsonReq["FILELENGTH"] = path.GetFileLenght();

					Net::OWN_MSG_PTR<MessageTypes> msg = cPeer.RegistrationFile(jsonReq);
					PrintReply(msg->remoteMsg.GetStrData(), Utils::ConvertStatusToStr(msg->remoteMsg.GetStatus()), static_cast<uint32_t>(msg->remoteMsg.GetType()));
				
					if (msg->remoteMsg.GetStatus() == MessageStatus::SUCCESS)
					{
						sPeer.AddRegisteredFile(path.GetPathFileName(), path.GetPath(), path.GetFileLenght());
					}
				}
				else
				{
					spdlog::error("File [{}] doesn't exist", path.GetPath());
				}
			}

			if (command.titleCommand == "list")
			{
				Net::OWN_MSG_PTR<MessageTypes> msg = cPeer.GETFilesList();
				PrintReply(msg->remoteMsg.GetStrData(), Utils::ConvertStatusToStr(msg->remoteMsg.GetStatus()), static_cast<uint32_t>(msg->remoteMsg.GetType()));
			
#ifdef _DEBUG
				std::vector<std::string> containerServer = sPeer.GetContainer();
				for (const std::string& i : containerServer)
				{
					spdlog::info("[ {} ]", i);
				}
#endif
			}
		}
	}

	return EXIT_SUCCESS;
}