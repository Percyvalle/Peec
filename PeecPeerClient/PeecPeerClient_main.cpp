#include <iostream>

#include "PeecCommandHandlers.hpp"

int main(int argc,char** argv)
{
	std::shared_ptr<ServerIMPL> sPeer = std::make_shared<ServerIMPL>(std::stoi(argv[1]), Utils::GetCountCPU());
	sPeer->Start();

	std::shared_ptr<ClientIMPL> cPeer = std::make_shared<ClientIMPL>();
	cPeer->Connect("127.0.0.1", 20228);

	std::shared_ptr<Utils::CommandParser> commandParser = std::make_shared<Utils::CommandParser>();
	commandParser->RegisterCommand("list", CommandList, cPeer, sPeer);
	commandParser->RegisterCommand("location", CommandLocation, cPeer);
	commandParser->RegisterCommand("reg", CommandRegistration, cPeer, sPeer);
	commandParser->RegisterCommand("load", CommandDownloadFile, cPeer);
	commandParser->RegisterCommand("exit", CommandExit, cPeer, sPeer, commandParser);

	commandParser->ExecuteParse();

	return EXIT_SUCCESS;
}