#pragma once

#include "PeecCommonHeaders.hpp"
#include "PeecQueue.hpp"

#include <Windows.h>
#include <sstream>

namespace Utils
{
	struct Command
	{
		std::string fullCommand;
		std::string titleCommand;
		std::vector<std::string> arguments;

		explicit Command() = default;
		explicit Command(const std::string& _input) : fullCommand(_input)
		{
			ParsingCommandLine();
		}

	private:
		void ParsingCommandLine()
		{
			std::istringstream iss(fullCommand);
			iss >> titleCommand;

			std::string arg;
			while (iss >> arg)
			{
				arguments.push_back(arg);
			}
		}
	};

	class CommandParser
	{
	private:
		enum class ParserStates
		{
			RUNNING,
			STOPPED
		};

		std::thread parseThread;
		std::unordered_set<std::string> exitCommands;
		std::unordered_set<std::string> existingCommands;
		std::atomic<ParserStates> parserState = ParserStates::STOPPED;

		Utils::QueueLF<Command> commandQueue;

	public:
		CommandParser() = default;
		~CommandParser()
		{
			StopParse();
		}

		void AddCommand(const std::string& _command)
		{
			existingCommands.insert(_command);
		}

		void AddExitCommand(const std::string& _command)
		{
			existingCommands.insert(_command);
			exitCommands.insert(_command);
		}

		void ExecuteParse()
		{
			parserState = ParserStates::RUNNING;
			if (!parseThread.joinable())
			{
				parseThread = std::thread(
					[this]()
					{
						GettingCommandLine();
					});
			}
		}

		void StopParse()
		{
			parserState = ParserStates::STOPPED;
			if (parseThread.joinable())
			{
				parseThread.join();
			}
		}

		Utils::QueueLF<Command>& Incoming()
		{
			return commandQueue;
		}

	private:
		void GettingCommandLine()
		{
			while (parserState == ParserStates::RUNNING)
			{
				std::string input;
				std::getline(std::cin, input);
	
				Command inputCommand(input);
				if (existingCommands.find(inputCommand.titleCommand) != existingCommands.end())
				{
					commandQueue.push_back(inputCommand);
					
					if (exitCommands.find(inputCommand.titleCommand) != exitCommands.end())
					{
						parserState = ParserStates::STOPPED;
						break;
					}
				}
			}
		}
	};
}