#include <iostream>
#include <memory>

#include "PeecClientPeerIMPL.hpp"
#include "PeecServerPeerIMPL.hpp"
#include "PeecCommandHandlers.hpp"

#include <utils/CommandParser.hpp>

int main(int argc,char** argv)
{
	std::shared_ptr<ServerIMPL> sPeer = std::make_shared<ServerIMPL>(std::stoi(argv[1]), Utils::GetCountCPU());
	sPeer->Start();
	sPeer->ExecuteUpdateServer(true);

	std::shared_ptr<ClientIMPL> cPeer = std::make_shared<ClientIMPL>();
	cPeer->Connect("127.0.0.1", 20228);

	std::shared_ptr<Utils::CommandParser> commandParser = std::make_shared<Utils::CommandParser>();
	commandParser->RegisterCommand("list", CommandList, cPeer, sPeer);
	commandParser->RegisterCommand("location", CommandLocation, cPeer)->RequiredArgs();
	commandParser->RegisterCommand("reg", CommandRegistration, cPeer, sPeer)->RequiredArgs();
	commandParser->RegisterCommand("load", CommandDownloadFile, cPeer)->RequiredArgs();
	commandParser->RegisterCommand("exit", CommandExit, cPeer, sPeer, commandParser);
	commandParser->ExecuteParse();

	return EXIT_SUCCESS;
}